/*asl.c stuff goes down below */
#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"

HIDDEN semd_t *semdFree_h;    /*Free semaphore list*/
HIDDEN semd_t *semdActive_h;  /*Active semaphore list*/

HIDDEN semd_t * allocSemd();
HIDDEN void free(semd_t *s);
HIDDEN semd_t * search(int * semAdd);

int insertBlocked (int *semAdd, pcb_PTR p){
  
}

pcb_PTR removeBlocked (int *semAdd){

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
  for(i=0;i<MAXPROC+2;i++){
    free(&semdTable[i]);
  }
  /*create first dummy*/
  semdActive_h = &semdTable[0];
  semdActive_h -> s_semAdd = 0;

  /*create last dummy*/
  semdActive_h = &semdTable[MAXPROC+1];
  semdActive_h -> s_semAdd = MAXINT;
  
}

/* similar to allocPcbs from pcb.c */
HIDDEN semd_t * allocSemd(){
  semd_t * newSemd = semdFree_h;
  /* if the whole free list is null, return null */
  if(semdFree_h == NULL){
    return NULL;
  }
  else{
    /* if here, we're going to reset the node and return it to be put on the free list */
    newSemd->s_next = NULL;
    newSemd->s_semAdd = NULL;
    newSemd->s_procQ = NULL;
    return newSemd;
  }
}
