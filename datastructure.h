#include <stdlib.h>
#include <stdio.h>
#define SOCK_MRP 1000
#define TABLESIZE 100

typedef struct
{
	int message_id;
	char message;
	time_t lasttrytime;
	int status;
	int socketid;
	struct sockaddr address;
	int type;
	
} unacknowledgedMsgType;


typedef struct 
{
	int message_id;
	int type;
	struct sockaddr address;
	char message;

} receivedMsgType;
typedef struct 
{
	receivedMsgType receivedTableArray[TABLESIZE];
	int front;
	int rear;

} receivedMsgTable;
typedef struct 
{
	unacknowledgedMsgType unacknowledgedTableArray[TABLESIZE];
	int index;
} unacknowledgedMsgTable;

enum
{
	received_sem,
	unacknowledged_sem,
	status_variable_sem
};
enum
{
	data,
	acknowledgement
};
 
