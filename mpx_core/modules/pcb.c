#include "mpx_supt.h"
#include <core/serial.h>
#include <string.h>
#include <core/io.h>
#include "showpcbs.h"
#include "sys_call.h"
#include "deletepcb.h"

//make getready, return ready

/*typedef struct stack
{
	//memset(area,1024,'\0');
	unsigned char* area; //chose diff
	int size;
	unsigned char* top;
	unsigned char* base;
} stack;*/

typedef struct PCB
{
	char processName[20];
	int processClass;   	//0 is application, 1 is a system process
	int priority;		//0 is lowest priority, 9 the highest
	int state;		//1 is ready, 0 is blocked, 2 is running
	int suspended;		//0 is not suspended, 1 is suspended
	
	
	unsigned char stack[1024];
	unsigned char* top;
	unsigned char* base;
	
	struct PCB* nextPCB;
} PCB;
typedef struct queue
{
	int count;
	struct PCB* head;
	struct PCB* tail;
} queue;


PCB* FindPCB(char *name);
void insertPCB(PCB* insertThisPCB);

PCB* AllocatePCB();
int FreePCB(PCB* freeThisPCB);
PCB* SetupPCB(char *name, int class, int priority);
int RemovePCB(PCB* removeThisPCB);
void testPCB();
void InitPCB();
queue* GetReadyQ();
queue* GetBlockedQ();

   queue* Ready;    
   queue* Blocked;
   //int* writeptr;
   //writeptr=1;
/*void testPCB(){
    serial_println("Test:");
    //InitPCB(Ready, Blocked);
    
    
    Ready->count=0; //to start
    Blocked->count=0;
    
    
    PCB* pcb1 = SetupPCB("pcb1",0,5);
    pcb1->state=1;
    //insertPCB here
    insertPCB(pcb1);
    PCB* pcb2 = SetupPCB("pcb2",0,3);
    pcb2->state=1;
    insertPCB(pcb2);
    PCB* pcb3 = SetupPCB("pcb3",1,4);
    pcb3->state=1;
    insertPCB(pcb3);
    PCB* pcb4 = SetupPCB("pcb4",0,2);
    pcb4->state=1;
    insertPCB(pcb4);
    PCB* pcb5 = SetupPCB("pcb5",0,5);
    pcb5->state=1;
    insertPCB(pcb5);
    PCB* pcb6 = SetupPCB("pcb6",0,5);
    pcb6->state=1;
    insertPCB(pcb6);
    PCB* pcb7 = SetupPCB("pcb7",0,2);
    pcb7->state=1;
    insertPCB(pcb7);
    PCB* pcb8 = SetupPCB("pcb8",0,2);
    pcb8->state=1;
    insertPCB(pcb8);
    PCB* pcb9 = SetupPCB("pcb9",0,9);
    pcb9->state=1;
    insertPCB(pcb9);
    PCB* pcb10 = SetupPCB("pcb10",0,6);
    pcb10->state=1;
    insertPCB(pcb10);
    PCB* pcb11 = SetupPCB("pcb11",0,6);
    pcb11->state=1;
    insertPCB(pcb11);
    
    
    serial_println(Ready->head->processName);
    serial_println(Ready->head->nextPCB->processName);
    serial_println(Ready->head->nextPCB->nextPCB->processName);
    serial_println(Ready->head->nextPCB->nextPCB->nextPCB->processName);
    serial_println(Ready->head->nextPCB->nextPCB->nextPCB->nextPCB->processName);
    serial_println(Ready->head->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->processName);
    serial_println(Ready->head->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->processName);
    serial_println(Ready->head->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->processName);
    serial_println(Ready->head->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->processName);
    serial_println(Ready->head->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->processName);
    serial_println(Ready->head->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->nextPCB->processName);
    //serial_println(Ready->count);
    
    
    serial_println("--------");
    serial_println("\nBlocked\n");
    PCB* pcb12 = SetupPCB("pcb12",0,9);
    pcb12->state=0;
    insertPCB(pcb12);
    PCB* pcb13 = SetupPCB("pcb13",0,3);
    pcb13->state=0;
    insertPCB(pcb13);
    PCB* pcb14 = SetupPCB("pcb14",0,6);
    pcb14->state=0;
    insertPCB(pcb14);
    serial_println(Blocked->head->processName);
    serial_println(Blocked->head->nextPCB->processName);
    serial_println(Blocked->head->nextPCB->nextPCB->processName);
    //serial_println(Blocked->count);
    
    serial_println("------\n");
    
    if(FindPCB("pcb9")!=NULL){
        serial_println("found it!");
    }
    serial_println("Removing: ");
    RemovePCB(pcb9);
    if(FindPCB("pcb9")!=NULL){
        serial_println("found it!");
    }
    serial_println("Ready: ");
   // serial_println(Ready->count);
    //serial_println(Ready->head);
    serial_println("Blocked: ");
    //serial_println(Blocked->count);
   printStringln("showpcb");
   ShowPCB("pcb3");
   printStringln("showready");
   ShowReady();
   printStringln("showblocked");
   ShowBlocked();
   printStringln("showall");
   ShowAll();
}*/

