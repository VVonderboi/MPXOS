/** @file */

typedef struct alarms{
	//int temptime; //look at time.c to get setup
	int alarmHour;
	int alarmMinute;
	int alarmSecond;
	char message[50];
	struct alarms* nextAlarm;
} alarms;

typedef struct alarmQueue
{
	struct alarms* head;
	struct alarms* tail;
	int count;
	
} alarmQueue;
void InitAlarmQueue();
alarms* alarm(char* messageIn, char* time);
void removeAlarm(alarms* removeThisAlarm);
void insertAlarm(alarms* alarmIn);
int alarmChecker();
int checkAlarmQueue();
alarmQueue* GetAlarmQ();
void alarmTest();
void showAlarms();
