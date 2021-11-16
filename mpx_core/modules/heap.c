#include "mpx_supt.h"
#include <core/serial.h>
#include <string.h>
#include <core/io.h>
#include "help.h"
#include "date.h"
#include "time.h"
#include "pcb.h"
#include "showpcbs.h"
#include "deletepcb.h"
#include "createpcb.h"
#include "sys_call.h"
#include "procsr3.h"
#include "testProcsr3.h"
#include "alarm.h"
#include "process.h"
#include <mem/heap.h>

typedef struct CMCB{
	int type; //0 is free, 1 is allocated
	u32int beginningAddress;
	u32int size; //in bytes
	struct CMCB* nextCMCB;
	struct CMCB* prevCMCB;
} CMCB;

typedef struct memlist{
	struct CMCB* head;
	struct CMCB* tail;
} memlist;

int initializeHeap(int size);
memlist* newMem;
u32int globalAddr;

char charArray[11];
u32int sizeOfCMCB=sizeof(CMCB);

int initializeHeap(int size){
	globalAddr = kmalloc(size+sizeof(CMCB)+sizeof(memlist));  //**
	newMem=(memlist*) globalAddr;
	CMCB* newCMCB= (CMCB*) (globalAddr + sizeof(memlist));
	newCMCB->type=0;
	newCMCB->size=size;
	newCMCB->beginningAddress=globalAddr+sizeof(CMCB)+sizeof(memlist);
	newCMCB->nextCMCB=NULL;
	newCMCB->prevCMCB=NULL;
	newMem->head=newCMCB;
	newMem->tail=newCMCB;
	return size;
}
CMCB* setupC(int typeIn, int sizeIn,  int addressIn){
	CMCB* CMCBOut=(CMCB*) (addressIn-sizeof(CMCB));
	CMCBOut->type=typeIn;
	CMCBOut->size=sizeIn;
	CMCBOut->beginningAddress = addressIn; 
	CMCBOut->nextCMCB=NULL;
	CMCBOut->prevCMCB=NULL;
	return CMCBOut;
}

//Return pointer to alloc mem or NULL
void *allocateMem(u32int size){

	//First Fit strategy (look for open block that's big enough, whatever is first we find, that's what is allocated)
	//Block chosen will split up into 2 chunks, 1 to fill request, 1 to rejoin free
	CMCB* currentCMCB=newMem->head;//Parent
	//Update head
	while(currentCMCB!=NULL){
		if( currentCMCB->size >= (size+sizeOfCMCB) && currentCMCB->type == 0 ){//Allocate new block here
			//When a new block is being made, allocate at top)
			//Takes the address of the beginning of the parent block & make new block /w it
			//New block's prev is parent block's prev
			//Next block is parent block
			//Parent's prev points to new cmcb
			//Parent's next is same as was
		
			CMCB* CMCBFree = setupC(0, currentCMCB->size - size - sizeof(CMCB),  currentCMCB->beginningAddress+size+sizeof(CMCB));
			CMCBFree->prevCMCB = currentCMCB;
			CMCBFree->nextCMCB = currentCMCB->nextCMCB;
			currentCMCB->nextCMCB = CMCBFree;
			//NextCMCB->next = same as was;
			currentCMCB->size = size;
			currentCMCB->type = 1;
			return (void *) currentCMCB->beginningAddress;
		}
		currentCMCB=currentCMCB->nextCMCB;
	}
	klogv("Memory couldn't be allocated, no blocks big enough.\n");
	return (void *) NULL;
}
int freeMem(void *ptr){
//0 being ok, else is error code for return
	CMCB* currentCMCB=newMem->head;
	//Update tail
	while(currentCMCB != NULL){
	    if(currentCMCB->beginningAddress == (u32int) ptr){
		if(currentCMCB->type == 1){
			currentCMCB->type=0;
			if(currentCMCB->nextCMCB!=NULL && currentCMCB->nextCMCB->type == 0){//merge the current & next; this merges down
			//When merge, for next, need to set, going forward, the currentCMCB->nextCMCB needs to equal 
			//Merge the two sizes of the blocks (and size of CMCB)after doing the equalling 
				currentCMCB->size = (currentCMCB->size + currentCMCB->nextCMCB->size + sizeof(CMCB));
				if(currentCMCB->nextCMCB->nextCMCB != NULL){
					currentCMCB->nextCMCB=currentCMCB->nextCMCB->nextCMCB;
					currentCMCB->nextCMCB->prevCMCB = currentCMCB;
				}
				else{//If the nextcmcb after the one being deleted is null, is most likely tail, so update tail
					currentCMCB->nextCMCB=NULL;
					newMem->tail=currentCMCB;
				}
			}
			if(currentCMCB->prevCMCB!=NULL && currentCMCB->prevCMCB->type == 0){//Now need to merge up
				currentCMCB->prevCMCB->size = (currentCMCB->size + currentCMCB->prevCMCB->size + sizeof(CMCB));
				if(currentCMCB->nextCMCB != NULL){
					currentCMCB->prevCMCB->nextCMCB=currentCMCB->nextCMCB;
					currentCMCB->nextCMCB->prevCMCB=currentCMCB->prevCMCB;
				}
				else{
					currentCMCB->prevCMCB->nextCMCB=NULL;
					newMem->tail=currentCMCB->prevCMCB;
				}
				currentCMCB=NULL;
			}
			return 0; //All ok
		}
		else{
		 klogv("Block already freed.");
		 return 0;
		}
	}
	currentCMCB=currentCMCB->nextCMCB;
	}
	klogv("Could not find the block to free.\n");
	return 1; //Couldn't allocate
}

void showAllocated(){
	CMCB* currentCMCB=newMem->head;
	printStringln("Allocated: ");
	while(currentCMCB!=NULL){
		if(currentCMCB->type==1){
			printString( itoa(currentCMCB->size) );//type and size and address
			printString(" bytes, Address is: ");
			printStringln(itoa(currentCMCB->beginningAddress));
		}
		currentCMCB=currentCMCB->nextCMCB;
	}
}

void showFreed(){
	CMCB* currentCMCB=newMem->head;
	printStringln("Freed:");
	while(currentCMCB!=NULL){
		if(currentCMCB->type==0){
			printString( itoa(currentCMCB->size) );//type and size and address
			printString(" bytes, Address is: ");
			printStringln(itoa(currentCMCB->beginningAddress));
		}
		currentCMCB=currentCMCB->nextCMCB;
	}
}

void showAll(){
    CMCB* currentCMCB=newMem->head;
	printStringln("CMCB's:\n");
	while(currentCMCB!=NULL){

		if(currentCMCB->type==0){
			printStringln("Free: ");
			printString( itoa(currentCMCB->size) );//type and size and address
			printString(" bytes, Address is: ");
			printStringln(itoa(currentCMCB->beginningAddress));
		}
		else{
		    printStringln("Allocated: ");  
		    printString( itoa(currentCMCB->size) );//type and size and address
		    printString(" bytes, Address is: ");
			printStringln(itoa(currentCMCB->beginningAddress));
		}
		
		currentCMCB=currentCMCB->nextCMCB;
	}
}

int isEmpty(){
	CMCB* currentCMCB=newMem->head;
	if(currentCMCB==newMem->tail){
		return 0;
	}
	return 1;//0 is true, 1 is false
}
