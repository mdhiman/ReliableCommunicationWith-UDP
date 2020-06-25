#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // Internet family of protocols
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <signal.h>
#include <assert.h>
#include <syslog.h>
#include <time.h>
#include "datastructure.h"
#include "mrp.h"
#define BUFFSIZE 1024
#define timeout 8
int recvdMsgMemID,unacknowledgedMsgMemID,status_variable_id;//shared variable;
short semaphoreArray[3];//semaphore array for 3 above mentioned shared variable
int semaphoreID;//semaphore id
int r_process_id,s_process_id;//two process child process id for r and s ..we are storing it because it needs to close on r-close
static int messageID;//message id ,it is static because it is it should be unique for every message and for that we are just incrementing
receivedMsgTable *receivedTable;// received message table which is type of receivedmessage type(user defined structure)
unacknowledgedMsgTable *unacknowledgedTable;//
int *status_variable;//shared variable for maintaining the status of unacknowledgedmsgtable whether it is empty or not after r_close() call
float failure_prob;//it is the probability of being failure to deliver a packet


int r_socket(int domain, int type, int protocol)
{
	
	int socDes;
	struct sockaddr_in	thisAddr, thatAddr;
    int dataBytes, thatAddrLen ,dataSent;
    unacknowledgedMsgType buff;
	if(type!=SOCK_MRP)
	{
		printf("socket type error....exiting..\n");
		return -1;
	}

	if((socDes = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("cannot create socket") ;
        return 0 ;
    } 

    semaphoreArray[received_sem]=1;
    semaphoreArray[unacknowledged_sem]=1;
    semaphoreArray[status_variable_sem]=1;
    messageID=0;

    failure_prob=0.2;
    semaphoreID=Create_Sem(3,semaphoreArray);
   recvdMsgMemID= shmget(IPC_PRIVATE,sizeof(receivedMsgTable),IPC_CREAT|SHM_W|SHM_R);
   unacknowledgedMsgMemID=shmget(IPC_PRIVATE,sizeof(unacknowledgedMsgTable),IPC_CREAT|SHM_W|SHM_R);
   status_variable_id=shmget(IPC_PRIVATE,sizeof(int),IPC_CREAT|SHM_W|SHM_R);
	receivedTable= shmat(recvdMsgMemID,NULL,0);

	unacknowledgedTable=shmat(unacknowledgedMsgMemID,NULL,0);

	status_variable=shmat(status_variable_id,NULL,0);
	Lock_Semaphore(semaphoreID,received_sem);
	Lock_Semaphore(semaphoreID,unacknowledged_sem);
	receivedTable->front=-1;
	receivedTable->rear=0;
	unacknowledgedTable->index=0;
	Unlock_Semaphore(semaphoreID,received_sem);
	Unlock_Semaphore(semaphoreID,unacknowledged_sem);
	Lock_Semaphore(semaphoreID,status_variable_sem);
	*status_variable=1;
	Unlock_Semaphore(semaphoreID,status_variable_sem);
   

if((fork())==0)
{
	int dataBytes;
	r_process_id=getpid();
	receivedTable= shmat(recvdMsgMemID,NULL,0);
	unacknowledgedTable=shmat(unacknowledgedMsgMemID,NULL,0);
	
	while(1)
	{
		if(dataBytes=recvfrom(socDes,&buff,sizeof(buff),0, (struct sockaddr *)&thatAddr, &thatAddrLen)>= 0)
	{
			/*printf("data types ... received......%d\n",buff.type );
			printf("message from client ..%c\n",buff.message );
			//printf("%d\n",buff.type );
			printf("message id is...%d\n",buff.message_id );
			printf("message id is %ld\n",buff.lasttrytime );*/
			int dropMessageVal=dropMessage(failure_prob);
			//printf("dropMessage  value is...%f\n" ,dropMessageVal);
			if(dropMessageVal==0)
			{
		receivedMsgType temp;
		if(buff.type==data)
		{

		
		temp.message_id=buff.message_id;
		temp.type=buff.type;
		temp.address=buff.address;
		temp.message=buff.message;
		//printf("message....from test..%c\n",temp.message );
		 Lock_Semaphore(semaphoreID,received_sem);
		 receivedTable->receivedTableArray[receivedTable->rear]=temp;
		 //receivedTable->front=receivedTable->front+1;
		 receivedTable->rear=receivedTable->rear+1;
		 Unlock_Semaphore(semaphoreID,received_sem);
		 buff.type=acknowledgement;
	
		sendto(socDes,&buff,sizeof(buff),0,(struct sockaddr *)&thatAddr,sizeof(thatAddr));
		// send_acknowledgement(buff);
		}
		if(buff.type==acknowledgement)
		{

			int receivedid=buff.message_id,i;
				Lock_Semaphore(semaphoreID,unacknowledged_sem);
				for(i=0;i<=unacknowledgedTable->index;i++)
				{
					if(unacknowledgedTable->unacknowledgedTableArray[i].message_id==receivedid)
					{
						unacknowledgedTable->unacknowledgedTableArray[i].status=1;
						
						//printf("successfully   acknowledged..... one entry %d\n",unacknowledgedTable->unacknowledgedTableArray[i].message_id);
					}
				}
				Unlock_Semaphore(semaphoreID,unacknowledged_sem);
			}
		}
	}
	else
	{
		printf("can not receive error......\n");
	}

	}
	
	exit(0);
}

if((fork())==0)
{
	s_process_id=getpid();
	while(1)
	{
	sleep(timeout);
	receivedTable= shmat(recvdMsgMemID,NULL,0);
	unacknowledgedTable=shmat(unacknowledgedMsgMemID,NULL,0);
	status_variable=shmat(status_variable_id,NULL,0);
	int timediff,i;

	

				for(i=0;i<=unacknowledgedTable->index;i++)
				{
					//printf("from acknowledgement........\n");
					if(unacknowledgedTable->unacknowledgedTableArray[i].status==0)
					{
						//unacknowledgedTable->unacknowledgedTableArray[i].status=1;
						timediff=(int)difftime(time(0),unacknowledgedTable->unacknowledgedTableArray[i].lasttrytime);
						if(timediff>=timeout)
						{
							//printf("timeout occur..... one entry\n");
							unacknowledgedMsgType temp=unacknowledgedTable->unacknowledgedTableArray[i];
							sendto(temp.socketid,&temp,sizeof(temp),0,&temp.address,sizeof(temp.address));
						}
						
						
					}
				}
				printf("status  value....%d\n",*status_variable );
	}
	
	exit(0);
}
return socDes;

}

int r_bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen)
{
 if((bind(sockfd, (struct sockaddr *)addr, addrlen)) < 0) 
 	{
        perror("cannot bind") ;
        return -1 ;
    }
    return 0;

}

