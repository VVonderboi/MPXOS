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

void infiniteProcess();
void infinite();
void defaultProcesses();

void defaultProcesses(){
	PCB* compPCB = SetupPCB("comhand", 1 , 9);
	context* comcon = (context* )(compPCB->top);
	memset (comcon, 0, sizeof(context));
	comcon->fs = 0x10 ;
	comcon->gs = 0x10 ;
	comcon->ds = 0x10 ;
	comcon->es = 0x10 ;
	comcon->cs = 0x8 ;
	comcon->ebp = (u32int)(compPCB->stack);
	comcon->esp = (u32int)(compPCB->top);
	comcon->eip = (u32int)  comhand; // The function correlating to the process , ie. Proc1
	comcon->eflags = 0x202 ; 
	compPCB->suspended=0;
	insertPCB(compPCB);
	
	
	PCB* idlePCB = SetupPCB("idle", 1 , 0);
	context* idlecon = (context* )(idlePCB->top);
	memset (idlecon, 0, sizeof(context));
	idlecon->fs = 0x10 ;
	idlecon->gs = 0x10 ;
	idlecon->ds = 0x10 ;
	idlecon->es = 0x10 ;
	idlecon->cs = 0x8 ;
	idlecon->ebp = (u32int)(idlePCB->stack);
	idlecon->esp = (u32int)(idlePCB->top);
	idlecon->eip = (u32int)  idle; // The function correlating to the process , ie. Proc1
	idlecon->eflags = 0x202 ; 
	idlePCB->suspended=0;
	insertPCB(idlePCB);
	
	
}

void infinite(){
	char msg[30];
  	int count=0;
	
	memset( msg, '\0', sizeof(msg));
	strcpy(msg, "INFINITE PROCESS\n");
	count = strlen(msg);
  
  while(1){
	sys_req( WRITE, DEFAULT_DEVICE, msg, &count);
    	sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);
  }

}

void infiniteProcess(){
	
	PCB* inPCB = SetupPCB("infinite", 0 , 8);
	context* incon = (context* )(inPCB->top);
	memset (incon, 0, sizeof(context));
	incon->fs = 0x10 ;
	incon->gs = 0x10 ;
	incon->ds = 0x10 ;
	incon->es = 0x10 ;
	incon->cs = 0x8 ;
	incon->ebp = (u32int)(inPCB->stack);
	incon->esp = (u32int)(inPCB->top);
	incon->eip = (u32int)  infinite; // The function correlating to the process , ie. Proc1
	incon->eflags = 0x202 ; 
	inPCB->suspended=0;
	insertPCB(inPCB);

}
