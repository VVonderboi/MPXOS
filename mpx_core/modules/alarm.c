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
#include "comhand.h"

typedef struct alarms{
	int alarmHour;
	int alarmMinute;
	int alarmSecond;
	char message[50];
	struct alarms* nextAlarm;
} alarms;

typedef struct alarmQueue{
	struct alarms* head;
	struct alarms* tail;
	int count;
	
} alarmQueue;
alarms* alarm(char* messageIn, char* time);
int checkAlarmQueue();
void alarmChecker();
void insertAlarm(alarms* alarmIn);
alarmQueue* alarmqueued;
void alarmTest();
void showAlarms();

void InitAlarmQueue(){ // initializes the alarm queue
	alarmqueued = (alarmQueue*) sys_alloc_mem(sizeof(alarmQueue));
	memset (alarmqueued, '\0', sizeof(alarmQueue));
	alarmqueued->head=NULL;
	alarmqueued->tail=NULL;
	alarmqueued->count = 0;
}

alarms* alarm(char* messageIn, char* time){
	alarms* newAlarm = (alarms*) sys_alloc_mem(sizeof(alarms)); // allocates a new alarm, and resets the memory
	memset (newAlarm, '\0', sizeof(alarms));
	char timeCopy[strlen(time)]; 
	strcpy(timeCopy,time);
	
	char hourArray[3]; //converts the hours portion of the time string into an int
	memset(hourArray, '\0', 3);
	hourArray[0]=timeCopy[0];
	hourArray[1]=timeCopy[1];
	int hour = atoi(hourArray);
	if (hour > 23 || hour < 0){
		klogv("Not a valid hour, please enter between 0 and 23 inclusive.");
		return NULL;
	}
	
	char minuteArray[3]; //converts the minute portion of the time string into an int
	memset(minuteArray, '\0', 3);
	minuteArray[0]=timeCopy[3];
	minuteArray[1]=timeCopy[4];
	int minute = atoi(minuteArray);
	if (minute > 59 || minute < 0){
		klogv("Not a valid minute, please enter between 0 and 59 inclusive.");
		return NULL;
	}
	
	char secondArray[3]; //converts the seconds portion of the time string into an int
	memset(secondArray, '\0', 3);
	secondArray[0]=timeCopy[6];
	secondArray[1]=timeCopy[7];
	int second= atoi(secondArray);
	if (second > 59 || second < 0){
		klogv("Not a valid second, please enter between 0 and 59 inclusive.");
		return NULL;
	}
	
	memset(newAlarm->message, '\0', 50); // resets the memory for the message, just in case
	newAlarm->alarmHour=hour; // sets the data members of the struct
	newAlarm->alarmMinute=minute;
	newAlarm->alarmSecond=second;
	strcpy(newAlarm->message,messageIn);
	 
	return newAlarm;
}

void removeAlarm(alarms* removeThisAlarm){
	alarms* currentAlarm = alarmqueued->head; 
	if(alarmqueued->count!=0){ // checks if the alarm queue is empty
        if(strcmp(currentAlarm->message,removeThisAlarm->message)==0 && (currentAlarm->alarmSecond==removeThisAlarm->alarmSecond)){ 
            if(alarmqueued->count==1){ //checks the head to see if the messages match, if so sets the alarm queue to empty if the size is 1
                alarmqueued->head=NULL; 
                alarmqueued->tail=NULL; 
                alarmqueued->count--; 
                removeThisAlarm->nextAlarm=NULL; 
                sys_free_mem(removeThisAlarm); // frees the mem from the removed alarm 
                return;
            }
            alarmqueued->head=alarmqueued->head->nextAlarm;  
            alarmqueued->count--; 
            //removeThisAlarm->nextAlarm=NULL; 
            sys_free_mem(removeThisAlarm); 
            return;
        }
        while(currentAlarm->nextAlarm!=NULL){ 
            if(strcmp(currentAlarm->nextAlarm->message,removeThisAlarm->message)==0 && (currentAlarm->nextAlarm->alarmSecond==removeThisAlarm->alarmSecond)){ 
                if(currentAlarm->nextAlarm->nextAlarm!=NULL){ 
                    currentAlarm->nextAlarm=currentAlarm->nextAlarm->nextAlarm; 
                    alarmqueued->count--; 
                    //removeThisAlarm->nextAlarm=NULL; 
                    return; 
                }
                else {
                    currentAlarm->nextAlarm=NULL; 
                    alarmqueued->tail=currentAlarm; 
                    alarmqueued->count--; 
                    //removeThisAlarm->nextAlarm=NULL; 
                    return; 
                }
            }
            currentAlarm = currentAlarm->nextAlarm; 
        }
    }
	 return; 
}


