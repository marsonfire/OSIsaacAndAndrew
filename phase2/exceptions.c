/* ========== exceptions.c ==========
* Exceptions.c takes the various exceptions that can occur (syscall, 
* TLB, and program traps), and handles them through the correct handler.  
* A majority of excpetions.c is handling the various syscalls and 
* what they mean. Exceptions.c handles creation and destroying of processes
* along with determining if those processes need to be signaled or told
* to wait. Furthermore, exceptions.c handles clock exceptions when the
* clocks are done, and waiting for an I/O device. 
*/
#include "../h/const.h"
#include "../h/types.h"
#include "../e/asl.e"
#include "../e/pcb.e"
#include "../e/initial.e"
#include "../e/scheduler.e"
#include "/usr/local/include/umps2/umps/libumps.e"

/* ===== Start Initial Global Variables ===== */
extern int processCount;         /* number of processes in the system */
extern int softBlockCount;       /* number of processes blocked and waiting for an interrupt */
extern pcb_PTR currentProcess;   /* self explanatory... I hope... */
extern pcb_PTR readyQ;           /* tail pointer to queue of procblks representing processes ready and waiting for execution */
extern int semd[SEMNUM];       /* our 49 devices */
/* ===== End Initial Global Variables ===== */
/* ===== Start Scheduler Global Variables ===== */
extern cpu_t startTOD;           /* time the process started at */
extern cpu_t stopTOD;            /* time the process stopped at */
/* ===== End Scheduler Global Variables ===== */

/* Local module helper function declaration */
HIDDEN void sysCall1(state_PTR statep);
HIDDEN void sysCall2();
HIDDEN void killEverything(pcb_PTR p);
HIDDEN void sysCall3(state_PTR statep);
HIDDEN void sysCall4(state_PTR statep);
HIDDEN void sysCall5(state_PTR statep);
HIDDEN void sysCall6(state_PTR statep);
HIDDEN void sysCall7(state_PTR statep);
HIDDEN void sysCall8(state_PTR statep);
HIDDEN void passUpOrDie(int trapCode);

/* Module functions that will be available to other classes */
void copyState(state_PTR original, state_PTR dest);
void pgmTrapHandler();
void tlbManagementHandler();

/* ===== start sysCallHandler() =====  */
/*
 * ==Function: When a syscall is made, sysCallHandler will take the 
 * syscall request and call the appropriate sysCall function for that
 * request. If not in kernal mode, will instead raise a privileged 
 * instruction error and call the program trap handler. 
 * ==Arguments: None
 */
void sysCallHandler(){
    /* local vars */
    state_PTR oldState;
    state_PTR tempState;
    unsigned int status;
    unsigned int privilegedInst;
    int request;
    /*initialize vars */
    /*get what was in the old state */
    oldState = (state_PTR)SYSCALLBREAKOLD;
    /* increment pc by 1 */
    oldState->s_pc = oldState->s_pc + 4;
    /* status of the state */
    status = oldState->s_status;
    /*the syscall request that was made */
    request = oldState->s_a0;
    /* if we're in this, we're in user mode trying to make a request here */
    if((request >= 1 && request <= 8) && ((status & KERPOFF) != ALLOFF)) {
    /* copy state from oldSys over to oldProgram, but need to get oldProgram's state first */
      tempState = (state_PTR) PROGRAMTRAPOLD;
      copyState(oldState, tempState);
      /* set the cause register to be reserved instruction exception (which is 10) (00...0100100 = 0x00000028)  */
      /* set the cause to be reserved instruction, must reset it first */
      privilegedInst = (tempState->s_cause) & (0xFF);
      tempState->s_cause = privilegedInst | PRIVINSTADD; 
      /* call program trap handler */
      pgmTrapHandler();
    }
    /* we're in kernal mode and need to do some syscall -> could be user mode with syscall > 8 */
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
          passUpOrDie(SYSTRAP);
      }
    }
  }
  
/* ===== Start sysCall1() =====  */
/*
 * ==Function: When called and if there's space for a process to be created, 
 * a new process is created from the free list of PCB's. That new process will
 * added to the ready queue to be worked on. 
 * ==Arguments: state_PTR statep - gets the current state that made the sys call
 */
HIDDEN void sysCall1(state_PTR statep){
  pcb_PTR temp = allocPcb();
  /*check for null, set v0 to -1 if so*/
  if(temp == NULL){
    /* means we don't have any free processes to take */
    statep->s_v0 = FAIL;
  }
  else{
    /*increment since we're gonna add one */
    processCount = processCount + 1;
    /* insert process as a child of current process */
    insertChild(currentProcess, temp);
    /* add process to the ready queue */
    insertProcQ(&(readyQ), temp);
    /*need to copy all the 32 registers and the state over */
    copyState((state_PTR)statep->s_a1, &(temp->p_state));
    /* since it worked out, lets set v0 to 0 */
    statep->s_v0 = 0;
  }
  /*load old state with LDST */
  LDST(statep);
}

