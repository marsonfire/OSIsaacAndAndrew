/* initProc.c */

#include "../h/const.h"
#include "../h/types.h"
#include "/usr/local/include/umps2/umps/libumps.e"

/* init global vars */
pgTableOS_t ksegOS;
pgTable_t kuseg2;
pgTable_t kuseg3;
segTable_t *segTable = SEGSTARTADDRESS ;


HIDDEN void stubby();
HIDDEN void findVictim();

void test(){
  int i, j;
  state_PTR state;

  /* create our processes*/
  for(i = 0; i < MAXUPROC; i++){
    
    /* i is our asid, but asid must be 1-8 since 0 is reserved */
    state->s_asid = i + 1;

    /* set up entry hi and entry low in kuseg2 */
    for(j = 0; j < KSEGNUM; j++){
      kuseg2->ptes[j].entryHi = 0x8000 + i /* + some shift */;
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


