#include "mpx_supt.h"
#include <core/serial.h>
#include <string.h>
#include <core/io.h>
//go through and change all serial prints to printString

/**
* Name = help\n
* Query = /help = (parameter);\n
* Parameters: commandName\n
* Description: Returns a string providing information about the desired commandName.
*/
void help(char* param){
if(strlen(param) == 0){
printStringln("\nAvailable Commands are: \t/help; \t/version; \t/shutdown; \t/setdate; \n\t/getdate; \t/settime; \t/gettime; \n\t/deletepcb; \t/setpriority; \n/resume; \t/suspend; \n\t/showready; \t/showblocked; \t/showall;  \t/showpcb; \t/infinite; \t/showallalarms; \t/alarm; \n \t/showallocated; \t/showmem; \t/showfree;");
}
else if(strcmp(param,"version") == 0){
///display help manual page for version
printStringln("\nUsage: \t/version;");
printStringln("Description: \tVersion command will display current version of MPX OS");
printStringln("Parameters: \tnone");
}
else if(strcmp(param,"shutdown") == 0){
///display help manual page for shutdown
printStringln("\nUsage: \t/shutdown;");
printStringln("Description: \tShutdown command will shut the MPX OS down and halt ongoing processes. It will ask for confirmation before truly shutting down. /shutdown y; will confirm a shutdown request.");
}
else if(strcmp(param,"setdate") == 0){
///display help manual page for set_date
printStringln("\nUsage: \t/setdate = [ARG][MM-DD-YEAR];");
printStringln("\nDescription: \tSet Date command will allow user to set the system date");
printStringln("\nParameters: \tMM-DD-YEAR");
}
else if(strcmp(param,"getdate") == 0){
///display help manual page for get_date
printStringln("\nUsage: \t/getdate;");
printStringln("\nDescription: \tGet Date command will allow user to access the current system date");
printStringln("\nParameters: \tnone");
}
else if(strcmp(param,"settime") == 0){
///display help manual page for settime
printStringln("\nUsage: \t/settime  = [ARG][HH-MM-SS];");
printStringln("\nDescription: \tSet Time command will allow user to set the system time");
printStringln("\nParameters \tHH-MM-SS");
}

else if(strcmp(param,"gettime") == 0){
///display help manual page for gettime
printStringln("\nUsage: \t/gettime;");
printStringln("\nDescription: \tGet Time command will allow user to access the current system time");
printStringln("\nParameters: \tnone");
}

else if(strcmp(param,"deletepcb") == 0){
///display help manual page for deleteepcb
printStringln("\nUsage: \t/deletepcb =;");
printStringln("\nDescription: \tDeletes the PCB with the specified PCB if it exists");
printStringln("\nParameters: \tname\n");
}
else if(strcmp(param,"setpriority") == 0){
///display help manual page for setpriority
printStringln("\nUsage: \t/setpriority;");
printStringln("\nDescription: \tSets the priority of a given PCB");
printStringln("\nParameters: \tThe name of a PCB and a priority number\n");
}
else if(strcmp(param,"resume") == 0){
///display help manual page for resume
printStringln("\nUsage: \t/resume;");
printStringln("\nDescription: \tresumes a given pcb");
printStringln("\nParameters: \tName of a pcb\n");
}
else if(strcmp(param,"suspend") == 0){
///display help manual page for suspend
printStringln("\nUsage: \t/suspend;");
printStringln("\nDescription: \tSuspends a pcb");
printStringln("\nParameters: \tname\n");
}
else if(strcmp(param,"showready") == 0){
///display help manual page for showready
printStringln("\nUsage: \t/showready;");
printStringln("\nDescription: \tShows all pcbs in the ready queue");
printStringln("\nParameters: \tnone\n");
}
else if(strcmp(param,"showblocked") == 0){
///display help manual page for showblocked
printStringln("\nUsage: \t/showblocked;");
printStringln("\nDescription: \tShows the queue of blocked PCBs if it exists");
printStringln("\nParameters: \tnone\n");
}
else if(strcmp(param,"showall") == 0){
///display help manual page for showall
printStringln("\nUsage: \t/setpriority;");
printStringln("\nDescription: \tShows all pcbs in both queues");
printStringln("\nParameters: \tnone\n");
}
	else if(strcmp(param,"showpcb") == 0){
	///display help manual page for createpcb
	printStringln("\nUsage: \t/showpcb;");
	printStringln("\nDescription: \tShows all the attributes for a PCB");
	printStringln("\nParameters: \n");
	}
	else if(strcmp(param,"alarm") == 0){
///display help manual page for alarm
printStringln("\nUsage: \t/alarm;");
printStringln("\nDescription: \tcreates alarm");
printStringln("\nParameters: \t hours minutes seconds\n");
}
else if(strcmp(param,"showallalarms") == 0){
///display help manual page for showallalarms
printStringln("\nUsage: \t/showallalarms;");
printStringln("\nDescription: \tshows alarm");
printStringln("\nParameters: \tnone\n");
}
else if(strcmp(param,"infinite") == 0){
///display help manual page for infinite
printStringln("\nUsage: \t/infinite;");
printStringln("\nDescription: \truns the infinite command");
printStringln("\nParameters: \t none\n");
}
else if(strcmp(param,"showallocated") == 0){
///display help manual page for showallocated
printStringln("\nUsage: \t/showallocated;");
printStringln("\nDescription: \tprints the allocated memory in the heap");
printStringln("\nParameters: \t none \n");
}
else if(strcmp(param,"showfree") == 0){
///display help manual page for showfree
printStringln("\nUsage: \t/showfree;");
printStringln("\nDescription: \tprints the free memory in the heap");
printStringln("\nParameters: \t none \n");
}
else if(strcmp(param,"showmem") == 0){
///display help manual page for showmem
printStringln("\nUsage: \t/showmem;");
printStringln("\nDescription: \tprints the heap");
printStringln("\nParameters: \t none \n");
}
	else{
	klogv("Command Invalid!");
	printStringln("\nAvailable Commands are: \t/help; \t/version; \t/shutdown; \t/setdate; \t/getdate; \t/settime; \t/gettime; 	\t/deletepcb; \t/setpriority; \n/resume; \t/suspend; \t/testpcb; \t/showready; \t/showblocked; \t/showall;	\t/showpcb; \t/infinite; \t/showallalarms; \t/alarm; \t/showmem; \t/showallocated; \t/showfree;");
	}
}
