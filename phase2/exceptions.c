#include "../h/const.h"
#include "../h/types.h"
#include "../phase1/asl.c"
#include "../phase1/pcb.c"
#include "../phase2"




/*Create process - 1 syscall*/
int SYSCALL(CREATEPROC, state_t *statep){
  return NULL;
}

/*Terminate process - 2 syscall*/
void SYSCALL(TERMINATEPROC){
  return NULL;
}

/*Perform operation on semaphore, set 3 in a0 - 3 syscall*/
void SYSCALL(VERHOGEN, int *semaddr){
  return NULL;
}

/*Perform P operation on semaphore - 4 syscall*/
void SYSCALL(PASSERN, int *semaddr){
  return NULL;
}

/*Specify vector and a bunch of other crap - 5 syscall*/
void SYSCALL(SPECTRAPVEC, int type, state_t *oldp, state_t *newp){
  return NULL;
}

/*Gets the time, - 6 syscall*/
cpu_t SYSCALL(GETCPUTIME){
  return NULL;
}

/*Hold the clock - 7 syscall*/
void SYSCALL(WAITCLOCK){
  return NULL;
}

/*Hold an IO? - 8 syscall*/
unsigned int SYSCALL(WAITIO, int intlNo, int dnum, int waitRead){
  retunr NULL;
}
