/*scheduler.c */
#include "../h/types.h"
#include "../h/const.h"
#include "../e/pcb.e"
#include "../e/asl.e"
#include "../e/initial.e"
#include "../e/exceptions.e"
#include "/usr/local/include/umps2/umps/libumps.e"

/*global vars for time. see phase2 video 14 */
cpu_t startTOD; /* time the process started at */ 
cpu_t stopTOD;  /* time the process stopped at */
/*initial.c's global varaibles */
extern int processCount;         /* number of processes in the system */
extern int softBlockCount;       /* number of processes blocked and waiting for an interrupt */
extern pcb_PTR currentProcess;   /* self explanatory... I hope... */
extern pcb_PTR readyQ;           /* tail pointer to queue of procblks representing processes ready and waiting for execution */
extern int semd[MAGICNUM];		 /* array of our devices */

void scheduler() {

  /* before doig anything, let's save off the time the process took
 	(if one was running) */
  if(currentProcess != NULL){
    /* time the process stopped */
    STCK(stopTOD);
    /* time the process took */
    currentProcess->p_time = currentProcess->p_time + (stopTOD - startTOD);
  }

  /* get the process to run off the queue */
  currentProcess = removeProcQ(&readyQ);
  if(currentProcess != NULL){
	/* store the current time off */ 
	STCK(startTOD);
	/* load a timer with value of a quantum */ 
	setTIMER(QUANTUM);
	LDST(&(currentProcess->p_state));
  }
  /* if here, the current process is null and we need to check our proc count */
  if(processCount == 0){
  	HALT();
  }
  /* scream, "FIRE FIRE FIRE FIRE FIRE FIRE!!!!!" cuz we're deadlocked*/
  if(processCount > 0 && softBlockCount == 0){
  	PANIC();
  }
  /*go into suspended animation, waiting for our jobs to become un soft blocked */
  if(processCount > 0 && softBlockCount > 0){
  /* enable interrupts and turn everything off and then enable the interrupts!*/
  	setSTATUS(getSTATUS() | ALLOFF | IECON | IEPON | IMASKON);WAIT();
  }
}
 
