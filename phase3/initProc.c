/* initProc.c
   Virtual memory is set up for the Kaya OS. Segment and page tables are
   created and set up in order to get virtual memory working. Our device
   semaphores are created initialized to allow for mutual exclusion.
   User processes are created and initialized to be executed to run
   on the OS. 
*/

#include "../h/const.h"
#include "../h/types.h"
#include "/usr/local/include/umps2/umps/libumps.e"
#include "../e/exceptions.e"
#include "../e/sysSupport.e"

/* init global vars */
pgTableOS_t ksegOS;
pgTable_t kuseg2;
pgTable_t kuseg3;
framePool_t framePool[FRAMESIZE];
int swapSem, masterSem;
userProcData_t userProcs[MAXUPROC];

/* init our for 49 sema4s */
int semTable[SEMNUM];

HIDDEN void initUProc();
unsigned int getAsid();
HIDDEN void diskOp(int * sem, int diskNum, int readOrWrite);

void debug(int a){
  return;
}

/* Set up our OS page table, frame pool, and initialize the device semaphores
   for mutual exclusion. Then, create our user processes to be run within
   initUProc.
 */
void test(){
  int i, j;
  segTable_t * segTable;
  state_PTR uProc;

  masterSem = 0;
  
  /*set up OS page table*/
  ksegOS.header = ((PTEMAGICNO) << 24) |  KSEGNUMOS;
  for(i = 0; i < KSEGNUMOS; i++){
    ksegOS.ptes[i].entryHi = (0x20000 + i) << 12;
    ksegOS.ptes[i].entryLow = ((0x20000 + i) << 12) | DIRTY | VALID | GLOBAL;
  }

  /* set up framePool */
  for(i = 0; i < FRAMESIZE; i++){
    framePool[i].procID = -1; /* asid */
    framePool[i].segNum = 0;
    framePool[i].pgNum = 0;
    framePool[i].pte = NULL;
  }

  /* setup our semaphores */
  for(i = 0; i < SEMNUM; i++){
    semTable[i] = 1;
  }
  
  /* create our processes*/
  for(i = 0; i < MAXUPROC; i++){

    /* find the user process that we want to work with and set up its kuseg2 */
    userProcs[i-1].kuSeg2.header = (PTEMAGICNO << 24) | KSEGNUM;
    for(j = 0; j < KSEGNUM; j++){
      userProcs[i-1].kuSeg2.ptes[j].entryHi = ((0x80000 + j) << 12) | (i << 6);
      userProcs[i-1].kuSeg2.ptes[j].entryLow = ALLOFF | DIRTY;
    }
    
    /* fix last entry's entryhi */
    userProcs[i-1].kuSeg2.ptes[KSEGNUM - 1].entryHi = (0xBFFFF << 12) | (i << 6);

    /* set the sem for the uProc we're on */
    userProcs[i-1].sem = 0;
    
    /* set the appropriate entries in the global segment table */
    segTable = (segTable_t*) (SEGSTARTADDR + (i * SEGWIDTH));
    segTable->ksegOS = &ksegOS;  
    segTable->kuseg2 = &(userProcs[i-1].kuSeg2);

    
    debug(1);
    
    /* u-proc initialization, see Kaya 4.7 */
    uProc->s_asid = getAsid(); /* you know, i really didn't think this is what would defeat me */
    debug(2);
    uProc->s_pc = (memaddr) initUProc;
    uProc->s_t9 = (memaddr) initUProc;
    uProc->s_sp = KUSEG3FIRSTPAGE;
    uProc->s_status = ALLOFF | IEPON | TEON | VMPOFF | KERPON;
    /*create the process we've been working with, with a sys1 */
    debug(3);
    SYSCALL(CREATEPROC, (int)&uProc, 0, 0);
  }

  /* p the master sem */
  for(i=0; i < MAXUPROC; i++){
    SYSCALL(PASSERN, (int)&masterSem, 0, 0);
  }

  /* SEND THE PROCESS TO THE CHIAR!!! */
  SYSCALL(TERMINATEPROC, 0, 0, 0);

}

