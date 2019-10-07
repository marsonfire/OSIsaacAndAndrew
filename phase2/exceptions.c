#include "../h/const.h"
#include "../h/types.h"
#include "../e/asl.e"
#include "../e/pcb.e"
#include "../e/initial.e"
#include "../e/scheduler.e"
#include "/usr/local/include/umps2/umps/libumps.e"

/*global vars */
extern int processCount;         /* number of processes in the system */
extern int softBlockCount;       /* number of processes blocked and waiting for an interrupt */
extern pcb_PTR currentProcess;   /* self explanatory... I hope... */
extern pcb_PTR readyQ;           /* tail pointer to queue of procblks representing processes ready and waiting for execution */
extern int semd[MAGICNUM]        /* our 49 devices */
extern cpu_t startTOD; /* time the process started at */
extern cpu_t stopTOD;  /* time the process stopped at */

void sysCallHandler(){
     /*get what was in the old state */
    state_PTR oldState = (state_PTR)SYSCALLBREAKOLD;
    /* increment pc by 1 */
      oldState->s_pc = oldState->s_pc + 4
	/* status of the state */
	unsigned int status = oldState->s_status;
      /*the syscall request that was made */
      unsigned int request = oldState->s_a0;

      if(request > 8){
	/*pass up or die */
      }
      /* if we're in this, we're in user mode trying to make a request here */
      else if((request >= 1 && request <= 8) && (status & KERPOFF) != ALLOFF) {
	/* we need to do a priveledged pgm trap or something, see end of vid 5 */
	/* some more in the beginning of vid 7 */
      }
      /* we're in kernal mode and need to do some syscall */
      else{
	switch(request){
	case 1:
	  sysCall1(oldState);
	  break;
	case 2:
	  sysCall2();
	  break;
	case 3:
	  sysCall3(oldState);
	  break;
	case 4:
	  SYSCALL4(beingUsed);
	  break;
	case 5:
	  SYSCALL5(beingUsed);
	  break;
	case 6:
	  SYSCALL6(beingUsed);
	  break;
	case 7:
	  SYSCALL7(beingUsed);
	  break;
	case 8:
	  SYSCALL8(beingUsed);
	  break;
	default:
	  /* pass up or die */
	}
      }
  }

  /* decide if using *state or *semaddr for syscall arguments*/
  
/*Create process - 1 syscall*/
int sysCall1(state_PTR statep){
  pcb_PTR temp = allocPcb();
  /*check for null, set v0 to -1 if so*/
  if(temp == NULL){
    temp->s_v0 = -1;
  }
  else{
    /*increment since we're gonna add one */
    processCount = processCount + 1;
    /* insert process as a child of current process */
    insertChild(currentProcess, temp);
    /* add process to the ready queue */
    insertProcQ(&(readyQ), temp);
    /*need to copy all the 32 registers and the state over */
    copyState(statep->s_a1, &(temp->p_state));
    /* since it worked out, lets set v0 to 0 */
    statep->s_v0 = 0;
  }
  /*load old state with LDST */
  LDST(statep);
}

/*Terminate process - 2 syscall*/
void sysCall2(){
  /* if current process has no children, just clear it off */
  if(emptyChild(currentProcess)){
    outChild(currentProcess);
    freePcb(currentProcess);
    processCount = processCount - 1;
  }
  /*else, continually murder them, and have lots of fun doing it*/
  else{
    killEverything(currentProcess);
  }
  currentProcess = NULL;
  /*call scheduler afterwards*/
  scheduler();
}

/* MANIACAL LAUGHTER  */
void killEverything(pcb_PTR p){
  /* MURDER AND KILL THE CHILDREN, KILL AND MURDER THE CHILDREN */
  while(!emptyChild(p)){
    killEverything(removeChild(p));
  }
  /* check if semAdd is null or not */
  if(p->p_semAdd != NULL){
    /* get the address of the semaphore to be used later */
    int * sem = p->p_semAdd;
    outBlocked(p);
    /* one less process that's waiting */
    if(sem >= &(semd[0]) && sem <= &(semd[MAGICNUM - 1])){
      softBlockCount--;
    }
    else{
      (*sem)++;
    }
  }
  else if(p == currentProcess){
    /* remove child process from the parent process */
    outChild(currentProcess);
  }
  /* remove process from the ready queue */
  else{
    outProcQ(&(readyQ), p);
  }
  /* free up the space since we're not using it anymore and decrease process count by 1 too */
  freePcb(p);
  processCount--;
}

/*Perform operation on semaphore, set 3 in a0 - 3 syscall*/
void sysCall3(state_PTR statep){
  pcb_PTR temp = NULL;
  /* save off the a1 register */
  int* sem = (int*) statep->s_a1;
  /*increment */
  (*(sem))++;
  if((*(sem)) <= 0 ){
    /* unblock the process */
    temp = removeBlocked(sem);
    if(temp != NULL){
      /* put the process in the ready queue to be executed */
      insertProcQ(&(readyQ), temp);
    }
  }
  LDST(statep);
}

/*Perform P operation on semaphore - 4 syscall*/
void sysCall4(state_PTR statep){
  /* save off the a1 register */
  int* sem = (int*) statep->s_a1;
  (*(sem))--;
  if((*(sem)) < 0){
    /* need to store off the time and see how long the process took */
    STCK(stopTOD);
    currentProcess->p_time = currentProcess->p_time + (stopTOD - startTOD);
    /* something is using the resource at the same time, so this process is currently blocked and we'll need to call the scheduler and we'll think about it later */
    copyState(statep, &(currentProcess->p_state));
    insertBlocked(sem, currentProcess);
    scheduler();
  }
  LDST(statep);
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

/* fairly simple, we just set the registers and the state to what we want it to be */
void copyState(state_PTR original, state_PTR dest){
  dest->s_asid = original -> s_asid;
  dest->s_status = original->s_status;
  dest->s_pc = original->s_pc;
  dest->s_cause = original->s_cause;
  int i;
  for(i = 0; i < STATEREGNUM; i++){
    dest->s_reg[i]=original->s_reg[i];
  }
}
