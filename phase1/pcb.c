#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"

/*PTR to the head of queue*/
HIDDEN pcb_PTR pcbFree_h;

/*pcb.e is basically an interface to go off of */

/*puts pcb p back on the free list */
void freePCB(pcb_PTR p){
  /*insert new p into pcbFree */
  insertProcQ(&pcbFree_h, p);
}

/*allocate space for pcbs? */
pcb_PTR allocPcb (){
  /*creates placeholder, sets to current list element*/
	pcb_PTR temp = pcbFree_h;
	if(&pcbFree_h == NULL){
	  return NULL;    /*Return null if pcbFree list is empty*/
 	}
	/*remove list element here once implemented/figured out */


	*pcbFree_h = *pcbFree_h -> p_next;/*cycles element*/
	/*queue values to null*/
	temp -> p_next = NULL;
	temp -> p_prev = NULL;

	/*field values to be set to null here, will double check those soon*/

	/*once all is set, return new element*/
	return temp;

}

/*create 20 of them to go on the free list*/
void initPcbs(){
  int i;
  static pcb_t procTable[MAXPROC];                /*create a global array with 20 pcb_t */
  pcbFree_h = mkEmptyProcQ();                     /*set our global queue to be nothing/reset it */
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
  pcb_PTR p;
  /*empty queue so return nothing*/
  if(emptyProcQ(pcbFree_h)){
    return NULL;
  }
  /*otherwise, we'll need to remove the first one and return it*/
  else{
    p = tp->p_next;                                   /*set p to be the tail pointers next (head) of the queue that we'll remove*/
    *tp->p_next = *tp->p_next->p_next;                 /*set tp's next to be the 2nd in queue (new head)*/
    *tp->p_next->p_prev->p_next = NULL;                /*set the previous head's next to null*/
    *tp->p_next->p_prev->p_next = NULL;                /*set the previous head's prev to null*/
    *tp->p_next->p_next = *tp;                         /*set the new head's next to be the tail */
    return p;
  }
}

pcb_PTR outProcQ (pcb_PTR *tp, pcb_PTR p){

}

pcb_PTR headProcQ (pcb_PTR tp){

}

