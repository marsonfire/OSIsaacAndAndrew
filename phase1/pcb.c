#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"

/*PTR to the head of the free list */
HIDDEN pcb_PTR pcbFree_h;

//take something off the free list to be used
pcb_PTR allocPCB(){

}

//puts pcb p back on the free list
void freePCB(pcb_PTR p){

}

//init the 20 pcb's I think
void initPCBs(){

}

//makes a pcb null/empty
pcb_PTR mkEmptyProcQ (){
	return null;
}

//checks if a pcb is empty or not
int emptyProcQ (pcb_PTR tp){
	return(tp == null);
}