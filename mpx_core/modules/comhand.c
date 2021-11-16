/*
* One space is required bewteen words and symbols in commands, excluding semi-colons
* Ex. /help = version;
*     /setdate = 12/30/2000;
*/

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
#include "heap.h"

const char delim[2] = " ";
int flagGlobal=0;
int infiniteflag= 0;

void bcdToI(unsigned char bcdValue);
unsigned char iToBcd(int intVal);
void testPCB();

/*! 
  Procedure..: parameter
  Description..: take the input from the user and parses out the paramaters from the query
  	ex.
  		query =>/settime = 12/34/25;
  		parameter =>12/34/25
  	
*/

char* parameter(char* cmdBuffer) {
	int flag = 0;
	cmdBuffer = cmdBuffer + strlen(cmdBuffer)+1; // takes the length of the current strtok and adds it to the index of the buffer then adds 1 to get to the '=' part of the query
	int i = 0; // index for loop size
	int paramIndex = 0;  // parameter index
	char param[50]; // parameter string
	memset(param, '\0', 50); // resets the memory for params
	char* returnValue;
	if ((char)*cmdBuffer != '='){ // if a '=' is not used in the query, exits and throws a syntax error
			klogv("syntax error");
			returnValue = param;
			return returnValue;
		}
		cmdBuffer = cmdBuffer+1; // else there is a '=' so the buffer index moves by 1 forward where there should be a space before the parameters exist
	if ((char)*cmdBuffer == ' '){ // checks for a ' ' (space) after the '='
			cmdBuffer = cmdBuffer+1; // moves the index forward to the parameter
		}

	while(i<100){

		if (*cmdBuffer == ';'){ // if the ';' is detected breaks out of the while loop
			flag=1;
			break;
		}
		else{ // else every character is added to the param string
			param[paramIndex] = *cmdBuffer;
			paramIndex++;
		}
		cmdBuffer = cmdBuffer+1; // moves the buffer index forward by 1
		i++;
	}
	if(flag==0){
		klogv("Syntax error");
		returnValue = " ";
		return returnValue;
	}
	returnValue = param;
		
		cmdBuffer = 0;
      	return returnValue;
   }
   
/*! 
  Procedure..: comhand
  Description..: comhand() takes the query given by the user using polling() and decides which command needs to be executed	
*/

