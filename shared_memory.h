#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <signal.h>
#include <assert.h>
#include <syslog.h>

#if !defined(__GNU_LIBRARY__) || defined(_SEM_SEMUN_UNDEFINED)
union semun
{
int val; // value for SETVAL
struct semid_ds* buf; // buffer for IPC_STAT, IPC_SET
unsigned short* array; // array for GETALL, SETALL
struct seminfo* __buf; // buffer for IPC_INFO
};
#endif

void Lock_Semaphore(int semid,int val)
{
	struct sembuf s;
	s.sem_num=val;
	s.sem_op=-1;
	s.sem_flg=SEM_UNDO;
	semop(semid,&s,1);
	return;

}
void Unlock_Semaphore(int semid,int val)
{
	struct sembuf s;
	s.sem_num=val;
	s.sem_op=1;
	s.sem_flg=SEM_UNDO;
	semop(semid,&s,1);
	return;
}
int Create_Sem(int no,short *valArray)
{
	int semid;
	union semun var;
	semid=semget(IPC_PRIVATE,no,SHM_R|SHM_W);
	var.array=valArray;
	semctl(semid,0,SETALL,var);
	return semid;

}
void Delete_Semaphore(int semid)
{
	if (semctl(semid, 0, IPC_RMID, NULL) == -1)
    {
    perror("Error in releasing semaphore!");
    exit(EXIT_FAILURE);
    }

}
