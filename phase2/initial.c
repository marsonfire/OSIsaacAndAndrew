/* initial.c */
#include "../h/types.h"
#include "../h/const.h"
#include "../e/pcb.e"
#include "../e/asl.e"
#include "../e/scheduler.e"
#include "../e/exceptions.e"
#include "../e/initial.e"
#include "../e/interrupts.e"
#include "../e/p2test.e"
#include "/usr/local/include/umps2/umps/libumps.e"

int processCount;         /* number of processes in the system */
int softBlockCount;    /* number of processes blocked and waiting for an interrupt */    
pcb_PTR currentProcess;   /* self explanatory... I hope... */
pcb_PTR readyQ;         /* tail pointer to queue of procblks representing processes ready and waiting for execution */
int semd[MAGICNUM];     /* how we get the devices, MAGICNUM was mentioned in class and is in consts.h as 49 */

void debug(int a){
  int i;
  i = 0;
}


int main(){
  debug(111);
  devregarea_t* ramBaseAddress; 
  int i;
  state_PTR syscallNew;
  state_PTR programTrapNew;
  state_PTR tlbManagementNew;
  state_PTR interruptNew;
  unsigned int RAMTOP;
  pcb_PTR p;

  /* init and set up a bunch of our vars */
  /* He calls this RAMTOP in the video but it's the same address (0x1000000)*/
  ramBaseAddress = (devregarea_t *) RAMBASEADDR;
  /* set up RAMPTOP */
  RAMTOP = (ramBaseAddress->rambase) + (ramBaseAddress->ramsize);
  /*init asl and pcb*/
  initASL();
  initPcbs();

  /*global variables*/
  processCount = 0;
  softBlockCount = 0;
  currentProcess = NULL;
  readyQ = mkEmptyProcQ();

  /* need to initalize and set each device to 0 */
  for(i = 0; i <MAGICNUM; i++){
    semd[i] = 0;
  }

  /* set up and create new syscall area in memory */
  /* set pc and t9 */
  /* set sp to RAMTOP and set status to ALLOFF */
  syscallNew = (state_PTR) SYSCALLBREAKNEW;
  syscallNew->s_pc = (memaddr) sysCallHandler; 
  syscallNew->s_t9 = (memaddr) sysCallHandler;
  syscallNew->s_sp = RAMTOP;
  syscallNew->s_status = ALLOFF;

  /* set up and create new programTrap area in memory */
  /* set pc and t9 */
  /* set sp to RAMTOP and set status to ALLOFF */
  programTrapNew = (state_PTR) PROGRAMTRAPNEW;
  programTrapNew->s_pc = (memaddr) pgmTrapHandler;
  programTrapNew->s_t9 = (memaddr) pgmTrapHandler;
  programTrapNew->s_sp = RAMTOP;
  programTrapNew->s_status = ALLOFF;

  /* set up and create new tlbManagement area in memory */
  /* set pc and t9 */
  /* set sp to RAMTOP and set status to ALLOFF */
  tlbManagementNew = (state_PTR) TLBMANAGEMENTNEW;
  tlbManagementNew->s_pc = (memaddr) tlbManagementHandler;
  tlbManagementNew->s_t9 = (memaddr) tlbManagementHandler;
  tlbManagementNew->s_sp = ramBaseAddress->rambase + ramBaseAddress->ramsize;
  tlbManagementNew->s_status = ALLOFF;

  /* set up and create new interrupt area in memory */
  /* set pc and t9 */
  /* set sp to RAMTOP and set status to ALLOFF */
  interruptNew = (state_PTR) INTERRUPTNEW;
  interruptNew->s_pc = (memaddr) interruptHandler;
  interruptNew->s_t9 = (memaddr) interruptHandler;
  interruptNew->s_sp = RAMTOP;
  interruptNew->s_status = ALLOFF;  

  /*get a pcb*/
  p = allocPcb();
  /*incrememnt process, since we just created it*/
  processCount++;
  /* set the values that we need for our currentProcess to be initialized */
  currentProcess->p_state.s_sp = (RAMTOP - PAGESIZE);
  currentProcess->p_state.s_pc = (memaddr) test; /*from p2test */
  currentProcess->p_state.s_t9 = (memaddr) test;
  currentProcess->p_state.s_status = ALLOFF | IECON | TEON | IMASKON;
  /*insert into the readyQueue */
  insertProcQ(&readyQ, p);
  /* make currentProcess nul again and then  start hte interval timer */
  currentProcess = NULL;
  LDIT(INTERVALTIMER);
  /*start up scheduler*/
  scheduler();

  return -1;
}
