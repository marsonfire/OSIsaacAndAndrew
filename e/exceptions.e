/* exceptions.e */

#ifndef exceptions
#define exceptions


extern void sysCallHandler();
extern void pgmTrapHandler();
extern void tlbManagementHandler();
extern void copyState(state_PTR original, state_PTR dest);

#endif
