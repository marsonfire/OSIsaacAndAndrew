#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"

/*PTR to the head of queue*/
HIDDEN pcb_PTR pcbFree_h;

/*pcb.e is basically an interface to go off of */

void debugA(int a){
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
  pcb_PTR temp = removeProcQ(&pcbFree_h);
	if(temp != NULL){
	  /*queue values to null*/
	  temp->p_next = NULL;
	  temp->p_prev = NULL;
	  /*tree values set to NULL*/
	  temp->p_child = NULL;
	  temp->p_parent = NULL;
	  temp->p_nextSib = NULL;
	  temp->p_prevSib = NULL;

	  /*once all is set, return new element*/
	}
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
  if(emptyProcQ(*tp)){
	  p->p_next = p;  /*p's next points to itself*/
	  p->p_prev = p;  /*p's previous points to itself*/
	  *tp = p;	  /*the tail pointer is p since it's the only node in the queue*/
	}
	/*if here, we don't have an empty queue*/
	else{;
	  pcb_PTR temp = *tp;			/*save the tail pointer in a temp var to be used later*/
	  *tp = p;					/*make the tail pointer point to our new node since it's going to the end*/
	  p->p_next = temp->p_next;	/*make p's next point to the head, which is what temp's (the previous tail) next was*/
	  temp->p_next = p;			/*temp is now 2nd to last and p is last so set it's p_next*/
	  p->p_prev = temp;			/*p's previous is now temp*/
	  p->p_next->p_prev = p;		/*p's next = the head, make the head's previous be p so it's not pointing to the previous tail*/
	}
}

pcb_PTR removeProcQ (pcb_PTR *tp){
    /*empty queue so return nothing*/
  if(emptyProcQ(*tp)){
    return NULL;
  }
  /*only one element in the queue*/
  else if((*tp)->p_next == (*tp)){
    pcb_PTR head = (*tp);
    (*tp) = mkEmptyProcQ();
    return head;
  }
  /*otherwise, we'll need to remove the first one and return it*/
  else{
    pcb_PTR head = (*tp)->p_next;           /*head that we'll remove*/
    (*tp)->p_next->p_next->p_prev  = (*tp);         /*sets the new head's previous to the tail */
    (*tp)->p_next = (*tp)->p_next->p_next;         /*set the new head */
    head->p_next = NULL;
    return head;
  }
}

pcb_PTR outProcQ (pcb_PTR *tp, pcb_PTR p){
  if(emptyProcQ(*tp)){
    return NULL;
  }
  /*only one element in the queue*/
  else if((*tp)->p_next ==(*tp)){
    pcb_PTR head = (*tp);
    /* the one element in the queue is what we're looking for*/
    if(head == p){
      removeProcQ((*tp));
    }
    /* that one element wasn't the one we're looking for, so return null */ 
    else {
      return NULL;
    }
  }
  /*more than one element in the queue */
  else{
    pcb_PTR pcbLookingAt = (*tp)->p_next; /*the head*/
    /* the head is the element that we're looking for, so just remove it like normal*/
    if(pcbLookingAt == p){
      removeProcQ(tp);
    }
    /* the one we're looking for is the tail */
    else if((*tp) == p){
      pcb_PTR temp = (*tp);
      (*tp)->p_prev->p_next = (*tp)->p_next;       /*the new tail's next is set to the head */
      (*tp)->p_next->p_prev = (*tp)->p_prev;       /*the head's previous is set to tail's previous */
      (*tp) = (*tp)->p_prev;                         /*the new tail is set as the new tail (old tail's previous */
      return temp;
    }
    /* remove something from the middle */
    else{
      pcbLookingAt = (*tp)->p_next->p_next;       /*already checked for the head, so look at the second element */
      while(pcbLookingAt != (*tp)){
	if(pcbLookingAt  == p){
	  pcbLookingAt->p_prev->p_next = pcbLookingAt->p_next;         /*set the previous element's next to the one ahead of the one we're looking at */
	  pcbLookingAt->p_next->p_prev = pcbLookingAt->p_prev;         /*set the next element's prevoius to the one behind the one we're looking at */
	  return pcbLookingAt;
	}
	pcbLookingAt = pcbLookingAt->p_next;
      }
      return NULL;
    }
  }
}

pcb_PTR headProcQ (pcb_PTR tp){
  if(emptyProcQ(tp)){
    return NULL;
  }
  else{
    return tp->p_next;
  }
}

int emptyChild(pcb_PTR p){
  return p->p_child == NULL;
}

void insertChild(pcb_PTR parent, pcb_PTR p){
  /*parent has no kids :( */
  if(emptyChild(parent)){
    parent->p_child = p;
    p->p_parent = parent;
  }
  /*parent has kids (good Catholics) */
  else{
    parent->p_child->p_prevSib = p;
    p->p_nextSib = NULL;
    p->p_parent = parent;
    p->p_nextSib = parent->p_child;
    parent->p_child = p;
  }
}

pcb_PTR removeChild(pcb_PTR parent){
  /*no kids, so don't remove anything, just return null */
  if(emptyChild(parent)){
    return NULL;
  }
  /*only one child*/
  else if(parent->p_child->p_nextSib == NULL){
    pcb_PTR child = parent->p_child;
    parent->p_child->p_parent = NULL;
    parent->p_child = NULL;
    return child;
  }
  /* more than one child */
  else{
    pcb_PTR firstChild = parent->p_child;
    parent->p_child->p_nextSib->p_prevSib = NULL;
    parent->p_child = parent->p_child->p_nextSib;
    return firstChild;
  }
}

pcb_PTR outChild(pcb_PTR p){
  /* orphan child has no parents :( */
  if(p->p_parent == NULL){
    return NULL;
  }
  /*only 1 child */
  else if(p->p_nextSib == NULL && p->p_prevSib == NULL){
    p->p_parent->p_child = NULL;
    p->p_parent = NULL;
    return p;
  }
  /* more than 1 one child */
  else{
    /* first child is what we're looking for */
    if(p->p_prevSib == NULL){
      p->p_parent->p_child = NULL;
      p->p_parent = NULL;
      p->p_nextSib = NULL;
      return p;
    }
    /* last child is what we're looking for */
    else if(p->p_nextSib == NULL){
      p->p_prevSib->p_nextSib = NULL;
      p->p_prevSib = NULL;
      p->p_parent = NULL;
      return p;
    }
    /*something in the middle to remove */
    else {
      p->p_prevSib->p_nextSib = p->p_nextSib;
      p->p_nextSib->p_prevSib = p->p_prevSib;
      p->p_nextSib = NULL;
      p->p_prevSib = NULL;
      p->p_parent = NULL;
      return p;
    }
  }
}
