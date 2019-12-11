#include "../h/const.h"
#include "../h/types.h"
#include "../e/initProc.e"
#include "/usr/local/include/umps2/umps/libumps.e"


HIDDEN void sysCall9(state_PTR requester);
HIDDEN void sysCall10();
HIDDEN void sysCall11();
HIDDEN void sysCall12();
HIDDEN void sysCall13();
HIDDEN void sysCall14();
HIDDEN void sysCall15();
HIDDEN void sysCall16();
HIDDEN void sysCall17(state_PTR requester);
HIDDEN void sysCall18();


void userSyscallHandler(){
  /*codehere*/
  int request;
  state_PTR oldState;

  /*Stores off the old state for later use, incrememnts it*/
  oldState = (state_PTR)SYSCALLBREAKOLD;
  oldState -> s_pc + 4;
  request = oldState -> s_a0;
  
  switch(request){
    case READTERM:
      sysCall9(oldState);
      break;
    
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

<<<<<<< HEAD
HIDEEN void sysCall9(state_PTR requester){
=======
HIDDEN void sysCall9(){
>>>>>>> 9f10dda213ed574e936ebf1cae78a3ceac2d2a08

}

HIDDEN void sysCall10() {
  
}

HIDDEN void sysCall11(){

}

HIDDEN void sysCall12(){

}

HIDDEN void sysCall13(){

}

HIDDEN void sysCall14(){

}

HIDDEN void sysCall15(){

}

HIDDEN void sysCall16(){

}

HIDDEN void sysCall17(state_PTR requester){

  /* allocates cpu*/
  cpu_t TOD;                
  STCK(TOD);

  /*stores time off in v0*/
  requester -> s_v0 = TOD;

  /*back to the future*/
  LDST(requester);
}

HIDDEN void sysCall18(){

}
