#include "mpx_supt.h"
#include <core/serial.h>
#include <string.h>
#include <core/io.h>
#include "pcb.h"
#include "showpcbs.h"
#include "deletepcb.h"

void createPCB(char *name, int class, int priority);

/** 
* Name = createpcb\n
* Query = /createpcb = (parameter);\n
* Parameters: name/class/priority ([1-20 alphanumeric characters]/y/z)\n
* Description: Creates a new PCB based on the parameters and inserts into the queue.
*/
void createPCB(char *name, int class, int priority){
	PCB* found = FindPCB(name);
	if (strlen(name)>20 || strlen(name)==0 || class > 1 || class < 0 || priority > 9 || priority < 0){
		klogv("Not a valid request. Check attribute ranges");
	}
	
	else{
		if(found!=NULL){
			klogv("Already a PCB with same name.");
		}
		else{
			PCB* newPCB = SetupPCB(name, class, priority);
			insertPCB(newPCB);
		}		
	}
}
