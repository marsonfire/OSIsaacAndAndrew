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
HIDDEN void findVictim();

void test(){
  int i, j;
  state_PTR state;
  segTable_t * segTable;
  masterSem = 0;
  swapSem = 1;
  
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

    
    
    for(j = 0
    
    /* set up entry hi and entry low in kuseg2 */
    for(j = 0; j < KSEGNUM; j++){
      kuseg2->ptes[j].entryHi = 0x8000 + i ;
      kuseg2->ptes[j].entryLow = ALLOFF | DIRTY;
    }
    
    /* fix last entry's entryhi */
    kuseg2->ptes[KSEGNUM - 1}.entryHI = 0xBFFFF + i + /* a shift I think */;

    /* set the appropriate entries in the global segment table */
    segTable->ksegOS = (&ksegOS);
    segTable->kuseg3 = (&kuseg3);
    segTable->kuseg2 = /* process' kseg 2 table */
	
      /*
    state->s_sp = some stack space at i+1 sys space
    state->s_pc = stubby();
    sys1()
      */
  }
  ksegOS.header = ((0x2a) << 24) KSEGNUMOS;
  for(i = 0; i < KSEGNUMOS; i++){
    ksegOS.ptes[i].entryHi
  }
}

HIDDEN void stubby(){
  unsigned int getENTRYHI();
  int i;
  /* read from tape 
  3 sys5's to set up new areas
  1 each for 3 trap types 
  */
  for(i = 0; i < someNum; i++){
    
  }
}

HIDDEN void findVictim(){
  static int next = 0;
  return ((next + 1) % POOLSIZE);
}


