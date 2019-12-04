#include "../h/const.h"
#include "../h/types.h"
#include "../e/initProc.e"
#include "/usr/local/include/umps2/umps/libumps.e"

void userSyscallHandler(){
  
}

HIDDEN void syscall10() {
  
  char* message = 
  devregtr * base = (devregtr *) (TERM0ADDR);
  devregtr status;

  SYSCALL(PASSERN, (int)&term_mut, 0, 0);/* P(term_mut) */
  while (*s != EOS) {
    *(base + 3) = PRINTCHR | (((devregtr) *s) << BYTELEN);
    status = SYSCALL(WAITIO, TERMINT, 0, 0);
    if ((status & TERMSTATMASK) != RECVD)
      PANIC();
    s++;
  }
  SYSCALL(VERHOGEN, (int)&term_mut, 0, 0);/* V(term_mut) */
}
