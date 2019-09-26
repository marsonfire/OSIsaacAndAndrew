/*scheduler.c */
#include "../h/types.h"
#include "../h/const.h"
#include "../e/pcb.e"
#include "../e/asl.e"
#include "/initial.c"

/*global vars for time. see phase2 video 14 */
cpu_t startTOD; 
cpu_t stopTOD;

void scheduler(){

	pcb_PTR currentP = removeProcQ(&readyQue);

	if(currentP != NULL){
		currentProcess = currentP; /*currentProcess = the global var in initial.c */
		STCK(startTOD);
		/* load a timer with value of a quantum */
		setTIMER(QUANTUM);
		LDST(&(currentProcess->p_state));
	}

	if(processCount == 0){
		HALT();
	}

	if(processCount > 0 && emptyProcQ(&(readyQue))){
		if(softBlockCount == 0){
			PANIC();
		}
		else{
			suspended animation
			wait bit in the status register
			so i think you set the state to "wait"
			interrupts will be enabled
		}
	}
}
 