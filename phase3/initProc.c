/* initProc.c */

#include "../h/const.h"
#include "../h/types.h"
#include "/usr/local/include/umps2/umps/libumps.e"

/* init global vars */
pgTableOS_t ksegOS;
pgTable_t kuseg2;
pgTable_t kuseg3;
framePool_t framePool[FRAMESIZE];
int swapSem, masterSem;
userProcData_t userProcs[MAXUPROC];

/* init our for 49 sema4s */
int semTable[SEMNUM];

HIDDEN void stubby();
HIDDEN void readFileIn();


void test(){
  int i, j;
  state_PTR state;
  segTable_t * segTable;
  masterSem = 0;
  swapSem = 1;
  state_PTR uProc;
  
  /*set up OS page table*/
  ksegOS.header = ((PTEMAGICNO) << 24) KSEGNUMOS;
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
      userProcs[i-1].kuSeg2.ptes[j].entryLow = ALLOFF | DIRTYON;
    }
    
    /* fix last entry's entryhi */
    userProcs[i-1].kuseg2->ptes[KSEGNUM - 1}.entryHI = (0xBFFFF << 12) | (i << 6);

    /* set the sem for the uProc we're on */
    userProcs[i-1].sem = 0;

    /* set the appropriate entries in the global segment table */
    segTable->ksegOS = (&ksegOS);
    segTable->kuseg2 = (&(userProcs[i-1].kuSeg2));

    /* u-proc initialization, see Kaya 4.7 */
    uProc.s_asid = i << 6;
    uProc.s_pc = (memaddr) initUProc;
    uProc.s_t9 = (memaddr) initUProc;
    uProc.s_sp = KUSEG3FIRSTPAGE;
    uProc.s_status = ALLOFF | IEPON | TEON | VMPOFF | KERPON;

    /*create the process we've been working with, with a sys1 */
    SYSCALL(CREATEPROCESS, (int)&uProc, 0, 0);
  }

  /* p the master sem */
  for(i=0; i < MAXUPROC; i++){
    SYSCALL(PASSEREN, (int)&masterSem, 0, 0);
  }

  /* SEND THE PROCESS TO THE CHIAR!!! */
  SYSCALL(TERMINATEPROCESS, 0, 0, 0)

}

HIDDEN void initUProc(){
  unsigned int asid = getAsid();
  int i;
  device_PTR disk, tape;
  state_PTR uProc;
  
  /* get our disk and tape device we're on */
  disk = (device_PTR) (INTDEVREG + DISKSTARTADDR + (DEVREGSIZE * EIGHTPERDEV) + (asid-1));
  tape = (device_PTR) (INTDEVREG + TAPESTARTADDR + (DEVREGSIZE * EIGHTPERDEV));

  /* go read our file */
  while(tape->d_data1 != EOT){
    tape->d_command = READBLK;

    /* wait for everyting to be read in from the tape */
    SYSCALL(WAITIO, TAPEINT, (asid - 1), 0);
  }

  /* set up and do our 3 sys 5's */
  for(i = 0; i < 3; i++){

    /* get the process' state so we can see what trap we're on */
    uProc = &(userProcs[asid-1].newTraps[i]);

    /* set up the process' asid*/
    uProc->s_asid = getENTRYHI();

    /* put the process into the right status so we can do the 3 sys 5s */
    /* interrupts enabled, kernal mode on, timer on, and virtual memroy on */
    uProc->s_status = (ALLOFF | IECON | IEPON | IMASKON | KERPON | TEON | VMPON);

    /* actually do the sys 5's */
    switch(i){

      /* tlb trap */
      case(0):
	
	break;

      /* prog trap */
      case(1):
      
        break;

      /* sys trap */
      case(2):

        break;
    }
        
  }

  /* get ready to execute the process */
  uProc->s_pc = (memaddr)PROGSTARTADDR;
  
}

/* get the asid of the process */
unsigned int getAsid(){

  /* get the process' ID and return that */
  unsigned int asid = getENTRYHI();
  asid = (asid & 0x00000FC0) >> 6;
  return asid;
  
}