int comhand(){
	char cmdBuffer[100];
	int bufferSize = 0;
	int quit = 0;
	help("");
	while(!quit){
		
		//get a command
		memset(cmdBuffer, '\0', 100);
		bufferSize=99; //reset size before each call to read
		sys_req(READ,DEFAULT_DEVICE,cmdBuffer,&bufferSize); // polling is called here
		char* bufferCopy = cmdBuffer;
		//serial_print("buffer copy =>");
		//serial_println(bufferCopy);
// Commands
		
/*! 
* Name = shutdown\n
* Query = /shutdown;\n
* Parameters: N/A\n
* Description: Shuts down the command handler, no longer able to input queries. Asks for confirmation before truly shutting down toamke sure the user intended to do so.
*/
		
		
		
		if (strcmp(cmdBuffer,"/shutdown;") == 0 || strcmp(cmdBuffer,"/shutdown y;")==0 || strcmp(cmdBuffer,"/shutdown n;")==0){
			if(strcmp(cmdBuffer,"/shutdown;")==0){
				serial_println("Are you sure you want to shut down? /shutdown y; to confirm or /shutdown n; to stop");
				//printStringln("Are you sure you want to shut down? /shutdown y; to confirm or /shutdown n; to stop");
				flagGlobal=1;
				continue;
			}
			
			else if(flagGlobal==1 && strcmp(cmdBuffer,"/shutdown y;")==0){
				endAll();
				sys_req(EXIT, DEFAULT_DEVICE, NULL, NULL);
			}
			else if(flagGlobal==1 && strcmp(cmdBuffer,"/shutdown n;")==0){
				flagGlobal=0;
				continue;
			}
		}
		
		
/** 
* Name = version\n
* Query = /version;\n
* Parameters: N/A\n
* Description: Returns the version of the project.
*/
		
		else if (strcmp(cmdBuffer,"/version;") == 0 ){
		printStringln("");
		printStringln("version");
		printStringln("MPX R6 4/30/2021");
		}
		
/** 
* Name = help\n
* Query = /help = (parameter);\n
* Parameters: commandName\n
* Description: Returns a string providing information about the desired commandName.
*/
		
		else if (strcmp(strtok(cmdBuffer,delim),"/help") == 0 ){ /// Strtok takes the left side of the '=' in the cmdBuffer string and compares to "/help " to check the command before the parameters
			char* parameters = parameter(bufferCopy);
			help(parameters);
		}
		
/** 
* Name = setDate\n
* Query = /setDate = (parameter);\n
* Parameters: month/day/year (xx/yy/zzzz)\n
* Description: Sets the date based on the inputted string.
*/
		
		else if (strcmp(strtok(cmdBuffer,delim),"/setdate") == 0 ){ 
			char* date = parameter(bufferCopy);
			if(strcmp(date," ")!=0){
				char dateCopyNew[strlen(date)];
				strcpy(dateCopyNew,date);
				setDate(dateCopyNew);
			}
		}
		
/** 
* Name = getDate\n
* Query = /getDate;\n
* Parameters: N/A\n
* Description: Returns the date in  string form.
*/
		
		else if (strcmp(cmdBuffer,"/getdate;") == 0 ){ 
			getDate();
		}
		
/** 
* Name = setTime\n
* Query = /setTime = (parameter);\n
* Parameters: hours:minutes:seconds (xx/yy/zz)\n
* Description: Sets the time based on the inputted string.
*/
		
		else if (strcmp(strtok(bufferCopy,delim),"/settime") == 0 ){ 
			char* time = parameter(bufferCopy);
			
			if(strcmp(time," ")!=0){
				char timeCopyNew[strlen(time)];
				strcpy(timeCopyNew,time);
				setTime(timeCopyNew);
			}
			
		}

/** 
* Name = getTime\n
* Query = /getTime;\n
* Parameters: N/A\n
* Description: Returns the time in  string form.
*/
		
		else if (strcmp(cmdBuffer,"/gettime;") == 0 ){ 
			getTime();
		}
		
/** 
* Name = createpcb\n
* Query = /createpcb = (parameter);\n
* Parameters: name/class/priority ([1-20 alphanumeric characters]/y/z)\n
* Description: Creates a new PCB based on the parameters and inserts into the queue.

		
		else if (strcmp(strtok(bufferCopy,delim),"/createpcb") == 0 ){ 
			char* params = parameter(bufferCopy);
			
			if(strcmp(params," ")!=0){
				char* createName = strtok(params,"/");
				int class = atoi(strtok(NULL,"/"));
				int priority = atoi(strtok(NULL,"/"));
				createPCB(createName, class, priority);
			}
			
		}*/
		
/** 
* Name = deletepcb\n
* Query = /deletepcb = (parameter);\n
* Parameters: name (1-20 alphanumeric characters)\n
* Description: Deletes the PCB with the specified PCB if it exists.
*/
		
		else if (strcmp(strtok(bufferCopy,delim),"/deletepcb") == 0 ){ 
			char* name = parameter(bufferCopy);
			PCB* inPCB = FindPCB(name);
			if(strcmp(name,"infinite")==0){
			
				if(inPCB->suspended==1){
					deletePCB(name);
				}else{
				klogv("Infinite is not suspended cannot delete");
				}
				
				
			}else{
				if(inPCB->processClass==0){
					deletePCB(name);
				}
				else{
					klogv("System process, cannot delete.");
				}
			}
			
		}
		
		
		/**
* Name = setpriority\n
* Query = /setpriority = (PCBName)/(priority);\n
* Parameters: The name of a PCB and a priority number\n
* Description: Sets the priority of a given PCB
*/

		else if (strcmp(strtok(bufferCopy,delim),"/setpriority") == 0 ){

		char* param = parameter(bufferCopy);
		char* PCBName = strtok(param,"/");
		if(FindPCB(PCBName)->processClass==0){
		int prio = atoi(strtok(NULL,"/"));
		if (prio <= 9 && prio >= 0) { // checks to make sure the priority is between 0 and 9
			if(strcmp(PCBName," ")!=0){ // checks to make sure the PCB name is not empty
				PCB* pcb = FindPCB(PCBName); // gets the PCB pointer from findPCB
			if (pcb == NULL) {
				klogv("Invalid PCB name.");
			}
			else {
				pcb->priority = prio; // updates the priority of the PCB
				RemovePCB(pcb); // removes the PCB from the current queue it is in
				insertPCB(pcb); // inserts the PCB back into the appropriate queue
			}
			}
		}
		else {
			klogv("Invalid priority integer given.");
		}
		}
		else{
			klogv("System process. Cannot change priority.");
		}
		}

/**
* Name = resume\n
* Query = /resume = (PCBName);\n
* Parameters: The name of a PCB\n
* Description: Resumes a PCB
*/

		else if (strcmp(strtok(bufferCopy,delim),"/resume") == 0 ){
		
			char* PCBName = parameter(bufferCopy);
			PCB* pcb = FindPCB(PCBName);
			if (pcb == NULL) {
				klogv("Invalid PCB name.");
			}
			else {
				if (pcb->suspended == 0) { // checks to see if the PCB is already resumed
					klogv("PCB is already resumed."); // does nothing to the PCB
				}
				else {
					if(pcb->processClass==0){
					pcb->suspended = 0; // updates the PCB's suspended state to 0
					RemovePCB(pcb); // removes the PCB from the current queue it is in
					insertPCB(pcb); // inserts the PCB back into the appropriate queue
					}
					else{
						klogv("System process. Unable to resume.");
					}
				}
			}
		}

/**
* Name = suspend\n
* Query = /suspend = (PCBName);\n
* Parameters: The name of a PCB\n
* Description: Suspends a PCB
*/

		else if (strcmp(strtok(bufferCopy,delim),"/suspend") == 0 ){
	
			char* PCBName = parameter(bufferCopy);
			PCB* pcb = FindPCB(PCBName);
			if (pcb == NULL) {
			klogv("Invalid PCB name.");
			}
			else {
				if (pcb->suspended == 1) { // checks to see if the PCB is already suspended
					klogv("PCB is already suspended.");	// does nothing to the PCB
				}
				else {
					if(pcb->processClass==0){
					pcb->suspended = 1; // updates the PCB's suspended state to 1
					RemovePCB(pcb); // removes the PCB from the current queue it is in
					insertPCB(pcb); // inserts the PCB back into the appropriate queue
					}
					else{
						klogv("System process. Unable to suspend.");
					}
				}
			}
		}
		

		/*else if (strcmp(cmdBuffer,"/testpcb;") == 0 ){ 
			testPCB();
		}*/
/** 
* Name = ShowReady\n
* Query = /showready;\n
* Parameters: N/A\n
* Description: Shows all PCBs in ready queue.
*/

		else if (strcmp(cmdBuffer,"/showready;") == 0 ){ 
			printStringln("");			
			ShowReady();
		}
/** 
* Name = ShowBlocked\n
* Query = /showblocked;\n
* Parameters: N/A\n
* Description: Shows all PCBs in blocked queue
*/
		
		else if (strcmp(cmdBuffer,"/showblocked;") == 0 ){ 
			printStringln("");
			ShowBlocked();
		}
/** 
* Name = ShowAll\n
* Query = /showall;\n
* Parameters: N/A\n
* Description: Shows all PCB's in both queues
*/

		else if (strcmp(cmdBuffer,"/showall;") == 0 ){ 
			printStringln("");
			ShowAll();
		}
/** 
* Name = ShowPCB\n
* Query = /showpcb = (PCBName);\n
* Parameters: N/A\n
* Description: Shows all the attributes for a PCB
*/

		else if (strcmp(strtok(bufferCopy,delim),"/showpcb") == 0 ){
	
			char* PCBName = parameter(bufferCopy);
			ShowPCB(PCBName);
		}

		/*else if (strcmp(cmdBuffer,"/headtail;") == 0 ){ 
			queue* q = (queue*) GetReadyQ();
			serial_println(q->head->processName);
			serial_println(q->tail->processName);
		}*/
		
		/**
* Name = BlockPCB\n
* Query = /BlockPCB = (pcbName);\n
* Parameters: Name of a PCB\n
* Description: Puts a PCB on the block queue


		else if (strcmp(strtok(cmdBuffer,delim),"/blockpcb") == 0 ){ /// Strtok takes the left side of the '=' in the cmdBuffer 			string and compares to "/help " to check the command before the parameters
			char* name = parameter(bufferCopy);
			BlockPCB(name);
		}*/

/**
* Name = UnblockPCB\n
* Query = /UnblockPCB = (parameter);\n
* Parameters: commandName\n
* Description: Unblocks a PCB.


		else if (strcmp(strtok(cmdBuffer,delim),"/unblockpcb") == 0 ){ /// Strtok takes the left side of the '=' in the cmdBuffer 			string and compares to "/help " to check the command before the parameters
			char* parameters = parameter(bufferCopy);
			UnblockPCB(parameters);
		}*/
		
		
		
		//yield goes here
		/*else if(strcmp(cmdBuffer,"/yield;") == 0){
			asm volatile("int $60");
			sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);
		}*/
		
		
		
		
		//loadr3 goes here
		else if(strcmp(cmdBuffer,"/loadr3;") == 0){
			//ask user for pcb name; 
			testProcsr3();
			
		}
		
		//////////////////////////////////////////////// TEST FOR ALARM
		/*else if(strcmp(cmdBuffer,"/test;") == 0){
			
			alarmTest();
			
		}*/
		//////////////////////////////////////////////////////

		else if(strcmp(strtok(bufferCopy,delim),"/alarm") == 0 ){ //alarm = hh/mm/ss/message;
			char* alarmstring = parameter(bufferCopy);
			if(strcmp(alarmstring," ")!=0){
				char alarmStringNew[strlen(alarmstring)];
				strcpy(alarmStringNew,alarmstring);
				if(strlen(alarmStringNew)<=10){
					klogv("Wrong format, 2+ chars for message and 2 characters for time");
					continue;
				}
				//two arrays, 1 is first 8 char for time, second is message (9-strlen)
				char timearray[10];
				timearray[0]=alarmStringNew[0];
				timearray[1]=alarmStringNew[1];
				timearray[2]=alarmStringNew[2];
				timearray[3]=alarmStringNew[3];
				timearray[4]=alarmStringNew[4];
				timearray[5]=alarmStringNew[5];
				timearray[6]=alarmStringNew[6];
				timearray[7]=alarmStringNew[7];
				timearray[8]=alarmStringNew[8];
				int arraysize =strlen(alarmStringNew) - 10;
				char messagearray[arraysize];	
				int i=0;
				int j=9;
				while(i<=strlen(alarmStringNew) - 10){
					messagearray[i]=alarmStringNew[j];
					i++;
					j++;					
				}
				
				alarms* newAlarm = alarm(messagearray, timearray);
				insertAlarm(newAlarm);
				
			}
		}
		
		else if(strcmp(cmdBuffer,"/showallalarms;") == 0){
			showAlarms();
		}
		else if(strcmp(cmdBuffer,"/infinite;") == 0){
			infiniteProcess();
		}
		
/**
* Name = isempty\n
* Query = /isempty;\n
* Parameters: None\n
* Description: Returns and prints if the heap is empty
*/
		else if(strcmp(cmdBuffer,"/isempty;") == 0){ //temporary command left in for our testing
			printStringln( itoa(isEmpty()) );
		}
/**
* Name = showallocated\n
* Query = /showallocated;\n
* Parameters: None\n
* Description: Prints the allocated memory in the heap.
*/		
		else if(strcmp(cmdBuffer,"/showallocated;") == 0){
			showAllocated();
		}
/**
* Name = showfree\n
* Query = /showfree;\n
* Parameters: None\n
* Description: Prints the free portions of memory in the heap
*/		
		else if(strcmp(cmdBuffer,"/showfree;") == 0){
			showFreed();
		}
/**
* Name = showmem\n
* Query = /showmem;\n
* Parameters: None\n
* Description: Prints the heap
*/		
		else if(strcmp(cmdBuffer,"/showmem;") == 0){
			showAll();
		}
		
		else if(strcmp(cmdBuffer, "/clear;") == 0){
			printString("\x1B[2J");
			printString("\x1B[H");
			
		}
		else {
			klogv("Invalid Command");
		}
		
	sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);
	}
	return 0;
	//process the command
	//see if quit was entered
}
/** 
  Procedure..: bcdToI\n
  Description..: takes an unsigned char, splits it into two halves where one is &'ed with 0x0F and the other is shifted 4 bits to the right, adds '0' to them to convert them from ints to chars, and returned together with a string
  	
*/
void bcdToI(unsigned char bcdValue){
	int temp1 = bcdValue & 0x0F;
	int temp2 = bcdValue >> 4;
	char char1 = temp1 + '0';
	char char2 = temp2 + '0';
	char stringNew[3];
	stringNew[1]=char1;
	stringNew[0]=char2;
	stringNew[2]='\0';
	printString(stringNew);
}
/** 
  Procedure..: iToBcd\n
  Description..: takes an int, splits it into two halves where one is divided by 10 and shifted left by 4 bits and the other is modulo'ed by 10, and returns them added together
  	
*/
unsigned char iToBcd(int intVal){
	unsigned char temp3 =  (intVal/10) << 4 ;
	unsigned char temp4 = intVal % 10;
	/*char stringM[3];
	stringM[0]=temp4;
	stringM[1]=temp3;
	stringM[2]='\0';
	serial_println(stringM);*/
	return temp3+temp4;
}
