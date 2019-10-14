/* exceptions.e */

#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"
#include "../e/initial.e"
#include "/usr/local/include/umps2/umps/libumps.e"

#ifndef exceptions
#define exceptions

extern void sysCallHandler();
extern void tlbManager();
extern void pgmTrap();
extern void pgmTrapHandler();
extern void tlbManagementHandler();

#endif
