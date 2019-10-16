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
extern int semd[MAGICNUM];       /* our 49 devices */
extern cpu_t startTOD; /* time the process started at */
extern cpu_t stopTOD;  /* time the process stopped at */

void sysCallHandler(){
     /*get what was in the old state */
    state_PTR oldState = (state_PTR)SYSCALLBREAKOLD;
    /* increment pc by 1 */
    oldState->s_pc = oldState->s_pc + 4;
	/* status of the state */
      unsigned int status = oldState->s_status;
      /*the syscall request that was made */
      unsigned int request = oldState->s_a0;

      if(request > 8){
	passUpOrDie();
      }
      /* if we're in this, we're in user mode trying to make a request here */
      else if((request >= 1 && request <= 8) && (status & KERPOFF) != ALLOFF) {
	/* beginning of vid 7 is good for this */
	/* copy state from oldSys over to oldProgram, but need to get oldProgram's state first */
	state_PTR temp = (state_PTR) PROGRAMTRAPOLD;
	copyState(oldState, temp);
	/* set the cause register to be reserved instruction exception (which is 10) (00...0100100 = 0x00000028)  */
	/* set the cause to be reserved instruction, must reset it first */
	temp->s_cause = temp->s_cause & 0x00000000;
	temp->s_cause = temp->s_cause | 0x00000028;
	/* call program trap handler */
	pgmTrapHandler();
      }
      /* we're in kernal mode and need to do some syscall */
      else{
	switch(request){
	case CREATEPROC:
	  sysCall1(oldState);
	  break;
	case TERMINATEPROC:
	  sysCall2();
	  break;
	case VERHOGEN:
	  sysCall3(oldState);
	  break;
	case PASSERN:
	  sysCall4(oldState);
	  break;
	case SPECTRAPVEC:
	  sysCall5(oldState);
	  break;
	case GETCPUTIME:
	  sysCall6(oldState);
	  break;
	case WAITCLOCK:
	  sysCall7(oldState);
	  break;
	case WAITIO:
	  sysCall8(oldState);
	  break;
	default:
	  /*don't think this will ever actually happen */
	  passUpOrDie(oldState, SYSTRAP);
	}
      }
  }

  /* decide if using *state or *semaddr for syscall arguments*/
  