/* Takes the created process and reads the tape data onto the disk. Then,
   does 3 syscall 5s for the trap types and the appropriate handler for the
   sys trap will be called, (tlbManagementHandler, pgmTraphandler, or
   userSyscallhandler). After this, we set the user process up to be started
   and load the OS with it 
*/
HIDDEN void initUProc(){
  unsigned int asid = getAsid();
  int i;
  device_PTR disk, tape;
  state_PTR uProc;
  state_PTR tempState;

  debug(4);
  
  /* get our disk and tape device we're on 
  disk = (device_t*) (INTDEVREG + ((DISKINT-NOSEMS) * DEVREGSIZE * EIGHTPERDEV) + (0 * DEVREGSIZE));
  */
  readTape(asid);

  /* set up and do our 3 sys 5's */
  for(i = 0; i < 3; i++){

    /* get the process' state and get what trap we're on */
    tempState = &(userProcs[asid-1].newTraps[i]);

    /* set up the process' asid*/
    tempState->s_asid = getENTRYHI();

    /* put the process into the right status so we can do the 3 sys 5s */
    /* interrupts enabled, kernal mode on, timer on, and virtual memroy on */
    tempState->s_status = (ALLOFF | IECON | IEPON | IMASKON | KERPON | TEON | VMPON);

    /* depending on the trap, set up the uProc accordingly */
    switch(i){

      /* tlb trap */
      case(0):
	tempState->s_t9 = (memaddr) tlbManagementHandler;
	tempState->s_pc = (memaddr) tlbManagementHandler;
	break;

      /* prog trap */
      case(1):
	tempState->s_t9 = (memaddr) pgmTrapHandler;
	tempState->s_pc = (memaddr) pgmTrapHandler;
        break;

      /* sys trap */
      case(2):
	tempState->s_t9 = (memaddr) userSyscallHandler;
	tempState->s_pc = (memaddr) userSyscallHandler;
        break;
    }

    /* set our sp and then call sys 5 */
    tempState->s_sp = PROGSTARTADDR;
    SYSCALL(SPECTRAPVEC, i, (int)&(userProcs[asid - 1].oldTraps[i]), (int)tempState);
  }

  /* get ready to execute the process */
  uProc->s_pc = (memaddr) PROGSTARTADDR;
  uProc->s_t9 = (memaddr) PROGSTARTADDR;
  uProc->s_sp = (memaddr) KUSEG3FIRSTPAGE; /* also last page of kuseg2 */
  uProc->s_asid = getENTRYHI();

  /* reset our process' status so we can continue */
  /* interrupts enabled, user mode on, local timer on, VM off */
  uProc->s_status = ALLOFF | IECON | IEPON | IMASKON | KERPOFF | TEON | VMPOFF;

  LDST(uProc);
}

/* Gets the asid of the process. */
unsigned int getAsid(){

  /* get the process' ID and return that */
  unsigned int asid = getENTRYHI();
  asid = (asid & ASIDENTRYHI) >> 6;
  return asid;
}

/* Reads the tape until we have reached the end of the tape or the end of
   file and and makes a call to write that file to the disk. */
/* may not work */
HIDDEN void readTape(unsigned int asid){
  device_PTR tape;
  
  /* get our tape device that we'll be reading from */
  tape = (device_PTR) (INTDEVREG + ((TAPEINT-NOSEMS) * DEVREGSIZE * EIGHTPERDEV) + ((asid-1) * DEVREGSIZE));

  /* go read our tape */
  while(tape->d_data1 != EOT || tape->d_data1 != EOF){

    /* WAIT while reading in from the tape and going to the disk to write */
    SYSCALL(WAITIO, TAPEINT, asid-1, 0);

    tape->d_command = READBLK;
    diskOp(asid-1, tape->d_data1, WRITEBLK);
  }
}

/* Performs a read or a write for the disk device. Either stores the 
   information from the tape into the backing store or reads from the backing
   store, providing the OS with the data needed at the time. */
/*  doesn't work -> not completed */
HIDDEN void diskOp(int * sem, int diskNum, int readOrWrite){

  /* go get the address of the disk */
  device_PTR disk = (device_PTR) (INTDEVREG + ((DISKINT-NOSEMS) * DEVREGSIZE * EIGHTPERDEV) + (diskNum * DEVREGSIZE));

  /* WAIT while we mess with the disk */
  SYSCALL(PASSERN, (int) sem, 0,0);

  /* turn interrupts off */
  setSTATUS(getSTATUS() | IEPOFF | IMASKOFF);

  /* do the actual disk operation */
  
  
  
  /* enable interrupts again now that we're done with the disk*/
  setSTATUS(getSTATUS() | IEPON | IMASKON);

  /* SIGNAL that we're done with the disk */
  SYSCALL(VERHOGEN, (int)sem, 0, 0);
  
}
 