/* ===== Start sysCall2() =====  */
/*
 * ==Function: Gets the current process running and ends it. All children
 * processes are ended too. 
 * ==Arguments: none
 */
HIDDEN void sysCall2(){
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

/* ===== Start killEverything() =====  */
/*
 * ==Function: Helper function for sysCall2 to make recursion easier.
 * Recursively checks the process for any children and destroys each of 
 * them. 
 * ==Arguments: pcb_PTR p - process to be killed
 */
HIDDEN void killEverything(pcb_PTR p){
  /* MANIACAL LAUGHTER */
  while(!emptyChild(p)){
    killEverything(removeChild(p));
  }
  /* check if semAdd is null or not */
  if(p->p_semAdd != NULL){
    /* get the address of the semaphore to be used later */
    int* sem = p->p_semAdd;
    outBlocked(p);
    /* one less process that's waiting */
    if(sem >= &(semd[0]) && sem <= &(semd[SEMNUM - 1])){
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

/* ===== Start sysCall3() =====  */
/*
 * ==Function: A V (signal) operation on the semaphore. Checks if a process can 
 * be unblocked and inserted onto the ready queue to be executed.  
 * ==Arguments: state_PTR statep - gets the current state that made the sys call
 */
HIDDEN void sysCall3(state_PTR statep){
  pcb_PTR temp;
  int* mutex;
  /* save off the a1 register */
  mutex = (int*) statep->s_a1;
  /*increment */
  (*(mutex))++;
  if((*(mutex)) <= 0 ){
    /* unblock the process */
    temp = removeBlocked(mutex);
    if(temp != NULL){
      /* put the process in the ready queue to be executed */
      insertProcQ(&(readyQ), temp);
    }
  }
  LDST(statep);
}

/* ===== Start sysCall4() =====  */
/*
 * ==Function: A P (wait) operation on the semaphore. Checks to see if the 
 * shared resource is being used by another process. This makes a process
 * blocked and is forced to wait if resource is being used.    
 * ==Arguments: state_PTR statep - gets the current state that made the sys call
 */
HIDDEN void sysCall4(state_PTR statep){
  cpu_t pTime;
  /* save off the a1 register */
  int* mutex = (int*) statep->s_a1;
  (*(mutex))--;
  if((*(mutex)) < 0){
    /* need to store off the time and see how long the process took */
    STCK(pTime);
    currentProcess->p_time = currentProcess->p_time + (pTime - startTOD);
    /* something is using the resource at the same time, so this process is currently 
    blocked and we'll need to call the scheduler and we'll think about it later */
    copyState(statep, &(currentProcess->p_state));
    insertBlocked(mutex, currentProcess);
    scheduler();
  }
  LDST(statep);
}

/* ===== Start sysCall5() =====  */
/*
 * ==Function: Specifies the exception state vector and kills process if the
 * new space in memory isn't null. If null, save the contents of a2 (old processor
 * state) and a3 (new processor state) are saved into the associated new and old 
 * areas. 
 * ==Arguments: state_PTR statep - gets the current state that made the sys call
 */
HIDDEN void sysCall5(state_PTR statep){
  /*get exception stored in a1 register */
  switch(statep->s_a1){
    /* for each case, check if trap new area is null in the current process. 
    If it isn't kill it. Then set the values of the the old and new area to be the 
    addresses of the exceptions, which is passed in the a2 (old state) and a3 (new state) register */
    case TLBTRAP:
      if(currentProcess->newTlb != NULL){
        sysCall2();
      }
      else{
      	currentProcess->oldTlb = (state_PTR)statep->s_a2;
      	currentProcess->newTlb = (state_PTR)statep->s_a3;
      }
      break;
    case PROGTRAP:
      if(currentProcess->newPgm != NULL){
	      sysCall2();
      }
      else{
      	currentProcess->oldPgm = (state_PTR)statep->s_a2;
      	currentProcess->newPgm = (state_PTR)statep->s_a3;
      }
      break;
    case SYSTRAP:
      if(currentProcess->newSys != NULL){
        sysCall2();
      }
      else{
      	currentProcess->oldSys = (state_PTR)statep->s_a2;
      	currentProcess->newSys = (state_PTR)statep->s_a3;
      }
      break;
  }
  LDST(statep);
}

/* ===== Start sysCall6() =====  */
/*
 * ==Function: The total time the process took is recorded and saved off
 * in order to see how much time has been used up by that process.  
 * ==Arguments: state_PTR statep - gets the current state that made the sys call
 */
HIDDEN void sysCall6(state_PTR statep){
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

/* ===== Start sysCall7() =====  */
/*
 * ==Function: Perform a P (wait) operation on the interval timer, which will be V'd
 * (signaled) every 100 milliseconds.
 * ==Arguments: state_PTR statep - gets the current state that made the sys call
 */
HIDDEN void sysCall7(state_PTR statep){
  /* get the interval timer (last in the list of semaphores) */
  /* basically, do a sys4 call but with different values for the interval timer */
  int* intervalTimerDev = (int*) &(semd[SEMNUM-1]);
  (*intervalTimerDev)--;
  if((*intervalTimerDev) < 0){
    insertBlocked(intervalTimerDev, currentProcess);
    /* copy over old statep into the currentProcess' state */
    copyState(statep, &(currentProcess->p_state));
    /* now this process will be soft blocked */
    softBlockCount++;
  }
  scheduler();
}

/* ===== Start sysCall8() =====  */
/*
 * ==Function: Wait for an I/O device. After finding the correct device and line 
 * number, will need to get that device and perform a P (wait) operation on it.
 * ==Arguments: state_PTR statep - gets the current state that made the sys call
 */
HIDDEN void sysCall8(state_PTR statep){
  /* get line number, device number, and terminal read operation from a registers*/
  int lineNum = statep->s_a1;
  int deviceNum = statep->s_a2;
  int terminalReadOp = statep->s_a3;
  int semDeviceIndex;
  int* semDevice;
  /* we want lineNum values between 3 an 7, so kill off anything else */
  if(lineNum < DISKINT || lineNum > TERMINT){
    sysCall2();
  }
  /* calculating which device it is */
  /* check if it's a terminal read first */
  if(lineNum == TERMINT && terminalReadOp == TRUE){
    /* line we're at, - 3 (cuz we start at device 3), + terminalReadOp */
    semDeviceIndex = lineNum - NOSEMS + terminalReadOp;
  }
  else{
    /* line we're at - 3, and terminalOp is 0, so don't add it */
    semDeviceIndex = lineNum - NOSEMS;
  }
  /* we have 8 of each of the devices... so we need to do:
     (8 * semDeviceIndex) + deviceNum to find the right one */
  semDeviceIndex = (EIGHTPERDEV * semDeviceIndex) + deviceNum;
  /* basically does a sysCall 4 with these values on this device now... similar to how it was done with the interval timer in syscall 7 */
  semDevice = &(semd[semDeviceIndex]);
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

/* ===== Start copyState() =====  */
/*
 * ==Function: When called and if there's space for a process to be created, 
 * a new process is created from the free list of PCB's. That new process will
 * added to the ready queue to be worked on. 
 * ==Arguments: state_PTR original - original state that will be copied over
 *              state_PTR dest - state that will contain the original state's
 *                               values 
 */
void copyState(state_PTR original, state_PTR dest){
  int i;
  dest->s_asid = original -> s_asid;
  dest->s_status = original->s_status;
  dest->s_pc = original->s_pc;
  dest->s_cause = original->s_cause;
  for(i = 0; i < STATEREGNUM; i++){
    dest->s_reg[i]=original->s_reg[i];
  }
}

/* ===== Start passUpOrDie() =====  */
/*
 * ==Function: Checks the trap code we're given and checks the new area based on
 * the trap code. If not null, must copy the copy the old state over and load the 
 * new state. Otherwise, want to kill the current process. 
 * ==Arguments: int trapCode - code passed in to tell which kind of trap we have
 *                            TLBTRAP = 0, PROGTRAP = 1, SYSTRAP = 2
 */
HIDDEN void passUpOrDie(int trapCode){
  /* need to check the code that was passed along in order to make sure we handle this correclty
 -- TLBTRAP = 0, PROGTRAP = 1, SYSTRAP = 2 --> defined in const.h already for us to use */
  /* in each the new state must != NULL, and we need to copy the state 
  over from the given statep, to the currentProcess' old state and then load the 
  new state */
  /*vid 8 (and class) */
  state_PTR statep;
  switch(trapCode){
  case TLBTRAP:
    /* do tlb trap thing */
    statep = (state_PTR)TLBMANAGEMENTOLD;
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
    statep = (state_PTR)PROGRAMTRAPOLD;
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
    statep = (state_PTR)SYSCALLBREAKOLD;
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

/* ===== Start pgmTrapHandler() =====  */
/*
 * ==Function: Calls passUpOrDie() with a program trap code. Called when a privileged 
 * instruction is called when not in kernal mode.  
 * ==Arguments: none
 */
void pgmTrapHandler(){
  passUpOrDie(PROGTRAP);
}

/* ===== Start tlbManagementHandler() =====  */
/*
 * ==Function: Calls passUpOrDie() with a tlb trap code. Called when umps2 fails
 * to translate a virtual address to the corresponding physical address in memory. 
 * ==Arguments: none
 */
void tlbManagementHandler(){
  passUpOrDie(TLBTRAP);
}