/*Create process - 1 syscall*/
int sysCall1(state_PTR statep){
  pcb_PTR temp = allocPcb();
  /*check for null, set v0 to -1 if so*/
  if(temp == NULL){
    statep->s_v0 = -1;
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

/* Check through our current process  and recursively kill its children processes and all things associated with it in the semaphore lists and ready queue. */  
void killEverything(pcb_PTR p){
  /* MANIACAL LAUGHTER */
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

/*Perform V operation on semaphore, set 3 in a0 - 3 syscall*/
/* this is the signal operation */
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
/* this is the Wait operation */
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
void sysCall5(state_PTR statep){
  /*get exception stored in a1 register */
  switch(statep->s_a1){
    /* for each case, check if trap new area is null in the current process. If it isn't kill it. Then set the values of the the old and new area to be the addresses of the exceptions, which is passed in the a2 (old state) and a3 (new state) register */
    case TLBTRAP:
      if(currentProcess->newTlb != NULL){
	sysCall2();
      }
      currentProcess->oldTlb = (state_PTR)statep->s_a2;
      currentProcess->newTlb = (state_PTR)statep->s_a3;
      break;
    case PROGTRAP:
      if(currentProcess->newPgm != NULL){
	sysCall2();
      }
      currentProcess->oldPgm = (state_PTR)statep->s_a2;
      currentProcess->newPgm = (state_PTR)statep->s_a3;
      break;
    case SYSTRAP:
      if(currentProcess->newSys != NULL){
	sysCall2();
      }
      currentProcess->oldSys = (state_PTR)statep->s_a2;
      currentProcess->newSys = (state_PTR)statep->s_a3;
      break;
  }
  LDST(statep);
}

/*Gets the time, - 6 syscall*/
void sysCall6(state_PTR statep){
  /* get the current time into var time */
  cpu_t time;
  STCK(time);
  /* set the process' time */
  currentProcess->p_time = (currentProcess->p_time) + (time - startTOD);
  /* store the time the process took into the state's v0 */
  statep->s_v0 = currentProcess->p_time;
  /* update the start time */
  STCK(startTOD);
  LDST(statep);
}

/*Hold the clock - 7 syscall*/
void sysCall7(state_PTR statep){
  /* get the interval timer (last in the list of semaphores) */
  /* basically, do a sys4 call but with different values for the interval timer */
  int * sem = (int*) &(semd[MAGICNUM-1]);
  (*sem)--;
  if((*sem) < 0){
    insertBlocked(sem, currentProcess);
    /* copy over old statep into the currentProcess' state */
    copyState(statep, &(currentProcess->p_state));
    /* now this process will be soft blocked */
    softBlockCount++;
  }
  scheduler();
}

/*Hold an IO? - 8 syscall*/
void sysCall8(state_PTR statep){
  /* get line number, device number, and terminal read operation from a registers*/
  int lineNum = statep->s_a1;
  int deviceNum = statep->s_a2;
  int terminalReadOp = statep->s_a3;
  /* we want lineNum values between 3 an 7, so kill off anything else */
  if(lineNum < DISKINT || lineNum > TERMINT){
    sysCall2();
  }
  /* calculating which device it is */
  int semDeviceIndex;
  /* check if it's a terminal read first */
  if(lineNum == TERMINT && terminalReadOp == TRUE){
    /* line we're at, - 3 (cuz we start at device 3), + terminalReadOp */
    semDeviceIndex = lineNum - 3 + terminalReadOp;
  }
  else{
    /* line we're at - 3, and terminalOp is 0, so don't add it */
    semDeviceIndex = lineNum - 3;
  }
  /* we have 8 of each of the devices... so we need to do:
     (8 * semDeviceIndex) + deviceNum to find the right one */
  semDeviceIndex = (8 * semDeviceIndex) + deviceNum;
  /* basically does a sysCall 4 with these values on this device now... similar to how it was done with the interval timer in syscall 7 */
  int * semDevice = &(semd[semDeviceIndex]);
  (*semDevice)--;
  if((*semDevice) < 0){
    insertBlocked(semDevice, currentProcess);
    /* copy over statep into the currentProcess' state */
    copyState(statep, &(currentProcess->p_state));
    /* process is blocked so increase softBlockCount */
    softBlockCount++;
    /* call the scheduler */
    scheduler();
  }
  LDST(statep);
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

void passUpOrDie(state_PTR statep, int trapCode){
  /* need to check the code that was passed along in order to make sure we handle this correclty
 -- TLBTRAP = 0, PROGTRAP = 1, SYSTRAP = 2 --> defined in const.h already for us to use */
  /* in each the new state must != NULL, and we need to copy the state 
over from the given statep, to the currentProcess' old state and then load the 
new state */
  /*vid 8 (and class) */
  switch(trapCode){
  case TLBTRAP:
    /* do tlb trap thing */
    if(currentProcess->newTlb != NULL){
      copyState(statep, currentProcess->oldTlb);
      LDST(currentProcess->newTlb);
    }
    else{
      sysCall2();
    }
    break;
  case PROGTRAP:
    /* do progtrap thing */
    if(currentProcess->newPgm != NULL){ 
      copyState(statep, currentProcess->oldPgm);
      LDST(currentProcess->newPgm);
    }
    else{
      sysCall2();
    }
    break;
  case SYSTRAP:
    /* do syscalltrap thing*/
    if(currentProcess->newSys != NULL){
      copyState(statep, currentProcess->oldSys);
      LDST(currentProcess->newSys);
    }
    else{
      sysCall2();
    }
    break;
  }
  /* don't like repetitive code, but only want to kill processes when we want to, and not accidentally... */
}

void pgmTrapHandler(){
  /* get the old program trap area and send it along to pass up or die */
  passUpOrDie((state_PTR)PROGRAMTRAPOLD, PROGTRAP);
}

void tlbManagementHandler(){
  /*get the old tlb management area and sent it along to pass up or die */
  passUpOrDie((state_PTR)TLBMANAGEMENTOLD, TLBTRAP);
}
