/*asl.c stuff goes down below */
#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"

HIDDEN semd_t *semdFree_h;    /*Free semaphore list*/
HIDDEN semd_t *semdActive_h;  /*Active semaphore list*/

HIDDEN semd_t * allocSemd();
HIDDEN void free(semd_t *s);
HIDDEN semd_t * search(int * semAdd);

/*
*Function: Insert a specified pcb at a semd with a certain address. If the new semd is null,
*return false.
*/
int insertBlocked (int *semAdd, pcb_PTR p){
  semd_t * found  = search(semAdd);           /*get semAdd semaphore, put in found, parent of what we actually want*/
  if(found->s_next->s_semAdd == semAdd){
    /* if here, we have found the place to insert the pcb and will do that */
    p->p_semAdd = semAdd;
    insertProcQ(&(found->s_next->s_procQ),p);
    return 0;
  }
  else{
    semd_t * newSemd = allocSemd();
    if(newSemd == NULL){
      /* if here, we're blocked and can't insert */
      return 1;
    }
    /* if here, we are going to use the allocated newSemd and add it to the semdActive_h and 
       then going to put p on it */
    newSemd->s_next = found->s_next;
    found->s_next = newSemd;
    newSemd->s_procQ = NULL;
    insertProcQ(&(newSemd->s_procQ),p);/**/
    newSemd->s_semAdd = semAdd;
    p->p_semAdd = semAdd;
    return 0;
  }
}

/*
 *Function: Search for a semaphor with semAdd. If none is found, return NULL.
 *If one is found, remove the head of the pcb queue related to it and
 *return a pointer to the removed pcb. If the semaphor becomes empty, 
 *remove it.
 */
pcb_PTR removeBlocked (int *semAdd){
  semd_t *found = search(semAdd);
  /* we found the semaphore in the asl with the ID we want to change */
  if(found->s_next->s_semAdd == semAdd){
    /* get the one we want to remove */
    pcb_PTR returnedPCB = removeProcQ(&(found->s_next->s_procQ));
    /* if the pcb is null, we'll need to fix some pointers and free up the semaphore */
    if(found->s_next->s_procQ == NULL){
      semd_t * temp = found->s_next;
      found->s_next = found->s_next->s_next;
      /* if here, the asl node doesn't have a procq on it, so lets free it up*/
      free(temp);
    }
    /* then, just set the address and return the one we wanted to remove */
    returnedPCB->p_semAdd = NULL;
    return returnedPCB;
  }
  return NULL;
}

/*
 *Functon: Remove the pcb specified in the parameters, determined by the semAdd
 *of the pcb. If p doesn't exist, return NULL. Otherwise, return p.
 */
pcb_PTR outBlocked (pcb_PTR p){
  semd_t *found = search(p->p_semAdd);
  /* we found the semaphore in the asl with the ID we want to change */
  if(found->s_next->s_semAdd == p->p_semAdd){
    /* pull out the pcb that we want */
    pcb_PTR returnedPCB = outProcQ(&(found->s_next->s_procQ), p);
    if(found->s_next->s_procQ == NULL){
      semd_t * temp = found->s_next;
      found->s_next = found->s_next->s_next;
      /* if here, the asl node doesn't have a procq on it, so lets free it up*/
      free(temp);
    }
    returnedPCB->p_semAdd = NULL;
    return returnedPCB;
  }
  return NULL;
}

/*
 *Function: Return a pointer to the head of the specified queue. If semAdd is 
 *not found, return NULL. 
 */
pcb_PTR headBlocked (int *semAdd){
  semd_t * found = search(semAdd);
  /* we found it! so return it */
  if(found->s_next->s_semAdd == semAdd){
    return headProcQ(found->s_next->s_procQ);
  }
  else{
    return NULL;
  }
}

/*
* Function: Initiate the ASL. Allocate 20 sempahores, with 2 dummy nodes, 1 at the beginning and 1 at the end.
*/
void initASL (){
  int i;
  static semd_t semdTable[MAXPROC+2];/*unsure if needed*/
  /*Initialize both lists as NULL*/
  semdFree_h = NULL;
  semdActive_h = NULL;

  /*fill free list*/
  for(i=0;i<MAXPROC;i++){
    free(&(semdTable[i]));
  }

  /*create last dummy*/
  semdActive_h = &semdTable[MAXPROC+1];
  semdActive_h->s_next = NULL;
  semdActive_h -> s_semAdd = (int *)MAXINT;
  semdActive_h->s_procQ = NULL;

  /*create first dummy*/
  (semdTable[MAXPROC]).s_next = semdActive_h;
  semdActive_h = &(semdTable[MAXPROC]);
  semdActive_h -> s_semAdd = 0;
  semdActive_h->s_procQ = NULL;
  
}

/*
* Function: Get the head of the free list and return it so that it can be used as a semaphore in the active list.
*/
HIDDEN semd_t * allocSemd(){
  semd_t * newSemd = semdFree_h;
  /* if the whole free list is null, return null */
  if(semdFree_h == NULL){
    return NULL;
  }
  else{
    semdFree_h = semdFree_h->s_next;
    /* if here, we're going to reset the node and return it to be put on the free list */
    newSemd->s_next = NULL;
    newSemd->s_semAdd = NULL;
    newSemd->s_procQ = NULL;
    return newSemd;
  }
}

/*
* Function: Given a semaphore, take it off the active list and put it on the free list to be used later.
*/
HIDDEN void free(semd_t * s){
  if(semdFree_h == NULL){
    /* if the free list is empty/null, then make  our s the free list */
    /*s->s_next = NULL;*/
    semdFree_h = s;
    semdFree_h->s_next=NULL;
  }
  /* otherwise, we want to just add s  to the free list (top of the stack) */
  else{
    s->s_next = semdFree_h;
    semdFree_h = s;
  }
}

/*
* Function: Given a semAdd, loop through the sempahore list and find the parent of the semaphore with semAdd
*/
HIDDEN semd_t * search(int * semAdd){
  semd_t * temp = semdActive_h;

  if(semAdd == NULL){
    semAdd = (int *)MAXINT;
  }
  while(semAdd > (temp->s_next->s_semAdd)){
    /*go to next node in active*/
    temp = temp->s_next;
    if((temp->s_next) == NULL){
      break;
    }
  }
  return temp;
}
