/* ========== Interrupts.c ==========
 * Interrupts serves as the handler for interruptions of every type, 
 * and converts said interrupts into V operations on the selected 
 * device. 
 */
#include "../h/const.h"
#include "../h/types.h"
#include "../e/asl.e"
#include "../e/pcb.e"
#include "../e/initial.e"
#include "../e/scheduler.e"
#include "../e/exceptions.e"
#include "../e/interrupts.e"
#include "/usr/local/include/umps2/umps/libumps.e"

/* ===== Start Initial Global Variables ===== */
extern int processCount;         /* number of processes in the system */
extern int softBlockCount;       /* number of processes blocked and waiting for an interrupt */
extern pcb_PTR currentProcess;   /* self explanatory... I hope... */
extern pcb_PTR readyQ;           /* tail pointer to queue of procblks representing processes ready and waiting for execution */
extern int semd[SEMNUM];       /* our 49 devices */
/* ===== End Initial Global Variables ===== */
/* ===== Start Scheduler Global Variables ===== */
extern cpu_t startTOD;           /* time the process started at */
extern cpu_t stopTOD;            /* time the process stopped at */
/* ===== End Scheduler Global Variables ===== */

/* function from exceptions.c */
extern void copyState(state_PTR original, state_PTR dest);

/* local module helper function declaration */
HIDDEN void done(cpu_t startTime);
HIDDEN int getDevice(unsigned int* bitMap);


/* ===== Start interruptHandler() ===== */
/*
 *==Function: Called as a setup in initial.c. It reserves areas
 * in memory for future interrupts and then passes along to scheduler()
 *==Arguments: None.
 */
void interruptHandler(){
  state_PTR oldInterruptArea;
  cpu_t startTime, endTime;
  device_t* device;
  int deviceNum, lineNum;
  int* sem;
  /* need to be able to store the status of the device and put it into the
     process' state later */
  int deviceStatus;
  int semIndex;
  /* figure out which interrupt line the interrupt is on */ 
  unsigned int cause;
  oldInterruptArea = (state_PTR)INTERRUPTOLD;
  cause = ((oldInterruptArea->s_cause) & IMASKON) >> 8;
  /* start clock */
  STCK(startTime);
  /* this shouldn't ever happen */
  if((cause & FIRST) != 0){
    /* line 0 - multi core - won't happen because kaya doesn't support it */ 
    PANIC();
  }
  /* if this runs, a process' clock has run out and we will exit the interrupHandler and go back to the scheduler */
  else if((cause & SEC) != 0){
    /* line 1 - processor local timer */
    done(startTime);
  }
  else if((cause & THIRD) != 0){
    /* line 2 - interval timer */
    /* start by loading interval timer with 100 miliseconds */
    LDIT(INTERVALTIMER);
    /* get interval timer from the semaphore list */
    sem = (int*) &(semd[SEMNUM - 1]);
    while(headBlocked(sem) != NULL){
      /* get blocked process */
      pcb_PTR blocked = removeBlocked(sem);
      /* get tht time put in the endTime so we can calculate how long interrupt took */
      STCK(endTime);
      if(blocked != NULL){
      	insertProcQ(&(readyQ), blocked);
      	/* set the time the process took */
      	blocked->p_time = blocked->p_time + (endTime - startTime);
      	/* it won't be blocked anymore */
      	softBlockCount--;
      }
    }
    /* reset it since we're done with being blocked */
    (*sem) = 0;
    done(startTime);
  }
  
  /*sets up for the disk device*/
  else if((cause & FOURTH)!= 0){
    lineNum = DISKINT;
  }
  
  /*setrs up for the tape device, a sticky issue*/
  else if((cause & FIFTH)!= 0){
    lineNum = TAPEINT;
  }
  
  /*sets up for the network device*/
  else if((cause & SIXTH)!= 0){
    lineNum = NETWINT;
  }
  
  /*sets up for the printer device*/
  else if((cause & SEVENTH)!= 0){
    lineNum = PRNTINT;
  }
  
  /*sets up for the terminal*/
  else if((cause & EIGHT)!= 0){
    lineNum = TERMINT;
  }
  /* lineNum has the lineNum set from above. Must subtract 3 because we have
 the first 3 devices without 8 semaphores and then multiply by the WordLen (4)
 However, we still need to get to the address of the registers' so we add 
 Interrupting Devices Bitmap address to it so we're in the right memory area.*/
  deviceNum = getDevice((unsigned int*) (INTDEVBITMAP + ((lineNum - NOSEMS) * WORDLEN)));
  /* get the device register now */
  /* get the lineNum - 3 for the first 3 devices without semaphores
   Then, * 8 for the each having 8 devices. 
   Then, * DEVREGSIZE (16) so we can get to the Device register address */
  /* need to get the offset of the device number we have 
     so use the devNum we got above * DEVREGSIZE (16) */
  /* add our 2 values to INTDEVREG (address of starting interrupt device 
   register 3) and our offsets should give us the exact address of the 
   intterupt line #, device # device register we want */
  device = (device_t*) (INTDEVREG + ((lineNum - NOSEMS) * DEVREGSIZE * EIGHTPERDEV) + (deviceNum * DEVREGSIZE));
  /* if it's not 7, then we are not working witha terminal, meaning we 
     dont have to worry about the transmit and recv stuff at all */
  if(lineNum != 7){
    /* save our status off */
    deviceStatus = device->d_status;
    /* lineNum previously set - 3 for first 3 without semaphores
        then, multiply by 8 for each with 8 devices, plus the device number we got
        earlier so that we can get the index of the semaphore in our semd array */
    semIndex = ((lineNum - NOSEMS) * EIGHTPERDEV) + deviceNum;
    /* acknowledge the device with a 1 */
    device->d_command = ACK;
  }
  /* now we have to do a terminal */
  else{
    /* check if the transmission status is ready, if not, we'll want to write
        to terminal -  bottom page 46 of princ of ops has codes */
    if((device->t_transm_status & 0xFF) != READY){
      /* store our status */
      deviceStatus = device->t_transm_status;
      /* get the semaphore index for later */
      semIndex = ((lineNum - NOSEMS) * EIGHTPERDEV) + deviceNum;
      /* acknowldge terminal device with a 1 -> slightly different... see it 
	     set up in const.h or page 47 of princ of ops */
      device->t_transm_command = ACK;
    }
    /* read from the terminal -> transmission recv is ready */
    else {
      /* store our recv status */
      deviceStatus = device->t_recv_status;
      /* -2 now because we are using the "other" read part of the terminal of sorts */
      semIndex = ((lineNum - 2) * EIGHTPERDEV) + deviceNum;
      /* acknowledge terminal device that it's a receive command */
      device->t_recv_command = ACK;
    }
  }
  /* home stretch */
  /* signal to associated device in our semaphore array */
  sem = &(semd[semIndex]);
  (*sem)++;
  if((*sem) <= 0){
    /* signal to process that it's time to wake up and be a productive 
       member of our society */
    pcb_PTR p = removeBlocked(sem);
    if(p != NULL){
      p->p_state.s_v0 = deviceStatus;
      insertProcQ(&(readyQ), p);
      softBlockCount--;
    }
  }
  done(startTime);
}
/* ===== End interruptHandler() ===== */

