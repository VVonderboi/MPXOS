/** @file */
typedef struct PCB{
	char processName[20];
	int processClass;
	int priority;
	int state;
	int suspended;
	
	
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
/*typedef struct queue{
	int count;
	struct PCB* head;
	struct PCB* tail;
} queue;*/
int test();
void insertPCB(PCB *PCBIn);
PCB* AllocatePCB();
int FreePCB(PCB* freeThisPCB);
PCB* SetupPCB(char *name, int class, int priority);
PCB* FindPCB(char *name);
int RemovePCB(PCB* removeThisPCB);
void InitPCB();
queue* GetReadyQ();
queue* GetBlockedQ();
int BlockPCB(char *name);
int UnblockPCB(char *name);
void endAll();
