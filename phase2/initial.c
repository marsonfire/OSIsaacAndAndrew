/* ========== initial.c ==========
 * Serves as the starting point of the OS, with the initial call
 * of main(). This .c file itself serves as an initializer for 
 * many of the global variables used in this OS.
 */
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

/* ===== Global Variables ===== */
int processCount;         /* number of processes in the system */
int softBlockCount;       /* number of processes blocked and waiting for an interrupt */    
pcb_PTR currentProcess;   /* self explanatory... I hope... */
pcb_PTR readyQ;           /* tail pointer to queue of procblks representing processes ready and waiting for execution */
int semd[MAGICNUM];       /* how we get the devices, MAGICNUM was mentioned in class and is in consts.h as 49 */
/* ===== End Global Variables ===== */

/* ===== Start Main() =====*/
/* ==Function: Entry point of the OS. Populates the ROM reserves, initializes
 * the ASL, PCBs, semaphores, and a single process. Once startup is
 * complete, the scheduler is called
 * ==Arguments: None
 */
int main(){
  devregarea_t* ramBaseAddress; 
  int i;
  state_PTR syscallNew;
  state_PTR programTrapNew;
  state_PTR tlbManagementNew;
  state_PTR interruptNew;
  unsigned int ramtop;
  pcb_PTR p;

  /* init and set up a bunch of our vars */
  /* He calls this RAMTOP in the video but it's the same address (0x1000000)*/
  ramBaseAddress = (devregarea_t *) RAMBASEADDR;
  /* set up ramtop */
  ramtop = (ramBaseAddress->rambase) + (ramBaseAddress->ramsize);

  /* set up and create new syscall area in memory */
  /* set pc and t9 */
  /* set sp to RAMTOP and set status to ALLOFF */
  syscallNew = (state_PTR) SYSCALLBREAKNEW;
  syscallNew->s_pc = (memaddr) sysCallHandler; 
  syscallNew->s_t9 = (memaddr) sysCallHandler;
  syscallNew->s_sp = ramtop;
  syscallNew->s_status = ALLOFF;

    
  /* set up and create new programTrap area in memory */
  /* set pc and t9 */
  /* set sp to RAMTOP and set status to ALLOFF */
  programTrapNew = (state_PTR) PROGRAMTRAPNEW;
  programTrapNew->s_pc = (memaddr) pgmTrapHandler;
  programTrapNew->s_t9 = (memaddr) pgmTrapHandler;
  programTrapNew->s_sp = ramtop;
  programTrapNew->s_status = ALLOFF;

  /* set up and create new tlbManagement area in memory */
  /* set pc and t9 */
  /* set sp to RAMTOP and set status to ALLOFF */
  tlbManagementNew = (state_PTR) TLBMANAGEMENTNEW;
  tlbManagementNew->s_pc = (memaddr) tlbManagementHandler;
  tlbManagementNew->s_t9 = (memaddr) tlbManagementHandler;
  tlbManagementNew->s_sp = ramtop;
  tlbManagementNew->s_status = ALLOFF;
   
  /* set up and create new interrupt area in memory */
  /* set pc and t9 */
  /* set sp to RAMTOP and set status to ALLOFF */
  interruptNew = (state_PTR) INTERRUPTNEW;
  interruptNew->s_pc = (memaddr) interruptHandler;
  interruptNew->s_t9 = (memaddr) interruptHandler;
  interruptNew->s_sp = ramtop;
  interruptNew->s_status = ALLOFF;  

  /*init asl and pcb*/
  initASL();
  initPcbs();

  /*global variables*/
  processCount = 0;
  softBlockCount = 0;
  currentProcess = NULL;
  readyQ = mkEmptyProcQ();

  /* need to initalize and set each device to 0 */
  for(i = 0; i < MAGICNUM; i++){
    semd[i] = 0;
  }
   
  /*get a pcb*/
  p = allocPcb();
  /*incrememnt process, since we just created it*/
  processCount++;
  /* set the values that we need for our currentProcess to be initialized */
  p->p_state.s_sp = (ramtop - PAGESIZE);
  /* set the state to test from p2test because that's where we'll start */
  p->p_state.s_pc = (memaddr) test;
  p->p_state.s_t9 = (memaddr) test;
  p->p_state.s_status = ALLOFF | IECON | TEON | IMASKON;
  /*insert into the readyQueue */
  insertProcQ(&readyQ, p);
  /* make currentProcess null again and then start the interval timer */
  currentProcess = NULL;
  LDIT(INTERVALTIMER);
  /*start up scheduler*/
  scheduler();

  return -1;
}
/* ===== End Main() ===== */
