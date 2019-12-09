#ifndef initproc
#define initproc

extern pgTableOS_t ksegOS;
extern pgTable_t kuseg2;
extern pgTable_t kuseg3;
extern framePool_t framePool[FRAMESIZE];
extern int swapSem, masterSem;
extern userProcData_t userProcs[MAXUPROC];
extern int semTable[SEMNUM];

#endif initproc