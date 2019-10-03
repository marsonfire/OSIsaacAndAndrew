#include "../h/const.h"
#include "../h/types.h"
#include "../e/asl.e"
#include "../e/pcb.e"
#include "../e/initial.e"
#include "../e/scheduler.e"




void sysCallHandler(){
  state *beingUsed;
  int kernel;
  int request;
  request = passer -> s_a0;
  if(kernel){/*user mode*/
    /*shit and stuff*/
  }
  else{/*kernel mode*/
    switch(request){
    case CREATEPROC:
      SYSCALL1(beingUsed)
      break;
    case TERMINATEPROC:
      SYSCALL2();
      break;
    case VERHOGEN:
      SYSCALL3(beingUsed);
      break;
    case PASSERN:
      SYSCALL4(beingUsed);
      break;
    case SPECTRAPVEC:
      SYSCALL5(beingUsed);
      break;
    case GETCPUTIME:
      SYSCALL6(beingUsed);
      break;
    case WAITCLOCK:
      SYSCALL7(beingUsed);
      break;
    case WAITIO:
      SYSCALL8(beingUsed);
      break;
    default:
      PANIC();
  }
}

  /* decide if using *state or *semaddr for syscall arguments*/
  
/*Create process - 1 syscall*/
int SYSCALL1(state_t *statep){
  pcb_PTR temp = allocPcb();
  /*check for null, panic if so*/

  /*taken from initial.c*/
  processCount = processCount + 1;

  /*insert proc series*/
  /*load old state with LDST maybe, but want to reduc LDST use*/
  
}

/*Terminate process - 2 syscall*/
void SYSCALL2(){
  /*check for children. if none, execute*/
  /*else, recur syscall2 somehow*/
  /*each execytion will be followed by processCount deincrement*/
  processCount = processcount - 1;

  /*may want to call scheduler afterwards*/
}

/*Perform operation on semaphore, set 3 in a0 - 3 syscall*/
void SYSCALL3(int *semaddr){
  return NULL;
}

/*Perform P operation on semaphore - 4 syscall*/
void SYSCALL4(int *semaddr){
  return NULL;
}

/*Specify vector and a bunch of other crap - 5 syscall*/
void SYSCALL5(int type, state_t *oldp, state_t *newp){
  return NULL;
}

/*Gets the time, - 6 syscall*/
cpu_t SYSCALL6(){
  return NULL;
}

/*Hold the clock - 7 syscall*/
void SYSCALL7(){
  return NULL;
}

/*Hold an IO? - 8 syscall*/
unsigned int SYSCALL8(int intlNo, int dnum, int waitRead){
  retunr NULL;
}
