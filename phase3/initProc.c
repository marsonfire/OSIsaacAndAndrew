/* initProc.c */

#include "../h/const.h"
#include "../h/types.h"
#include "/usr/local/include/umps2/umps/libumps.e"

/* init global vars */
pgTableOS_t ksegOS;
pgTable_t kuseg2;
pgTable_t kuseg3;



HIDDEN void stubby();


void test(){
  int i;
  state_PTR state;

  /* create our processes*/
  for(i = 0; i < MAXUPROC; i++){
    state->s_asid = i + 1;
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
