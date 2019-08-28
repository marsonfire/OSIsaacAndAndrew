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
	//to be written when I have more than 24 minutes
	//I think this and removing are the most important thing we can do right now, so
	//may want to work on that next/together
}

pcb_PTR removeProcQ (pcb_PTR *tp){
	//see above comment
}

pcb_PTR outProcQ (pcb_PTR *tp, pcb_PTR p){

}

pcb_PTR headProcQ (pcb_PTR tp){

}

