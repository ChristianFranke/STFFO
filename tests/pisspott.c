#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sem.h>
 
int main() {
 
 
#ifdef USESEM
 
    int sem_id = semget (IPC_PRIVATE, 2, IPC_CREAT | 0644);
 
    struct sembuf k1up, k1down, k2up, k2down; // Structs für den Semaphor
 
    k1up.sem_num = 0;
    k1up.sem_flg = SEM_UNDO;
    k1up.sem_op = 1;
 
    k1down.sem_num = 0;
    k1down.sem_flg = SEM_UNDO;
    k1down.sem_op = -1;
 
    k2up.sem_num = 1;
    k2up.sem_flg = SEM_UNDO;
    k2up.sem_op = 1;
 
    k2down.sem_num = 1;
    k2down.sem_flg = SEM_UNDO;
    k2down.sem_op = -1;
 
 
    unsigned short marker[2];
    marker[0] = 1; // erste Semaphore mit 1 initialisieren
    marker[1] = 0; // zweite Semaphore mit 0 initialisieren
    semctl(sem_id, 2, SETALL, marker);
     
#endif
 
    int k1 = fork();
 
    if (k1 == 0) { // in Kind 1
        for (int i=0; i<10; i++) {
 
#ifdef USESEM
            //k1.down
            semop(sem_id, &k1down, 1);
#endif
            printf("piss \n");
            sleep(3);
#ifdef USESEM
            //k2.up
            semop(sem_id, &k2up, 1);
#endif
        }
        exit(0);
    }
 
    int k2 = fork();
 
    if (k2 == 0) { // in Kind 2
        for (int i=0; i<10; i++) {
 
#ifdef USESEM
            //k2.down
            semop(sem_id, &k2down, 1);
#endif
 
            printf("pott \n");
            sleep(1);
 
#ifdef USESEM
            //k1.up
            semop(sem_id, &k1up, 1);
#endif
        }
        exit(0);
    }
 
 
 
    waitpid(k1,0,0);
    waitpid(k2,0,0);
 
#ifdef USESEM
    semctl(sem_id, 0, IPC_RMID);
#endif
 
    printf("fin de job!\n");
    return 0;
}