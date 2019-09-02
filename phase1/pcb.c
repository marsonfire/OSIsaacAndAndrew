#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"

/*PTR to the head of queue*/
HIDDEN pcb_PTR pcbFree_h;

/*pcb.e is basically an interface to go off of */

void debug(int a){
  int i;
  i = 0;
}

void debug2(int a){
  int i;
  i = 0;
}

void debug3(int a){
  int i;
  i = 0;
}

/*puts pcb p back on the free list */
void freePcb(pcb_PTR p){
  /*insert new p into pcbFree */
  insertProcQ(&pcbFree_h, p);
}

/*allocate space for pcbs? */
pcb_PTR allocPcb (){
  /*creates placeholder, sets to current list element*/
  debug(10);
  pcb_PTR temp = removeProcQ(&pcbFree_h);
  debug(1);
	if(temp != NULL){  
	  /*queue values to null*/
	  debug(2);
	  temp->p_next = NULL;
	  temp->p_prev = NULL;
	
	  /*field values to be set to null here, will double check those soon*/

	  /*once all is set, return new element*/
	}
	debug(3);
	return temp;
	
}

/*create 20 of them to go on the free list*/
void initPcbs(){
  int i;
  static pcb_t procTable[MAXPROC];                /*create a global array with 20 pcb_t */
  pcbFree_h = mkEmptyProcQ();
  /*set our global queue to be nothing/reset it */
  for(i = 0; i < MAXPROC; i++){
    insertProcQ(&pcbFree_h, &procTable[i]);       /*put each pcb_t in our global queue*/
  }
}

/*makes a pcb null/empty */
pcb_PTR mkEmptyProcQ (){
	return NULL;
}

/*checks if a pcb is empty or not*/
int emptyProcQ (pcb_PTR tp){
	return(tp == NULL);
}

void insertProcQ (pcb_PTR *tp, pcb_PTR p){
  /*we have an empty queue, so create a new one*/
	if(emptyProcQ(pcbFree_h)){
	  p->p_next = p;  /*p's next points to itself*/
	  p->p_prev = p;  /*p's previous points to itself*/
	  *tp = p;	  /*the tail pointer is p since it's the only node in the queue*/
	}
	/*if here, we don't have an empty queue*/
	else{
	  pcb_PTR temp = *tp;			/*save the tail pointer in a temp var to be used later*/
	  *tp = p;					/*make the tail pointer point to our new node since it's going to the end*/
	  p->p_next = temp->p_next;	/*make p's next point to the head, which is what temp's (the previous tail) next was*/
	  temp->p_next = p;			/*temp is now 2nd to last and p is last so set it's p_next*/
	  p->p_prev = temp;			/*p's previous is now temp*/
	  p->p_next->p_prev = p;		/*p's next = the head, make the head's previous be p so it's not pointing to the previous tail*/
	}
}

pcb_PTR removeProcQ (pcb_PTR *tp){
  debug(100);
    /*empty queue so return nothing*/
  if(emptyProcQ(pcbFree_h)){
    debug(4);
    return NULL;
  }
  else if((*tp)->p_next == (*tp)){
    pcb_PTR head = (*tp);
    (*tp) = mkEmptyProcQ();
    return head;
  }
  /*otherwise, we'll need to remove the first one and return it*/
  else{
    debug(5);
    pcb_PTR head = (*tp)->p_next;           /*head that we'll remove*/
    (*tp)->p_next->p_next->p_prev  = (*tp);           /*tp's next is now the 2nd in queue (new head)*/
    /*(*tp)->p_next->p_prev->p_prev = NULL;          /*the old head's previous is null, so it doesn't point to the tail anymore*/
    /* (*tp)->p_next->p_prev->p_next = NULL;*/           /*the old head's next is null, so it doesn't point to the new head anymore */
    (*tp)->p_next = (*tp)->p_next->p_next;              /*set the new head's next to the be the tail */
    return head;
  }
}

pcb_PTR outProcQ (pcb_PTR *tp, pcb_PTR p){
  
}

pcb_PTR headProcQ (pcb_PTR tp){
  if(emptyProcQ(pcbFree_h)){
    return NULL;
  }
  else{
    return tp->p_next;
  }
}

int emptyChild(pcb_PTR p){

}

void insertChild(pcb_PTR prnt, pcb_PTR p){

}

pcb_PTR removeChild(pcb_PTR p){

}

pcb_PTR outChild(pcb_PTR p){

}
