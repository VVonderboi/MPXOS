#include "mpx_supt.h"
#include <core/serial.h>
#include <string.h>
#include <core/io.h>
#include "pcb.h"
#include "showpcbs.h"
#include "createpcb.h"

void deletePCB(char *name);
/** 
* Name = deletepcb\n
* Query = /deletepcb = (parameter);\n
* Parameters: name (1-20 alphanumeric characters)\n
* Description: Deletes the PCB with the specified PCB if it exists.
*/
void deletePCB(char *name){
	if (strlen(name)>20 || strlen(name)==0){
		klogv("Not a valid request. Check name length");
	}
	else{
		PCB* found = FindPCB(name);
		if(RemovePCB(found)==0){
			int result = FreePCB(found);
			result = result+1;
		}
		else{
			klogv("PCB not found, please try with a different name.");
		}
		
	}
	
}
