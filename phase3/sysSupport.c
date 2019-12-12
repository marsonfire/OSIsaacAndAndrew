/* ============= sysSupport.c ==============
 * sysSupport handles user I/O interrupts that request
 * resources given through virtual memory. All requests
 * are sent through the sysCall handler(calls 9 - 18).
 * Currently, only syscalls 10 and 17 active for basic 
 * VM operations. All active calls modify and return 
 * the OS to the last state.
 */

#include "../h/const.h"
#include "../h/types.h"
#include "../e/initProc.e"
#include "/usr/local/include/umps2/umps/libumps.e"


HIDDEN void sysCall9();
HIDDEN void sysCall10();
HIDDEN void sysCall11();
HIDDEN void sysCall12();
HIDDEN void sysCall13();
HIDDEN void sysCall14();
HIDDEN void sysCall15();
HIDDEN void sysCall16();
HIDDEN void sysCall17(state_PTR requester);
HIDDEN void sysCall18();


/* ========== userSyscallHandler() ========== 
 * All syscalls are processed through the handler, 
 * which redirect to sysCalls 9 through 18. Takes 
 * no arguments, but stores and returns the OS's 
 * state.
 */
void userSyscallHandler(){
  int request;
  state_PTR oldState;

  /*Stores off the old state for later use, incrememnts it*/
  oldState = (state_PTR)SYSCALLBREAKOLD;
  oldState -> s_pc + 4;
  request = oldState -> s_a0;
  switch(request){
    case READTERM:
      sysCall9();
      break;

    case WRITETERM:
      sysCall10();
      break;
   
    case VVIRTSEMA4:
      sysCall11();
      break;

    case PVIRTSEMA4:
      sysCall12();
      break;

    case DELAY:
      sysCall13();
      break;

    case DISKPUT:
      sysCall14();
      break;

    case DISKGET:
      sysCall15();
      break;

    case WRITEPRINTER:
      sysCall16();
      break;

    case GETTOD:
      sysCall17(oldState);
      break;
    
    case TERMINATE:
      sysCall18();
      break;
  }
}

HIDDEN void sysCall9(){
  /* To be continued */
}

/* ============= sysCall10() =============
 * Write to Terminal. Suspends other operations
 * and outputs a string to the designated terminal.
 * Currently, will only work with terminal 0.
 */
HIDDEN void sysCall10(char *msg) {
  char* s = msg;
  unsigned int * base = (unsigned int*) TERM0ADDR;
  unsigned int status;

  
  SYSCALL(PASSERN, 1, 0, 0);
  while (*s != EOS) {
    *(base + 3) = 2 | (((unsigned int)  s) << BYTE);
    status = SYSCALL(WAITIO, TERMINT, 0, 0);
    if ((status & 0xFF) != RECVD)
      PANIC();
    s++;
  }
  
  SYSCALL(VERHOGEN, 1, 0, 0);
}

HIDDEN void sysCall11(){
  /* To be continued */
}

HIDDEN void sysCall12(){
  /* To be continued */
}

HIDDEN void sysCall13(){
  /* To be continued */
}

HIDDEN void sysCall14(){
  /* To be continued */
}

HIDDEN void sysCall15(){
  /* To be continued */
}

HIDDEN void sysCall16(){
  /* To be continued */
}

/* ============= sysCall17(state) =============
 * Gets the time of day. Gets the amount of microseconds
 * from the last sustem reset, and places it in the v0 
 * register. Takes in the 'current' state from the handler
 * and returns that state with ToD.
 */
HIDDEN void sysCall17(state_PTR requester){

  /* allocates cpu with current time*/
  cpu_t TOD;                
  STCK(TOD);

  /*stores time off in v0*/
  requester -> s_v0 = TOD;

  /*back to the future*/
  LDST(requester);
}

HIDDEN void sysCall18(){
  /* To be continued */
}
