#include "mpx_supt.h"
#include <core/serial.h>
#include <string.h>
#include <core/io.h>
#include "pcb.h"

void checkPriority(int pr);





/*! 
  Procedure..: ShowPCB
  Description..: Displays the name, class, state, suspended status and priority for a given process name.
*/
void ShowPCB(char* name){
	PCB* process = FindPCB(name);
	if(process==NULL){
		klogv("PCB does not exist");
	}else{
		//serial_println(process->processName);
		//add other things once printString is sorted
		printString("Name: ");
		char* pname= process->processName;
		printStringln(pname);
		
		
		printString("Class: ");
		if(process->processClass == 0){
		printStringln("0 Application");
		}else if(process->processClass == 1) {
		printStringln("1 Sys Process");
		}else{
		printStringln("invalid class code");
		}
		
		
		printString("Priority: ");
		int ipri = process->priority;
		checkPriority(ipri);


		printString("State: ");
		if(process->state == 0){
		printStringln("0 Blocked");
		}else if(process->state == 1) {
		printStringln("1 Ready");
		}else{
		printStringln("invalid state code");
		}
		
		
		printString("Suspend status: ");
		if(process->suspended == 0){
		printStringln("Not suspended");
		}else if(process->suspended == 1) {
		printStringln("Suspended");
		}else{
		printStringln("invalid suspension code");
		}
		
		printStringln("");
		
		
	}

}

/*! 
  Procedure..: ShowReady
  Description..: Displays the name, class, state, suspended status and priority for the whole Ready queue
*/
void ShowReady(){
	queue* Ready = (queue*) GetReadyQ();
	PCB* currentPCB = Ready->head;
	
    	if(Ready->count!=0){
     
        	while(currentPCB!=NULL){
        		ShowPCB(currentPCB->processName);
        		
       		currentPCB=currentPCB->nextPCB;
        	}
    	}
}

/*! 
  Procedure..: ShowBlocked
  Description..: Displays the name, class, state, suspended status and priority for the whole blocked queue.
*/
void ShowBlocked(){
	queue* Blocked = (queue*) GetBlockedQ();
	PCB* currentPCB = Blocked->head;
	
    	if(Blocked->count!=0){
     
        	while(currentPCB!=NULL){
        		ShowPCB(currentPCB->processName);
       		currentPCB=currentPCB->nextPCB;
        	}
    	}
}

/*! 
  Procedure..: ShowAll
  Description..: Displays the name, class, state, suspended status and priority for both ready and blocked queues.
*/
void ShowAll(){
	printStringln("Blocked Queue");
	printStringln("");
	ShowBlocked();
	printStringln("");
	printStringln("Ready Queue");
	printStringln("");
	ShowReady();
	printStringln("");
}

/*! 
  Procedure..: Check priority
  Description..: checks the priority of a pcb prints out priority in showpcb
*/
void checkPriority(int pr){

switch (pr)
{
    case 0: printStringln("0");
        break;
    case 1: printStringln("1");
        break;
    case 2: printStringln("2");
        break;
    case 3: printStringln("3");
        break;
    case 4: printStringln("4");
        break;
    case 5: printStringln("5");
        break;
    case 6: printStringln("6");
        break;
    case 7: printStringln("7");
        break;
    case 8: printStringln("8");
        break;
    case 9: printStringln("9");
        break;
    default: break; // code to be executed if n doesn't match any cases
}
return;
}
