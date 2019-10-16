/* interrupts.c */
#include "../h/const.h"
#include "../h/types.h"
#include "../e/asl.e"
#include "../e/pcb.e"
#include "../e/initial.e"
#include "../e/scheduler.e"
#include "../e/exceptions.e"
#include "../e/interrupts.e"
#include "/usr/local/include/umps2/umps/libumps.e"

/*global vars */
extern int processCount;         /* number of processes in the system */
extern int softBlockCount;       /* number of processes blocked and waiting for an interrupt */
extern pcb_PTR currentProcess;   /* self explanatory... I hope... */
extern pcb_PTR readyQ;           /* tail pointer to queue of procblks representing processes ready and waiting for execution */
extern int semd[MAGICNUM]        /* our 49 devices */
extern cpu_t startTOD;           /* time the process started at */
extern cpu_t stopTOD;            /* time the process stopped at */

extern void copyState(state_PTR original, state_PTR dest);

void interruptHandler(){
  state_PTR oldInterruptArea = (state_PTR)INTERRUPTOLD;
  cpu_t startTime, endTime;
  device_t * device;
  int deviceNum, lineNum = 0;
  int* sem;
  /* figure out which interrupt line the interrupt is on */ 
  unsigned int cause = (oldInterruptArea->s_cause & IMASKON) >> 8;
  /* start clock */
  STCK(startTime);
  /* this shouldn't ever happen */
  if((cause & FIRST) != 0){
    /* line 0 - multi core - won't happen because kaya doesn't support it */ 
    PANIC();
  }
  /* if this runs, a process' clock has run out and we will exit the interrupHandler and go back to the scheduler */
  else if((cause & SECOND) != 0){
    /* line 1 - processor local timer */
    done(startTime);
  }
  else if((cause & THIRD) != 0){
    /* line 2 - interval timer */
    /* start by loading interval timer with 100 miliseconds */
    LDIT(INTERVALTIMER);
    /* get interval timer from the semaphore list */
    sem = (int*)  &(semd[MAGICNUM - 1]);
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
    lineNum = NEWINT;
  }
  
  /*sets up for the printer device*/
  else if((cause & SEVENTH)!= 0){
    lineNum = PRNTINT;
  }
  
  /*sets up for the terminal*/
  else if((cause & EIGHTH)!= 0){
    lineNum = TERMINT;
  }

  /* lineNum has the lineNum set from above. Must subtract 3 because we have
 the first 3 devices without 8 semaphores and then multiply by the WordLen (4)
 However, we still need to get to the address of the registers' so we add 
 Interrupting Devices Bitmap address to it so we're in the right memory area.*/
  unsigned int* findDeviceLineNum = ((lineNum - 3) * WORDLEN) + INTDEVBITMAP;
  /* get the device number */
  deviceNum = getDevice(findDeviceLinNum);
  /* get the device register now */
  /* get the lineNum - 3 for the first 3 devices without semaphores
   Then, * 8 for the each having 8 devices. 
   Then, * DEVREGSIZE (16) so we can get to the Device register address */
  unsigned int *  lineNumOffset = (lineNum - 3) * 8 * DEVREGSIZE;
  /* need to get the offset of the device number we have 
     so use the devNum we got above * DEVREGSIZE (16) */
  unsigned int * devNumOffset = devNum * DEVREGSIZE;
  /* add our 2 values to INTDEVREG (address of starting interrupt device 
   register 3) and our offsets should give us the exact address of the 
   intterupt line #, device # device register we want */
  device = (device_t *) (INTDEVREG + lineNumOffset + devNumOffset);
  /* need to be able to store the status of the device and put it into the
     process' state later */
  int deviceStatus;
  int semIndex = 0;
  /* if it's not 7, then we are not working witha terminal, meaning we 
     dont have to worry about the transmit and recv stuff at all */
  if(lineNum != 7){
    /* lineNum previously set - 3 for first 3 without semaphores
then, multiply by 8 for each with 8 devices, plus the device number we got
earlier so that we can get the index of the semaphore in our semd array */
    semIndex = ((lineNum - 3) * 8) + deviceNum;
    /* acknowledge the device with a 1 */
    device->d_command = ACK;
    /* save our status off */
    deviceStatus = device->d_status;
  }
  /* now we have to do a terminal */
  else{
    /* check if the transmission status is ready, if not, we'll want to write
to terminal -  bottom page 46 of princ of ops has codes */
    if((device->t_transm_status & 0xFF) != 1){
      /* get the semaphore index for later */
      semIndex = ((lineNum - 3) * 8) + deviceNum;
      /* acknowldge terminal device with a 1 -> slightly different... see it 
	 set up in const.h or page 47 of princ of ops */
      device->t_transm_command = ACK;
      /* store our status */
      deviceStatus = device->t_transm_status;
    }
    /* read from the terminal -> transmission recv is ready */
    else {
      /* -2 now because we are using the other terminal 'register' of sorts */
      semIndex = ((linNum - 2) * 8) + deviceNum;
      /* acknowledge terminal device that it's a receive command */
      device->t_recv_command = ACK;
      /* store our recv status */
      device = device->t_recv_status;
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

/* helper functions */

void done(cpu_t startTime){
  cpu_t endTime;
  state_PTR oldInterruptArea = (state_PTR)INTERRUPTOLD;
  if(currentProcess != NULL){
    /* start the clockand figure out the start time of the process */
    STCK(endTime);
    startTOD = startTOD + (endTime - startTime);
    /* copy over the old interrupt area state into current process' state */
    copyState(oldInterruptArea, &(currentProcess->p_state));
    /* put this into the ready queue */
    insertProcQ(&(readyQ), currentProcess);
  }
  scheduler();
}

/*identifies the device that an interrupt is happening at*/
/*bitMap may need to be changed to unsigned*/
int getDevice(unsigned int * bitMap){
  if(bitMap == FIRST){
    return 0;
  }
  else if(bitMap == SECOND){
    return 1;
  }
  else if(bitMap == THIRD){
    return 2;
  }
  else if(bitMap == FOURTH){
    return 3;
  }
  else if(bitMap == FIFTH){
    return 4;
  }
  else if(bitMap == SIXTH){
    return 5;
  }
  else if(bitMap == SEVENTH){
    return 6;
  }
  else if(bitMap == EIGHTH){
    return 7;
  }
}