/*! 
  Procedure..: InsertPCB
  Description..: PCB into the appropriate queue.
*/
void insertPCB(PCB *PCBIn){
    
    if(PCBIn->state==0){ //0 for blocked
        if(Blocked->count==0){
	    	Blocked->head=PCBIn;
	    	Blocked->count++;
	    	Blocked->tail=PCBIn;
    	}
	    else{
	    	Blocked->tail->nextPCB=PCBIn;
		    Blocked->tail=PCBIn;
	    	Blocked->count++;
	    }
    }
    else if(PCBIn->state==1){ //should be 1 to state ready; priority sort
        if(Ready->count==0){
            Ready->head=PCBIn;
	    	Ready->count++;
	    	Ready->tail=PCBIn;
        }
        
        else{
            if(PCBIn->priority>Ready->head->priority){
                PCBIn->nextPCB=Ready->head;
                Ready->head=PCBIn;//new code, updated head placement
                Ready->count++;
            }
            else if(Ready->count==1 && PCBIn->priority <= Ready->head->priority){
                Ready->head->nextPCB=PCBIn;
                Ready->tail=PCBIn;
                Ready->count++;
            }
            else if(Ready->tail->priority>=PCBIn->priority){
                            Ready->tail->nextPCB=PCBIn;
                            Ready->tail=PCBIn;
                            Ready->count++;
            }
            else{
                PCB* currentPCB = Ready->head;
                
                while(currentPCB->nextPCB!=NULL){
                    
                   if(currentPCB->nextPCB->priority<PCBIn->priority){//checks if curentpcb's nextpcb's priority is less than the currentpcb's priority
                   
                       //temp pointer for currentpcb next
                       //currentpcb next to pcbin
                       //pcbin next to 
                        PCB* tempPCB = currentPCB->nextPCB;
                        currentPCB->nextPCB=PCBIn;
                        PCBIn->nextPCB=tempPCB;
                        Ready->count++;
                        break;
                   }
                   currentPCB=currentPCB->nextPCB;
                   
                }
            }
        }
    }
    else{
        klogv("not proper states");
    }
    
    
}

/*! 
  Procedure..: AllocatePCB();
  Description..: Allocates memory to a PCB 
*/
PCB* AllocatePCB(){
    
    PCB* newPCB = (PCB*) sys_alloc_mem(sizeof(PCB)); 
    
 	return newPCB;
}
 
 /*! 
  Procedure..: FreePCB
  Description..: Frees all memory associated with the given PCB
*/
int FreePCB(PCB* freeThisPCB){ 
	sys_free_mem(freeThisPCB);
	return 0;
}

 /*! 
  Procedure..: SetupPCB
  Description..: Creates an empty PCB and initializes the PCB information. Sets state to ready (0)
*/
PCB* SetupPCB(char *name, int class, int priority){
	char* name2="";
	strcpy(name2,name);
	PCB* newPCB = AllocatePCB();
	strcpy(newPCB->processName,name2);
	newPCB->processClass=class;
	newPCB->priority=priority;
	newPCB->state=1;
	newPCB->suspended=0;
	newPCB->base=newPCB->stack;
	newPCB->top=newPCB->base + 1024 - sizeof(struct context);
	return newPCB;
}

/*! 
  Procedure..: FindPCB
  Description..: Finds the PCB for the given name if it is in either queue and returns the PCB, returns null if it is not found.
*/
PCB* FindPCB(char *name){
    PCB* currentPCB = Blocked->head;
    if(Blocked->count!=0){
        
         if(strcmp(Blocked->head->processName, name)==0){
                return currentPCB;
            }
        
        while(currentPCB->nextPCB!=NULL){
         if(strcmp(currentPCB->nextPCB->processName, name)==0){
                return currentPCB->nextPCB;
            }
            currentPCB=currentPCB->nextPCB;
        }
    }
    
    currentPCB = Ready->head;
    if(Ready->count!=0){
        if(strcmp(Ready->head->processName, name)==0){
            return currentPCB;
        }
        while(currentPCB->nextPCB!=NULL){
        if(strcmp(currentPCB->nextPCB->processName, name)==0){
            return currentPCB->nextPCB;
        }
        currentPCB=currentPCB->nextPCB;
        }
    }
    
    
    return NULL;
}

