/* scheduler.e */

#include "../h/types.h"
#include "../h/const.h"
#include "../e/pcb.e"
#include "../e/initial.e"
#include "/usr/local/include/umps2/umps/libumps.e"

#ifndef scheduler
#define scheduler

extern cpu_t startTOD;
extern cpu_t stopTOD;

extern void scheduler();

#endif
/* Endline fix?*/
