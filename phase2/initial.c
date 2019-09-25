#initial.c
#include "../h/types.h"
#include "../h/const.h"
#include "../e/pcb.e"
#include "../e/asl.e"
/*will probably need a lot more for this, as
 *it initializes everything*/

int processCount;         
int softBlockCount;       
pcb_PTR currentProcess;   
pcb_PTR readyQue;         

main(){
  /*Set stack pointers?*/
  /*set the new areas that are in constants */
  /* I think we have to do this for each const I added to consts.h*/
  state_PTR syscallNew = (state_PTR) SYSCALLBREAKNEW;
  /* below has to change, that's just what I'm doing for right now */
  syscallNew->s_pc = (memoryAddress) InterruptHandler;
  syscallNew->s_t9 = (memoryAddress) InterruptHandler;

  
  /*init asl and pcb*/
  initASL();
  initPcbs();

  /*global variables*/
  processCount = 0;
  softBlockCount = 0;
  curentProcess = NULL;
  readyQue = NULL;

  /*get a pcb*/
  p = allocPcb();
  /*set stack pointer to second to last space in memory*/
  /*set PC to p2test*/
  /*utilize more magic, set status*/

  /*incrememnt process, insert queue*/
  processCount++;
  insertProcQ(&readyQue, p);

  /*start up scheduler*/
  scheduler();
}