/*! 
  Procedure..: RemovePCB
  Description..: Will remove the PCB from the queue in which it is stored, does not free the memory of that PCB
*/
int RemovePCB(PCB* removeThisPCB){
    PCB* currentPCB=Blocked->head;
	
    if(Blocked->count!=0){
        if(strcmp(currentPCB->processName,removeThisPCB->processName)==0){
            if(Blocked->count==1){
                Blocked->head=NULL;
                Blocked->tail=NULL;
                Blocked->count--;
                removeThisPCB->nextPCB=NULL;
                return 0;
            }
            Blocked->head=Blocked->head->nextPCB;
            Blocked->count--;
            removeThisPCB->nextPCB=NULL;
            return 0;
        }
        while(currentPCB->nextPCB!=NULL){
            if(strcmp(currentPCB->nextPCB->processName,removeThisPCB->processName)==0){
                if(currentPCB->nextPCB->nextPCB!=NULL){
                    currentPCB->nextPCB=currentPCB->nextPCB->nextPCB;
                    Blocked->count--;
                    removeThisPCB->nextPCB=NULL;
                    return 0;
                }
                else {
                    currentPCB->nextPCB=NULL;
                    Blocked->tail=currentPCB;
                    Blocked->count--;
                    removeThisPCB->nextPCB=NULL;
                    return 0;
                }
            }
            currentPCB=currentPCB->nextPCB;
        }
    }
    
    currentPCB=Ready->head;
    if(Ready->count!=0){
        if(strcmp(currentPCB->processName,removeThisPCB->processName)==0){
            if(Ready->count==1){
                Ready->head=NULL;
                Ready->tail=NULL;
                Ready->count--;
                removeThisPCB->nextPCB=NULL;
                return 0;
            }
            Ready->head=Ready->head->nextPCB;
            Ready->count--;
            removeThisPCB->nextPCB=NULL;
            return 0;
        }
        while(currentPCB->nextPCB!=NULL){
            if(strcmp(currentPCB->nextPCB->processName,removeThisPCB->processName)==0){
                if(currentPCB->nextPCB->nextPCB!=NULL){
                    currentPCB->nextPCB=currentPCB->nextPCB->nextPCB;
                    Ready->count--;
                    removeThisPCB->nextPCB=NULL;
                    return 0;
                }
                else {
                    currentPCB->nextPCB=NULL;
                    Ready->tail=currentPCB;
                    Ready->count--;
                    removeThisPCB->nextPCB=NULL;
                    return 0;
                }
            }
            currentPCB=currentPCB->nextPCB;
        }
    }
    
    
    return 1;
}

/*! 
  Procedure..: InitPCB
  Description..: Allocates and initializes mempory for the two main queues.
*/
void InitPCB(){
	Ready = (queue*) sys_alloc_mem(sizeof(queue));
	Ready->head=NULL;
	Ready->tail=NULL;
	Ready->count=0;
	//set count to 0, head and tail to NULL
    	Blocked = (queue*) sys_alloc_mem(sizeof(queue));
    	Blocked->head=NULL;
	Blocked->tail=NULL;
	Blocked->count=0;
    	
}
//for delete both remove and free

/*! 
  Procedure..: GetReadyQ
  Description..: Passes the Ready queue from pcb.c to other files for use there.
*/
queue* GetReadyQ(){
return Ready;
}

/*! 
  Procedure..: GetBlockedQ
  Description..: Passes the Blocked queue from pcb.c to other files for use there.
*/
queue* GetBlockedQ(){
return Blocked;
}

/*!
  Procedure..: BlockPCB();
  Description..: Puts a PCB in the block queue
*/

int BlockPCB(char *name){

    PCB* blockChecker = FindPCB(name);

   


    //check if PCB name exists
    if (blockChecker == NULL){
    char* nameval = "Name invalid";
        klogv(nameval);
        return 0;
        }
    //check if PCB is already in the blocked queue
    //remove only in order to maintain the spacing
    blockChecker->state=0;
    RemovePCB(blockChecker);
    
    insertPCB(blockChecker);
	return 0;
}

/*!
  Procedure..: unblockPCB();
  Description..: Frees PCB in the block queue
*/

int UnblockPCB(char *name){
  int returner = 0;
  PCB* unblockChecker = NULL;

 
    unblockChecker = FindPCB(name);
    //check to see if pcb name exists
    if(unblockChecker != NULL){
      //check to see if pcb is already ready
      if(unblockChecker->state != 1){
    //need to use RemovePCB instead of Delete because Delete takes removes the allocated memory that the pcb uses
        RemovePCB(unblockChecker);
        unblockChecker->state=1;
        insertPCB(unblockChecker);
      } else {
        //pcb is already ready
     
      }
    } else {
      //pcb does not exist
   char* nameval = "Name invalid";
        klogv(nameval);
        return 0;
    }

    unblockChecker->state=1;
  return returner;
}

void endAll(){
	PCB* currentPCB=Blocked->head;
	PCB* tempVar=NULL;
	while(currentPCB!=NULL){
		tempVar = currentPCB->nextPCB;
		deletePCB(currentPCB->processName);
		currentPCB=tempVar;
	}
	currentPCB=Ready->head;
	while(currentPCB!=NULL){
		tempVar = currentPCB->nextPCB;
		deletePCB(currentPCB->processName);
		currentPCB=tempVar;
	}
	tempVar=NULL;
}


