#include "mpx_supt.h"
#include <core/serial.h>
#include <string.h>
#include <core/io.h>
#include "comhand.h"
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
#include <mem/heap.h>
#include <core/tables.h>

typedef struct DCB{
	int portflag; //determines if port open or closed; 0 for closed, 1 for open
	int* eventflag; //"this is a far pointer to an integer event flag." (from Detailed document)
	char* statuscode; //idle, reading, writing are possible values
	int incount; //counters for input buffer
	int outcount; //counters for output buffer
	unsigned char* inbuffer;
	unsigned char* outbuffer;
	
} DCB;

typedef struct IOCB{
	//int vector; interrupt vector table
	//needs pointer to PCB from, pointer to DCB to, buffer gave, count gave, opcode, pointer to next IOCB for queue
	PCB* inPCB;
	DCB* DCBTo;
	int countIn;
	char* opcode;
	struct IOCB* nextIOCB;
	//int evtflag; get from DCB
	//int interrID; same
	
} IOCB;

typedef struct IOCBQueue{
	struct IOCB* queueHead;
	struct IOCB* queueTail;

} IOCBQueue;

/*typedef struct IOBuffer{ //IOCB queue
	IOCB* head;
	IOCB* tail;
} IOBuffer;*/

typedef struct RingBuffer{
	char ring[100];
	char ringHead;
	char ringTail;	
	int inputindex;
	int outputindex;
	int counter;
} RingBuffer;

typedef struct wait{
	struct IOCB* headIOCB;
	struct IOCB* tailIOCB;
}wait;

RingBuffer* newRing;
int i=0;
int headI=0;
DCB* newDCB;
char* globalevent;
//int oldHandler;
int baud_div;
IOCB* newIOCB;
int actualcount=0;
wait* newWait;
IOCBQueue* newIOCBQueue;
void mainHandle();
int inputHandle();
int outputHandle();


/*! 
  Procedure..: com_open
  Description..: Will take in the event flag and baud rate, calculate the baud rate divisor, and initialize the DCB and Ring Buffer to open the COM ports.
*/
int com_open (int *eflag_p, int baud_rate){ //map 24 here
	
	//1. ensure valid params and device not open
	if(eflag_p!=NULL && (baud_rate == 110 || baud_rate == 150 || baud_rate == 300 || baud_rate == 600 || baud_rate == 1200 || baud_rate == 2400 || baud_rate == 4800 || baud_rate == 9600 || baud_rate == 19200)){//check eflag_p & baud_rate
		if(newDCB->portflag==0){//check if open
			//2. init DCB;
			newDCB->portflag = 1;
			newDCB->eventflag = eflag_p; //"this is a far pointer to an integer event flag." (from Detailed document); int*
			newDCB->statuscode = "IDLE"; //idle, reading, writing are possible values; char*

			//also init RingBuffer
			newRing->ring[0]=NULL;
			newRing->ringHead=newRing->ring[headI];
			newRing->ringTail=newRing->ring[i];
			newRing->inputindex= i;
			newRing->outputindex= i;
			newRing->counter = i;
			newWait->headIOCB=NULL;
			newWait->tailIOCB=NULL;
			newIOCBQueue->queueHead=NULL;
			newIOCBQueue->queueTail=NULL;
		
			//3. save address of current interrupt handler & put in new one
			//oldHandler = idt_get_gate(24);
			idt_set_gate(24, (u32int)mainHandle, 0x08, 0x8e);
		
			//4. compute baud rate divisor
			baud_div = 115200 / (long) baud_rate;
	
			//5. Store 0x80 in Line Control Register
				outb(COM1+3,0x80); 
				//LCR is COM1 + 3
		
			//6. store baud_div bytes in MSB and LSB
				//Make sure that bit 7 of the LCR is 1, this enables to do so!
				//COM1 + 1
				outb(COM1+1,baud_div);
			//7. Store 0x03 in LCR
				//LCR is COM1 + 3
				outb(COM1+3,0x03);
				
				
				
				
			//8. Enable right level in PIC mask reg
			 
			
			int mask2;
			
			cli();
			mask2 = inb(0x21);
			mask2 = mask2 >> 1;
			mask2 = mask2 >> 1;
			mask2 = mask2 >> 1;
			mask2 = mask2 >> 1;
			outb(0x21, mask2);
			sti();
			
				
				/*outb(0x21,);//check pg 17 of doc
				inb(0x21);
				//logic AND*/
	

			//9. Enable main serial port inter., put 0x08 in Modem reg
				outb(COM1+4,0x08);
			//10. Enable input ready inter., put 0x01 in Inter. Enable reg; see outputhandle for bit stuff (in the (), do correct level in the shift)
				outb(COM1+1,0x01);
				return 0;
		}
		else{
			return -103;//port already open
		}
	}
	else{
		if(*eflag_p==NULL){ //bad event flag pointer
			return -101;
		}
		else{ //bad baud rate div
			return -102;
		}
	}
		
	return 0;//0 if fine
}

