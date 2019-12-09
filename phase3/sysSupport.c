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
HIDDEN void sysCall17();
HIDDEN void sysCall18();


void userSyscallHandler(){
  /*codehere*/
  int request;

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
      sysCall17();
      break;

    case TERMINATE:
      sysCall18();
      break;
  }
}

HIDDEN void sysCall9(){

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

HIDDEN void sysCall17(){

}

HIDDEN void sysCall18(){

}
