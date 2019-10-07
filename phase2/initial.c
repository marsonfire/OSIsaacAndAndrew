/* initial.c */
#include "../h/types.h"
#include "../h/const.h"
#include "../e/pcb.e"
#include "../e/asl.e"
#include "../e/exceptions.e"
#include "../e/initial.e"
#include "../e/interrupts.e"
#include "../e/scheduler.e"
#include "../e/p2test.e"


int processCount;         /* number of processes in the system */
int softBlockCount;       /* number of processes blocked and waiting for an interrupt */    
pcb_PTR currentProcess;   /* self explanatory... I hope... */
pcb_PTR readyQ;         /* tail pointer to queue of procblks representing processes ready and waiting for execution */
int semd[MAGICNUM];     /* how we get the devices, MAGICNUM was mentioned in class and is in consts.h as 49 */

main(){

  devregarea_t * ramBaseAddress = (devregarea_t) RAMBASEADDR; /* He calls this RAMTOP in the video but it's the same address (0x1000000)*/

  /*set the new areas that are in constants */
  
  /* set up and create new syscall area in memory */
  /* set pc and t9 */
  /* set sp to RAMTOP and set status to ALLOFF */
  state_PTR syscallNew = (state_PTR) SYSCALLBREAKNEW;
  syscallNew->s_pc = (memaddr) sysCallHandler; 
  syscallNew->s_t9 = (memaddr) sysCallHandler;
  syscallNew->s_sp = ramBaseAddress->rambase + ramBaseAddress->ramsize;
  syscallNew->s_status = ALLOFF;

  /* set up and create new programTrap area in memory */
  /* set pc and t9 */
  /* set sp to RAMTOP and set status to ALLOFF */
  state_PTR programTrapNew = (state_PTR) PROGRAMTRAPNEW;
  programTrapNew->s_pc = (memaddr) sysCallHandler;
  programTrapNew->s_t9 = (memaddr) sysCallHandler;
  programTrapNew->s_sp = ramBaseAddress->rambase + ramBaseAddress->ramsize;
  programTrapNew->s_status = ALLOFF;

  /* set up and create new tlbManagement area in memory */
  /* set pc and t9 */
  /* set sp to RAMTOP and set status to ALLOFF */
  state_PTR tlbManagementNew = (state_PTR) TLBMANAGEMENTNEW;
  tlbManagementNew->s_pc = (memaddr) sysCallHandler;
  tlbManagementNew->s_t9 = (memaddr) sysCallHandler;
  tlbManagementNew->s_sp = ramBaseAddress->rambase + ramBaseAddress->ramsize;
  tlbManagementNew->s_status = ALLOFF;

  /* set up and create new interrupt area in memory */
  /* set pc and t9 */
  /* set sp to RAMTOP and set status to ALLOFF */
  state_PTR interruptNew = (state_PTR) INTERRUPTNEW;
  interruptNew->s_pc = (memaddr) sysCallHandler;
  interruptNew->s_t9 = (memaddr) sysCallHandler;
  interruptNew->s_sp = ramBaseAddress->rambase + ramBaseAddress->ramsize;
  interruptNew->s_status = ALLOFF;  

  /*init asl and pcb*/
  initASL();
  initPcbs();

  /*global variables*/
  processCount = 0;
  softBlockCount = 0;
  curentProcess = NULL;
  readyQ = NULL;

  /* need to initalize and set each device to 0 */
  int i;
  for(i = 0; i <MAGICNUM; i++){
    semd[i] = 0;
  }

  /*get a pcb*/
  p = allocPcb();
  /*incrememnt process, since we just created it*/
  processCount++;
  /* set the values that we need for our currentProcess to be initialized */
  currentProcess->p_state.s_sp = (ramBaseAddress->rambase - PAGESIZE);
  currentProcess->p_state.s_pc = (memaddr) test; /*from p2test */
  currentProcess->p_state.s_t9 = (memaddr) test;
  currentProcess ->p_state.s_status = ALLOFF | IECON | TEON | IMASKON;
  /*insert into the readyQueue */
  insertProcQ(&readyQ, p);
  currentProcess = NULL;
  /* start hte interval timer */
  LDIT(INTERVALTIMER);
  /*start up scheduler*/
  scheduler();

  return -1;
}