/*! 
  Procedure..: com_close
  Description..: Will set the mask register, clear the modem, enable interrupts, and restore the interrupt vector.
*/
int com_close (void){

	if(newDCB->portflag==1){ //1. check if port open
		newDCB->portflag=0; //2. clear open
		   //3. disable in PIC mask
		int mask;
		cli();
		mask = inb(0x21);
		mask = mask & ~0x80;
		outb(0x21, mask);
		sti();
		
		//4. disable all ACC interr, zero out Modem status and Interr Enable
		int modem;
		cli();
		modem = inb(COM1+6);
		modem=modem&0x00;
		outb(COM1+6,modem);
		sti();
		
		int interenable;
		cli();
		interenable=inb(COM1+1);
		interenable=interenable&0x00;
		outb(COM1+1,interenable);
		sti();
		
		//5. restore original saved interrupt vector
		//COM1 interrupt vector number (0030h) and level 4
		//idt_set_gate(24, (u32int)oldHandler, 0x08, 0x8e);
		
			int vector;
			
			cli();
			vector = inb(COM1);
			vector = vector >> 1;
			vector = vector >> 1;
			vector = vector >> 1;
			vector = vector >> 1;
			outb(COM1, vector);
			sti();
		
		return 0;
		
	}
	else{
		return -201;//port not open
	}
	
	
	
	//return -1;  0 if fine, -201 if "serial port not open", -1 if break on us 
}

/*! 
  Procedure..: com_read
  Description..: Will open the input buffer, move characters from the ring buffer to the input buffer until the ring is empty, the count input is reached, or an ENTER is pressed, and return.
*/
int com_read (char *buf_p, int *count_p){
	if(newDCB->portflag==1){ //2. check if open
		if(buf_p!=NULL&&count_p!=NULL&&*count_p<=0){ //1. check params
			//3. initialize input buffer
			newDCB->inbuffer=(unsigned char*) buf_p;
			newDCB->incount=*count_p;
			newDCB->statuscode = "READ";
			while(i!=0){
				if(newDCB->incount==newRing->counter){//count been reached
					break;
				}
				else if(newDCB->inbuffer[newDCB->incount]==13){//CR is found
					break;
				}
				newDCB->inbuffer=newDCB->inbuffer + newRing->ring[i];
				newRing->ring[i]='\0';
				i--;
				actualcount++;
			}
			if(newDCB->incount!=actualcount){//need more characters
				return NULL;
			}
			else{
				newDCB->statuscode = "IDLE";
				*newDCB->eventflag = 1;
				return actualcount;
			}
		}
		else{ //not valid params
			if(buf_p==NULL){//bad buffer address
				return -302;
			}
			else if(count_p==NULL||*count_p>0||*count_p==NULL){//bad count address or value
				return -303;
			}
			else{//device busy
				return -304;
			}
		}
	}
	else{
		return -301; //port not open
	}
	
	
	return 0;//0 if fine
}

/*! 
  Procedure..: com_write
  Description..: Will open the output buffer, get characters from the output register, move them to the correct register, and then enable interrupts.
*/
int com_write (char *buf_p, int *count_p){
	
	if(newDCB->portflag==1){
		if(buf_p!=NULL&&count_p!=NULL&&*count_p<=0){//check params
			newDCB->outbuffer=(unsigned char*) buf_p;
			newDCB->outcount=*count_p;
			newDCB->statuscode="WRITE";
			*newDCB->eventflag=1;
			//char charIn=newDCB->inbuffer[0]; get 1st char
			
			int writebyte;
			cli();
			writebyte = inb(0x21);
			writebyte = writebyte | 0x02;
			outb(0x21, writebyte);
			sti();
			
		}
		else{
			if(buf_p==NULL){
				return -402;//invalid buffer address
			}
			else if(count_p==NULL||*count_p>0||*count_p==NULL){
				return -403;//invalid count address or value
			}
			else{
				return -404;//device busy
			}
			
		}
	}
	else{
		return -401;//port not open
	}

	return 0;//0 if fine, -401 if "serial port not open", -402 if "invalid buffer address", -403 if "invalid count address or count value", -404 if "device busy"
}



/*! 
  Procedure..: mainHandle
  Description..: Will check the incoming bits to determine if it is inputting or outputting and then redirect to the correct handler.
*/
void mainHandle(){ 
	int EOI=0x20;
	//1. if port is not open, clear interrupt, return
	int bitIn = inb(COM1 +2); //2.
	int get1st = bitIn>>1 & 1;
	int get2nd = bitIn>>2 & 1;
	if(get1st==1 && get2nd==0){
		//output interrupt
		outputHandle();//3.
	}
	else if(get1st==0 && get2nd==1){
		//input interrupt
		inputHandle();//3.
	}
	else{
		//skip
	}
	outb(0x20, EOI);//4. send eoi to pic (clears interrupt)
	
}

