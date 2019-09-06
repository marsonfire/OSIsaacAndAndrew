
#ifndef lint
static char vcid[] = "nil";
#endif /* lint */
/*asl.c stuff goes down below */
#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"

HIDDEN semd_t *semdFree_h;    /*Free semaphore list*/
HIDDEN semd_t *semdActive_h;  /*Active semaphore list*/

HIDDEN semd_t * allocSemd();
HIDDEN void free(semd_t *s);
HIDDEN semd_t * search(int * semAdd);

void debug(int a){
  int i;
  i = 0;
}


/*Insert a specified pcb at a semd with a certain address*/
int insertBlocked (int *semAdd, pcb_PTR p){
  semd_t * found  = search(semAdd);/*get semAdd semaphore, put in found, parent of what we actually want*/
  semd_t * newSemd = allocSemd();
  if(found->s_next->s_semAdd == semAdd){
    debug(70);
    insertProcQ(&(found->s_next->s_procQ),p);
    return 0;
  }
  else if(newSemd == NULL){
    debug(80);
      return 1;
  }
  else{
    debug(90);
    /* if here, we are going to use the allocated newSemd and add it to the semdActive_h and 
       then going to put p on it */
    newSemd->s_next = found->s_next;
    found->s_next = newSemd;
    newSemd->s_procQ = NULL;
    insertProcQ(&(newSemd->s_next->s_procQ),p);/**/
    newSemd->s_semAdd = semAdd;
    return 0;
  }
}

pcb_PTR removeBlocked (int *semAdd){
  semd_t * found = search(semAdd);
  if(found == NULL){
    return NULL;
  }
  else if(found->s_next->s_procQ == NULL){
    semd_t * temp = found->s_next->s_next;
    found->s_next = temp;
    return NULL;
  }
  else if(found->s_next->s_semAdd == semAdd){
    semd_t * temp = found->s_next->s_procQ;
    pcb_PTR p = found->s_next->s_procQ;
    found->s_next->s_procQ = NULL;
    found->s_next = temp;
    return p;
  }  
  else{
    semd_t * temp = found->s_next->s_next;
    pcb_PTR p = found->s_next->s_procQ;
    found->s_next->s_procQ = NULL;
    found->s_next->s_next =NULL;
    found->s_next = temp;
    return p;
  }
}

pcb_PTR outBlocked (pcb_PTR p){

}

pcb_PTR headBlocked (int *semAdd){

}

void initASL (){
  static semd_t semdTable[MAXPROC+2];/*unsure if needed*/
  /*Initialize both lists as NULL*/
  semdFree_h = NULL;
  semdActive_h = NULL;

  /*fill free list*/
  int i;
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

/* get the head of the free list, and return it so it can be used */
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

/* want to take s off the active list and put it on the free list */
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

/*Searches for slot where selected node would go, returns parent node*/
HIDDEN semd_t * search(int * semAdd){
  semd_t * temp = semdActive_h;

  if(semAdd == NULL){
    semAdd = (int *)MAXINT;
  }
  while(semAdd > (temp->s_next->s_semAdd)){
    /*go to next node in active*/
    debug(3);
    temp = temp->s_next;
    if((temp->s_next) == NULL){
	break;
    }
  }
  return temp;
}
