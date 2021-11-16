/** @file */

typedef struct CMCB{
	int type; //0 is free, 1 is allocated
	int beginningAddress;
	int size; //in bytes
	//char* name;
	struct CMCB* nextCMCB;
	struct CMCB* prevCMCB;
} CMCB;

typedef struct memlist{
	struct CMCB* head;
	struct CMCB* tail;
} memlist;


///The data structure we use is a doubly linked list that contains complete memory control blocks that make up the heap.\n
///We only use one doubly linked list to represent the allocated and freed CMCBs.\n
///When memory is  allocated a new allocated CMCB is created and becomes the old free blocks next CMCB. \n

///LMCBs are not used. \n

///\b Merging \b Procedure  \n
///Check the current CMCB that is going to be freed next to see if the CMCB is free. If the CMCB is free then merge.\n
///Check the current CMCB that is going to be freed previous to see if the next CMCB is free if so merge.\n
///In conclusion, check down if CMCB is free then merge, then check up if CMCB is free then merge also.\n

int initializeHeap(int size);
CMCB* setupC(int typeIn, int sizeIn,  int addressIn);
//int* allocateMem(int byte);
void *allocateMem(u32int size);
int freeMem(void *ptr);
void showAllocated();
void showFreed();
void showAll();
int isEmpty();