ssize_t r_sendto(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen)
{
	
    receivedTable= shmat(recvdMsgMemID,NULL,0);
	unacknowledgedTable=shmat(unacknowledgedMsgMemID,NULL,0);
	unacknowledgedMsgType temp;
	temp.message_id=++messageID;
	temp.message=*(char *)buf;
	temp.lasttrytime=time(0);
	temp.status=0;
	temp.type=data;
	temp.socketid=sockfd;
	temp.address=*dest_addr;
	Lock_Semaphore(semaphoreID,unacknowledged_sem);
	/*receivedTable->front++;
	receivedTable->receivedTableArray[receivedTable->rear]=temp;
	receivedTable->rear++*/
	unacknowledgedTable->unacknowledgedTableArray[unacknowledgedTable->index]=temp;
	unacknowledgedTable->index=(unacknowledgedTable->index+1)%TABLESIZE;

	Unlock_Semaphore(semaphoreID,unacknowledged_sem);

	//printf(" %d\n", temp.type );

	return (sendto(sockfd,&temp,sizeof(temp),flags,dest_addr,addrlen));

	
}



ssize_t r_recvfrom(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen)
{
	receivedTable= shmat(recvdMsgMemID,NULL,0);
	unacknowledgedTable=shmat(unacknowledgedMsgMemID,NULL,0);
	//status_variable=shmat(status_variable_id,NULL,0);
	while(1)
	{
		if(!(receivedTable->rear==0 ||(receivedTable->rear-1==receivedTable->front)))
		{
			Lock_Semaphore(semaphoreID,received_sem);
			if(receivedTable->receivedTableArray[++receivedTable->front].type==data)
			{
				//*(char*)buf=receivedTable->receivedTableArray[receivedTable->front].message;
			printf("%c\n",receivedTable->receivedTableArray[receivedTable->front].message );
			//receivedTable->receivedTableArray[receivedTable->front].reading_status=1;
			
			Unlock_Semaphore(semaphoreID,received_sem);
			break;
			}

			
		}
		else
		{
			printf("no message yet.....%d\n",*status_variable);
			Lock_Semaphore(semaphoreID,status_variable_sem);
			if(*status_variable==2)
			{
				//Unlock_Semaphore(semaphoreID,status_variable_sem);
				if(isUMTEmptyAndAllRecvdRead())
				{
					killAndDeleteEverything(sockfd);

				}
			}
			Unlock_Semaphore(semaphoreID,status_variable_sem);
			sleep(4);
		}
	}

	return sizeof(buf);

}
int r_close(int fd)
{
	
		//status_variable=shmat(status_variable_id,NULL,0);
		if(isUMTEmptyAndAllRecvdRead())
		{
			killAndDeleteEverything(fd);
			return 0;
			
		}
		else
		{
			printf("setting status variable....%d\n",*status_variable);
			Lock_Semaphore(semaphoreID,status_variable_sem);
			*status_variable=2;
			Unlock_Semaphore(semaphoreID,status_variable_sem);
			printf("value is ...%d\n",*status_variable );
			return 0;
		}
	
	
	return 0;//on success

}


int dropMessage(float p)
{
	float f=(float)rand()/(float)RAND_MAX;
	
	if(f<p)
		return 1;
	else
		return 0;
}

int isUMTEmptyAndAllRecvdRead()
{
	int flag1=1,flag2=1;
	receivedTable= shmat(recvdMsgMemID,NULL,0);
	unacknowledgedTable=shmat(unacknowledgedMsgMemID,NULL,0);
	//status_variable=shmat(status_variable_id,NULL,0);
	int i;
	if(!(receivedTable->rear==0 ||(receivedTable->rear-1==receivedTable->front)))
	{
		printf("received table is not empty.....\n");
		flag1=0;
	}

	for(i=0;i<unacknowledgedTable->index;i++)
	{
		if(unacknowledgedTable->unacknowledgedTableArray[i].status==0)
		{
			printf("unacknowledgedTable is not empty.......\n");
			flag2=0;
			break;
		}
	}

	if(flag1&&flag2)
		return 1;
	else
		return 0;
}
void killAndDeleteEverything(int fd)
{
			printf("killing all process.....\n");
			shmctl(recvdMsgMemID,IPC_RMID,NULL);
			shmctl(unacknowledgedMsgMemID,IPC_RMID,NULL);
			shmctl(status_variable_id,IPC_RMID,NULL);
			Delete_Semaphore(semaphoreID);
			close(fd);
			//kill(r_process_id,SIGTERM);
			//kill(s_process_id,SIGTERM);
			system("killall server.out");
			system("killall client.out");

			return;
}
