/** @file */


typedef struct DCB{
	int portflag; //determines if port open or closed; 0 for closed, 1 for open
	int* eventflag; //"this is a far pointer to an integer event flag."; also need an event flag int? or just ptr?
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

int com_open (int *eflag_p, int baud_rate);
int com_close (void);
int com_read (char *buf_p, int *count_p);
int com_write (char *buf_p, int *count_p);
void mainHandle();
int inputHandle();
int outputHandle();
int IOScheduler(char* opcode, IOCB* cbIn);
IOCBQueue* queueOut();
