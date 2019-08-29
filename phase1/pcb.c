#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"

//PTR to the head of the free list
HIDDEN pcb_PTR pcbFree_h;

//pcb.e is basically an interface to go off of 

//take something off the free list to be used
pcb_PTR allocPCB(){

}

//puts pcb p back on the free list
void freePCB(pcb_PTR p){
	//insertProcQ here but into the freePCB list
}

//allocate space for pcbs?
pcb_PTR allocPcb (){
	if(pcbFree_h == null){
    	return null;//Return null if pcbFree list is empty
 	}
	//remove list element here once implemented/figured out


	//creates placeholder, sets to current list element
	pcb_PTR = temp;
	temp = pcbFree_h;

	pcbFree_h = pcbFree_h -> p_next;//cycles element
	//queue values to null
	temp -> p_next = null;
	temp -> p_prev = null;

	//field values to be set to null here, will double check those soon

	//once all is set, return new element
	return temp;

}

//create 20 of them to go on the free list?
void initPcbs (){

}

//makes a pcb null/empty
pcb_PTR mkEmptyProcQ (){
	return null;
}

//checks if a pcb is empty or not
int emptyProcQ (pcb_PTR tp){
	return(tp == null);
}

void insertProcQ (pcb_PTR *tp, pcb_PTR p){
	//we have an empty queue, so create a new one
	if(emptyProcQ()){
		p->p_next = p;  //p's next points to itself
		p->p_prev = p;	//p's previous points to itself
		*tp = p;		//the tail pointer is p since it's the only node in the queue
	}
	//if here, we don't have an empty queue
	else{
		pcb_PTR temp = *tp;			//save the tail pointer in a temp var to be used later
		*tp = p;					//make the tail pointer point to our new node since it's going to the end
		p->p_next = temp->p_next;	//make p's next point to the head, which is what temp's (the previous tail) next was
		temp->p_next = p;			//temp is now 2nd to last and p is last so set it's p_next
		p->p_prev = temp;			//p's previous is now temp
		p->p_next->p_prev = p;		//p's next = the head, make the head's previous be p so it's not pointing to the previous tail
	}
}

pcb_PTR removeProcQ (pcb_PTR *tp){
	
}

pcb_PTR outProcQ (pcb_PTR *tp, pcb_PTR p){

}

pcb_PTR headProcQ (pcb_PTR tp){

}

