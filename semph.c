#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define MAXCOUNT 1000000
#define NUM_OF_CHILDS 4
#define SEGSIZE sizeof(int)

static void sem_lock(int sem_set_id);
static void sem_unlock(int sem_set_id);
static int safesemget (key_t key, int nsems, int sem_flg);
static int safesemctl (int sem_set_id, int semnum, 
                       int cmd, union semun arg);
static int safesemop  (int sem_set_id, struct sembuf *sops,
                       unsigned nsops);
static int sem_delete = 0;
static int shm_delete = 0;

void sem_lock(int sem_set_id) {
		struct sembuf sem_op;  //Structure for sem operations
		
		sem_op.sem_num = 0;
		sem_op.sem_op = -1;
		sem_op.sem_flg = SEM_UNDO;
		semop(sem_set_id, &sem_op, 1);
	
		return;
	}
	
void sem_unlock(int sem_set_id) {
	struct sembuf sem_op;
	
	sem_op.sem_num = 0;
	sem_op.sem_op = 1;
	sem_op.sem_flg = SEM_UNDO;
	semop(sem_set_id, &sem_op, 1);
	}

void fork_child(int sem_set_id, struct hastable_s *hashtable_t) {
	
}
/*int main() {
	int i, shmid;
	key_t key;
	char *shar_mem, *s;
	
	key = 9876;
	
	int pid[NUM_OF_CHILDS];
	
	//Get Seg of Shar Mem, to process and set to 0
	
	shmid = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT | 0666);
	if(shmid < 0) {
		perror("main : shmget");
		exit(1);
	}
	
	shm_addr = shmat(shm_id, NULL, 0);
	if(shar_mem == (char *) -1) {
			perror("shmat");
			exit(1);
		}
		
	s = shar_mem;
	s += SEGSIZE;
	
	*s = 0;
	
	while(*shar_mem != '*')
		sleep (1);
	
	return 0;
}*/