void insertAlarm(alarms* alarmIn){

	if(alarmqueued->head==NULL){ // if the alarm queue is empty when insert is called, a new alarm PCB is created and inserted into the ready queue
		PCB* alarmPCB = SetupPCB("alarm", 0 , 7);
		context* alarmcon = (context* )(alarmPCB->top);
		memset (alarmcon, 0, sizeof(context));
		alarmcon->fs = 0x10 ;
		alarmcon->gs = 0x10 ;
		alarmcon->ds = 0x10 ;
		alarmcon->es = 0x10 ;
		alarmcon->cs = 0x8 ;
		alarmcon->ebp = (u32int)(alarmPCB->stack);
		alarmcon->esp = (u32int)(alarmPCB->top);
		alarmcon->eip = (u32int)  alarmChecker; 
		alarmcon->eflags = 0x202 ; 
		alarmPCB->suspended=0;
		insertPCB(alarmPCB);	
	
	    		alarmqueued->head=alarmIn; // if the alarm queue is empty, inserts an alarm at the head and tail
	    		alarmqueued->tail=alarmIn;
	    		alarmqueued->count++;
    		}
	 else{
	    		alarmqueued->tail->nextAlarm=alarmIn; // if the alarm queue is empty, inserts at the tail
		   		alarmqueued->tail=alarmIn;
		   		alarmqueued->count++;
	    	}
}

void alarmChecker(){

while(1){ // process function for the alarm PCB
	alarms* currentAlarm=alarmqueued->head;
	outb(0x70, 0x04);
	unsigned char hoursIn =inb(0x71); // gets the hours values from the registers and converts them to ints then converts them into seconds
	int temp1 = hoursIn & 0x0F;
	int temp2 = hoursIn >> 4;
	int hours = temp1 + temp2*10;	
	int hourSeconds=3600 * hours;
	
	outb(0x070, 0x02);
	unsigned char minutesIn =inb(0x71); // gets the minutes values from the registers and converts them to ints then converts them into seconds
	int temp3 = minutesIn & 0x0F;
	int temp4 = minutesIn >> 4;
	int minutes = temp3 + temp4*10;
	int minuteSeconds=60*minutes;
	
	outb(0x70, 0x00);
	unsigned char secondsIn =inb(0x71); // gets the seconds values from the registers
	int temp5 = secondsIn & 0x0F;	
	int temp6 = secondsIn >> 4;
	int seconds = temp5 + temp6*10;
	//convert time all into seconds
	seconds = seconds + minuteSeconds+hourSeconds; // adds up all the seconds

	if(checkAlarmQueue()==0){ // if the alarm queue is empty the alarm PCB exits entirely
		sys_req(EXIT, DEFAULT_DEVICE, NULL, NULL);
		}
	while(currentAlarm!=NULL) {
	int currentAlarmSeconds= (currentAlarm->alarmHour*3600)+(currentAlarm->alarmMinute*60)+(currentAlarm->alarmSecond); // gets the seconds for the current alarm
		if(currentAlarmSeconds<=seconds){ // checks the current alarm's time vs the time register seconds
			printString("Alarm message: ");
			printStringln(currentAlarm->message); // prints the alarm message
			removeAlarm(currentAlarm); // removes the alarm if it matches the time
			if(checkAlarmQueue()==0){ // if the alarm queue is empty after removing an alarm from the alarm queue, the alarm PCB exits entirely
				sys_req(EXIT, DEFAULT_DEVICE, NULL, NULL);
			}
		}
		currentAlarm=currentAlarm->nextAlarm;
	}
	sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);
	}
	return;
	
}

int checkAlarmQueue(){ // checks to see if the alarm queue is empty
	if(alarmqueued->count==0){ // changed to head
		return 0;
	}
	return 1;
}

alarmQueue* GetAlarmQ(){
	return alarmqueued;
}

void showAlarms(){ // iterates through the alarm queue and prints out the alarm messages
	if(alarmqueued->head!=NULL){
		alarms* thisAlarm = alarmqueued->head;
        	while(thisAlarm!=NULL){
        		if(thisAlarm==NULL){
        			break;
        		}
		printString("Name: ");
		printStringln(thisAlarm->message);
		char pmessage[50];
		strcpy(pmessage, thisAlarm->message);
		thisAlarm=thisAlarm->nextAlarm;
        	}
    }	
}

/*void alarmTest(){ // temp test method
	alarms* alarm1 = alarm("test1","12/30/45");
	insertAlarm(alarm1);
	alarms* alarm2 = alarm("test2","14/32/56");
	insertAlarm(alarm2);
	alarms* alarm3 = alarm("test3","15/35/25");
	insertAlarm(alarm3);
	alarms* alarm4 = alarm("test4","13/30/45");
	insertAlarm(alarm4);
	alarms* alarm5 = alarm("test5","10/30/45");
	insertAlarm(alarm5);
	alarms* alarm6 = alarm("test6","09/30/45");
	insertAlarm(alarm6);
	alarms* alarm7 = alarm("test7","11/30/45");
	insertAlarm(alarm7);
	showAlarms();
	}*/
