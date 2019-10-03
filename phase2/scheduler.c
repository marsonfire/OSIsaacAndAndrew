/*scheduler.c */
#include "../h/types.h"
#include "../h/const.h"
#include "../e/pcb.e"
#include "../e/asl.e"
#include "../e/initial.e"
#include "/usr/local/include/umps2/umps/libumps.e"

/*global vars for time. see phase2 video 14 */
cpu_t startTOD; 
cpu_t stopTOD;
/*initial.c's global varaibles */
extern int processCount;         /* number of processes in the system */
extern int softBlockCount;       /* number of processes blocked and waiting for an interrupt */
extern pcb_PTR currentProcess;   /* self explanatory... I hope... */
extern pcb_PTR readyQ;           /* tail pointer to queue of procblks representing processes ready and waiting for execution */

void scheduler(){

	pcb_PTR currentP = removeProcQ(&readyQ);

	/* we done */
	if(processCount == 0){
		HALT();
	}

	if(currentP != NULL){
		currentProcess = currentP; /*currentProcess = the global var in initial.c */
		STCK(startTOD); /* store the current time off */
		/* load a timer with value of a quantum */
		setTIMER(QUANTUM);
		LDST(&(currentProcess->p_state));
	}

	/* scream, "FIRE FIRE FIRE FIRE FIRE FIRE!!!!!" cuz we're deadlocked*/
	if(processCount > 0 && softBlockCount == 0){
		PANIC();
	}

	/*go into suspended animation, waiting for our jobs to become un soft blocked */
	if(processCount > 0 && softBlockCount > 0){
		/* the video says enable interrupts so turn everything off and then enable the interrupts!*/
		setSTATUS(ALLOFF | IECON | IEPON | IMASKON);
		WAIT();
	}
}
 