/***** ======= Helper Functions ======= *****/

/* ===== Start done() ===== 
 *==Function: Records the time off the cpu, and then checks for a
 * current running process when an interrupt occurs. If there is a
 * process, it does not have any time added to it when the OS works
 * through the interrupt handler.
 *==Arguments: cpu_t startTime, which serves as the starting time for
 * when an interrupt happens.
 *==Returns: None
 */
HIDDEN void done(cpu_t startTime){
  cpu_t endTime;
  state_PTR oldInterruptArea = (state_PTR)INTERRUPTOLD;
  if(currentProcess != NULL){
    /* start the clock and figure out the start time of the process */
    STCK(endTime);
    startTOD = startTOD + (endTime - startTime);
    /* copy over the old interrupt area state into current process' state */
    copyState(oldInterruptArea, &(currentProcess->p_state));
    /* put this into the ready queue */
    insertProcQ(&(readyQ), currentProcess);
  }
  scheduler();
}
/* ===== End done() ===== */

/* ===== Start getDevice() ===== 
 *==Function: Gets the device associated with an interrupt based on
 * the bit position.
 *==Arguments: unsigned int* bitMap, which is associated with a device.
 *==Returns: The relative position of the highest bit in the map. Will 
 * return -1 if no bit is found.
 */
HIDDEN int getDevice(unsigned int* bitMap){
  unsigned int interruptCause = (*bitMap);
  if((interruptCause & FIRST) != 0){
    return 0;
  }
  else if((interruptCause & SEC) != 0){
    return 1;
  }
  else if((interruptCause & THIRD) != 0){
    return 2;
  }
  else if((interruptCause & FOURTH) != 0){
    return 3;
  }
  else if((interruptCause & FIFTH) != 0){
    return 4;
  }
  else if((interruptCause & SIXTH) != 0){
    return 5;
  }
  else if((interruptCause & SEVENTH) != 0){
    return 6;
  }
  else if((interruptCause & EIGHT) != 0){
    return 7;
  }
  else{
    return -1;
  }
}
/* ===== End getDevice() ===== */
