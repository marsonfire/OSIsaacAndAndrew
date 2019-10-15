/* interrupts.c */
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
extern cpu_t startTOD;           /* time the process started at */
extern cpu_t stopTOD;            /* time the process stopped at */

extern void copyState(state_PTR original, state_PTR dest){

void interruptHandler(){
  state_PTR oldInterruptArea = (state_PTR)INTERRUPTOLD;
  cpu_t startTime, endTime;
  device_t * device;
  int deviceNum, lineNum = 0;
  /* figure out which interrupt line the interrupt is on */ 
  unsigned int interruptCause = (oldInterruptArea->s_cause & IMASKON) >> 8;
  /* start clock */
  STCK(startTime);
  /* this shouldn't ever happen */
  if((cause & FIRST) != 0){
    /* line 0 - multi core - won't happen because kaya doesn't support it */ 
    PANIC();
  }
  /* if this runs, a process' clock has run out and we will exit the interrupHandler and go back to the scheduler */
  else if((cause & SECOND) != 0){
    /* line 1 - processor local timer */
    done(startTime);
  }
  else if((cause & THIRD) != 0){
    /* line 2 - interval timer */
    /* start by loading interval timer with 100 miliseconds */
    LDIT(INTERVALTIMER);
    /* get interval timer from the semaphore list */
    int * sem = &(semd[MAGICNUM - 1]);
    while(headBlocked(sem) != NULL){
      /* get blocked process */
      pcb_PTR blocked = removeBlocked(sem);
      /* get tht time put in the endTime so we can calculate how long interrupt took */
      STCK(endTime);
      if(blocked != NULL){
	insertProcQ(&(readyQ), blocked);
	/* set the time the process took */
	blocked->p_time = blocked->p_time + (endTime - startTime);
	/* it won't be blocked anymore */
	softBlockCount--;
      }
    }
    /* reset it since we're done with being blocked */
    (*sem) = 0;
    done(startTime);
  }
  
  /*sets up for the disk device*/
  else if((cause & FOURTH)!= 0){
    lineNum = DISKINT;
  }
  
  /*setrs up for the tape device, a sticky issue*/
  else if((cause & FIFTH)!= 0){
    lineNum = TAPEINT;
  }
  
  /*sets up for the network device*/
  else if((cause & SIXTH)!= 0){
    lineNum = NEWINT;
  }
  
  /*sets up for the printer device*/
  else if((cause & SEVENTH)!= 0){
    lineNum = PRNTINT;
  }
  
  /*sets up for the terminal*/
  else if((cause $ EIGHTH)!= 0){
    lineNum = TERMINT;
  }
}

void done(cpu_t startTime){
  cpu_t endTime;
  state_PTR oldInterruptArea = (memaddr)INTERRUPTOLD;
  if(currentProcess != NULL){
    /* start the clockand figure out the start time of the process */
    STCK(endTime);
    startTOD = startTOD + (endTime - startTime);
    /* copy over the old interrupt area state into current process' state */
    copyState(oldInterruptArea, &(currentProcess->p_state));
    /* put this into the ready queue */
    insertProcQ(&(readyQ), currentProcess);
  }
  scheduler();
}

/*identifies the device that an interrupt is happening at*/
/*bitMap may need to be changed to unsigned*/
HIDDEN int getDevice(int bitMap){
  if(bitMap == FIRST){
    return 0;
  }
  else if(bitMap == SECOND){
    return 1;
  }
  else if(bitMap == THIRD){
    return 2;
  }
  else if(bitMap == FOURTH){
    return 3;
  }
  else if(bitMap == FIFTH){
    return 4;
  }
  else if(bitMap == SIXTH){
    return 5;
  }
  else if(bitMap == SEVENTH){
    return 6;
  }
  else if(bitMap == EIGHTH){
    return 7;
  }
}
 
}

 
