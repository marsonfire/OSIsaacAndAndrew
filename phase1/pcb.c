#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"

/*PTR to the head of queue*/
HIDDEN pcb_PTR pcbFree_h;

/*pcb.e is basically an interface to go off of */


/*
 *Function: Takes in a pcb p and inserts p into 
 * the pcb free list
 */
void freePcb(pcb_PTR p){
  p->p_next = pcbFree_h;
  pcbFree_h = p;
}

/*
 *Function: Takes an element off of the free list, and nullifies
 *its values, setting the pcb up with a new slate. The nullified pcb
 *is then returned for use, typically in a pcb queue. */
pcb_PTR allocPcb (){
    /*creates placeholder */
    pcb_PTR temp;
    if(pcbFree_h == NULL){
        return NULL;
    }
    temp = pcbFree_h;
    pcbFree_h = pcbFree_h->p_next;
    /*queue values to null*/
    temp->p_next = NULL;
    temp->p_prev = NULL;
    /*tree values set to NULL*/
    temp->p_child = NULL;
    temp->p_parent = NULL;
    temp->p_nextSib = NULL;
    temp->p_prevSib = NULL;
    temp->p_semAdd = NULL;
    /* reset phase 2 pointers */
    temp->p_time = 0;
    temp->oldSys = NULL;
    temp->oldPgm = NULL;
    temp->oldTlb = NULL;
    temp->newSys = NULL;
    temp->newPgm = NULL;
    temp->newTlb = NULL;

    /*once all is set, return new element*/
    return temp;
  
}

/*
*Function: Initializes the free list The list is first made empty, 
*and then has a MAXPROC amount of pcbs inserted into the list.
*/
void initPcbs(){
  int i;
  static pcb_t procTable[MAXPROC];                /*create a global array with 20 pcb_t */
  pcbFree_h = mkEmptyProcQ();
  /*set our global queue to be nothing/reset it */
  for(i = 0; i < MAXPROC; i++){
    freePcb(&procTable[i]);       /*put each pcb_t in our global queue*/
  }
}

/*
 *Function: Makes a pcb null
 */
pcb_PTR mkEmptyProcQ (){
  return NULL;
}

/*
*Function: Checks if a pcb is empty or not, and returns the result
*/
int emptyProcQ (pcb_PTR tp){
  return(tp == NULL);
}

/*
 *Function: Inserts a pcb into a queue. There are two states for this:
 *1. There is no queue. Create a queue with the only value being
 *the one to be inserted.
 *2. There is a queue. Insert the pcb as the new tail pointer.
 */
void insertProcQ (pcb_PTR *tp, pcb_PTR p){
  pcb_PTR temp;
  /*we have an empty queue, so create a new one*/
  if(emptyProcQ(*tp)){
    p->p_next = p;  /*p's next points to itself*/
    p->p_prev = p;  /*p's previous points to itself*/
  }
  /*if here, we don't have an empty queue*/
  else{
    p->p_next = (*tp)->p_next;/*make p's next point to the head, which is what tp's (the previous tail) next was*/
    (*tp)->p_next = p;      /* tp is now 2nd to last and p is last so set it's p_next*/
    p->p_prev = (*tp);      /*p's previous is now tp*/
    p->p_next->p_prev = p;    /*p's next = the head, make the head's previous be p so it's not pointing to the previous tail*/
  }
  *tp = p;  /*make the tail pointer point to our new node since it's going to the end*/
}


/*
 *Function: Removes a pcb from the tail.There are 3 cases for this:
 *1. There is no queue. Return NULL.
 *2. There is only 1 element in the queue. Remove that element.
 *3. There are multiple elements. Remove the first pcb and return it.
 */
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
    return head;
  }
}


/*
 *Function: Remove a specific element from the queue. There are 3 cases:
 *1. The element is not in the queue. Return NULL.
 *2. There is only one element. determine if it is the target, and
 *return either NULL or the target based on if it is there.
 *3. Multiple elements in queue. Search for the target, and either return
 *the target or null if not found
 */
pcb_PTR outProcQ (pcb_PTR *tp, pcb_PTR p){
  if(emptyProcQ(*tp)){
    return NULL;
  }
  /*only one element in the queue*/
  else if((*tp)->p_next ==(*tp)){
    pcb_PTR head = (*tp);
    /* the one element in the queue is what we're looking for*/
    if(head == p){
      return removeProcQ((*tp));
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
      return removeProcQ(*tp);
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

/*
 *Function: Returns a pointer to the first pcb, or head. If the queue is
 *empty, return NULL.
 */
pcb_PTR headProcQ (pcb_PTR tp){
  if(emptyProcQ(tp)){
    return NULL;
  }
  else{
    return tp->p_next;
  }
}


/*
 *Return true if the pcb has no children, and false if there are children.
 */
int emptyChild(pcb_PTR p){
  return p->p_child == NULL;
}


/*
 *Function: Make the pcb p a child of pcb parent.
 */
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


/*
 *Function: Removes the first child from the parent. If there are no children,
 *return NULL.
 */
pcb_PTR removeChild(pcb_PTR parent){
  /*no kids, so don't remove anything, just return null */
  if(emptyChild(parent)){
    return NULL;
  }
  pcb_PTR child = parent->p_child;
  /*only one child*/
  if(parent->p_child->p_nextSib == NULL){
    parent->p_child->p_parent = NULL;
    parent->p_child = NULL;
    child->p_parent = NULL;
    child->p_nextSib = NULL;
    child->p_prevSib = NULL;
    return child;
  }
  /* more than one child */
  else{
    parent->p_child = parent->p_child->p_nextSib;
    parent->p_child->p_prevSib = NULL;
    return child;
  }
}


/*
 *Function: Make the pcb p no longer a child of its parent. If p
 *currently has no parent, return NULL.
 */
pcb_PTR outChild(pcb_PTR p){
  /* orphan child has no parents :( */
  if(p->p_parent == NULL){
    return NULL;
  }
  /*only 1 child */
  if(p->p_parent->p_child == p){
    return removeChild(p->p_parent);
  }
  /* more than 1 one child */
  /* last child is what we're looking for */
  if(p->p_nextSib == NULL){
    p->p_prevSib->p_nextSib = NULL;
    p->p_prevSib = NULL;
    p->p_parent = NULL;
    return p;
  }
  /* not the last or only chlild */
  if((p->p_prevSib == NULL) && (p->p_nextSib != NULL)){
    p->p_prevSib->p_nextSib = p->p_nextSib;
    p->p_nextSib->p_prevSib = p->p_prevSib;
    p->p_nextSib = NULL;
    p->p_prevSib = NULL;
    p->p_parent = NULL;
    return p; 
  }
}