/*! 
  Procedure..: inputHandle
  Description..: Will read in a character from the comunication port, store the character in the ring buffer if the device is not ready for it, and then move it to the input buffer when it is and return.
*/
int inputHandle(){
 	char letterIn = (char) inb(COM1); //1. read char from input reg
 	if(strcmp(newDCB->statuscode,"READ")!=0){//2. if not reading, store in ring buffer;
 		newRing->ring[i] = letterIn;
 		newRing->ringHead=newRing->ring[headI];
 		newRing->ringTail=newRing->ring[i];
 		return NULL;
 	}
 	else{//3. else, is reading. store in input buffer of requester (COM1); 		
 			outb(letterIn, newDCB->inbuffer); 
 			if(newDCB->incount!= 0 ){//4. If the count is not completed and the character is not CR, return. Do not signal completion. ; decrement a count value like in polling, once is 0
 			//then done once hit 0
 				return NULL; //temp value, decide
 			}
  			else{//5. Otherwise, the transfer has completed. Set the status to idle. Set the event flag and return the requestor's count value. 
 				newDCB->statuscode="IDLE";
 				*newDCB->eventflag=1;
 				return newDCB->incount;
 			}
 		
 	}
}

/*! 
  Procedure..: outputHandle
  Description..: Will take in a character from the output buffer, move it to the correct register, and then return.
*/
int outputHandle(){
 /*1. If the current status is not writing, ignore the interrupt and return. 
    2. Otherwise, if the count has not been exhausted, get the next character from the requestor's output buffer and store it in the output register. Return without signaling completion. 
    3. Otherwise, all characters have been transferred. Reset the status to idle. Set the event flag and return the count value. Disable write interrupts by clearing bit 1 in the interrupt enable register.*/
	if(strcmp(newDCB->statuscode,"WRITE")){//1. If not writing status, return;
		return NULL;
	}
	else{ //2. Else if count not done, get next character from output buffer and store in out reg., return;
		if(newDCB->outcount!= 0  ){
			//char* outin=inb(outbuffer);
			//outb(newDCB->outbuffer, newRing[newRing->outputindex]);
			return NULL;
		}
		else{ //3. Else all transfered, set idle, event flag, return count, disable write interr.
			newDCB->statuscode="IDLE";
			*newDCB->eventflag= 1;
			
			//disable write interrupt by clearing bit 1 in the interrupt enable register
			int in;
			cli();
			in=inb(COM1+1);
			in=in & ~(1<<1);
			outb(COM1+1, in);
			sti();
			
			return newDCB->outcount;
		}
	}
}
 
 
 
 /*! 
  Procedure..: IOScheduler
  Description..: Will take in the system call, check to make the call is valid for the device it wants, checks the device requested to see if it can take on the request currently, and moves it to the device when it becomes available.
*/
int IOScheduler(const char* opcode, IOCB* cbIn){
	
	if(strcmp("READ", opcode)==0||strcmp("WRITE", opcode)){
			if(strcmp(cbIn->DCBTo->statuscode,"IDLE")==0){
				if(strcmp("READ", opcode)==0){
					/*if(){//device can't read
						return -502; //device not capable of this function
					}*/
					//com_read();
					return 0;
				}
				else{
					/*if(){//device can't write
						return -502; //device not capable of this function
					}*/
					//com_write();
					return 0;
				}
			}
			else{
				if(newWait->headIOCB==NULL&&newWait->tailIOCB==NULL){				
					newWait->headIOCB=cbIn;
					newWait->headIOCB->nextIOCB=NULL;
					newWait->tailIOCB=cbIn;
					newWait->tailIOCB->nextIOCB=NULL;
				}
				else if(newWait->headIOCB!=NULL && newWait->headIOCB->nextIOCB==NULL){
					newWait->headIOCB->nextIOCB=cbIn;
					newWait->tailIOCB=cbIn;
					newWait->headIOCB->nextIOCB->nextIOCB=NULL;
					newWait->tailIOCB->nextIOCB=NULL;
				}
				else{
					newWait->tailIOCB->nextIOCB=cbIn;
					newWait->tailIOCB->nextIOCB->nextIOCB=NULL;
				}
				return 0;
			}
	}
	else{
		return -501;//not READ or WRITE
	}
}

/*! 
  Procedure..: queueOut
  Description..: Will return the current IOCBQueue to the calling method to be used there.
*/
IOCBQueue* queueOut(){
	return newIOCBQueue;
}
