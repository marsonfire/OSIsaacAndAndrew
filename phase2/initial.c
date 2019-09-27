/* initial.c */
#include "../h/types.h"
#include "../h/const.h"
#include "../e/pcb.e"
#include "../e/asl.e"
/*will probably need a lot more for this, as
 *it initializes everything*/

int processCount;         /* number of processes in the system */
int softBlockCount;       /* number of processes blocked and waiting for an interrupt */    
pcb_PTR currentProcess;   /* self explanatory... I hope... */
pcb_PTR readyQ;         /* tail pointer to queue of procblks representing processes ready and waiting for execution */

main(){

  devregarea_t * ramBaseAddress = (devregarea_t) RAMBASEADDR; /* He calls this RAMTOP in the video but it's the same address (0x1000000)*/

  /*Set stack pointers?*/
  /*set the new areas that are in constants */

  /* below, we'll have to write interruptHandler in interrupt.c */

  /* set up and create new syscall area in memory */
  /* set pc and t9 */
  /* set sp to RAMTOP and set status to whatever the hell it's supposed to be */
  state_PTR syscallNew = (state_PTR) SYSCALLBREAKNEW;
  syscallNew->s_pc = (memaddr) interruptHandler; 
  syscallNew->s_t9 = (memaddr) interruptHandler;
  syscallNew->s_sp = ramBaseAddress->rambase + ramBaseAddress->ramsize;
  syscallNew->s_status = ALLOFF;

  /* set up and create new programTrap area in memory */
  /* set pc and t9 */
  /* set sp to RAMTOP and set status to whatever the hell it's supposed to be */
  state_PTR programTrapNew = (state_PTR) PROGRAMTRAPNEW;
  programTrapNew->s_pc = (memaddr) interruptHandler;
  programTrapNew->s_t9 = (memaddr) interruptHandler;
  programTrapNew->s_sp = ramBaseAddress->rambase + ramBaseAddress->ramsize;
  programTrapNew->s_status = ALLOFF;

  /* set up and create new tlbManagement area in memory */
  /* set pc and t9 */
  /* set sp to RAMTOP and set status to whatever the hell it's supposed to be */
  state_PTR tlbManagementNew = (state_PTR) TLBMANAGEMENTNEW;
  tlbManagementNew->s_pc = (memaddr) interruptHandler;
  tlbManagementNew->s_t9 = (memaddr) interruptHandler;
  tlbManagementNew->s_sp = ramBaseAddress->rambase + ramBaseAddress->ramsize;
  tlbManagementNew->s_status = ALLOFF;

  /* set up and create new interrupt area in memory */
  /* set pc and t9 */
  /* set sp to RAMTOP and set status to whatever the hell it's supposed to be */
  state_PTR interruptNew = (state_PTR) INTERRUPTNEW;
  interruptNew->s_pc = (memaddr) interruptHandler;
  interruptNew->s_t9 = (memaddr) interruptHandler;
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

  /*get a pcb*/
  p = allocPcb();
  /*set stack pointer to second to last space in memory*/
  /*set PC to p2test*/
  /*utilize more magic, set status*/

  /*incrememnt process, insert queue*/
  processCount++;
  insertProcQ(&readyQ, p);

  /*start up scheduler*/
  scheduler();

  return -1;
}
