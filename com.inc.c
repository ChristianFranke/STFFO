#include "semph.c"

static void delete (void) {
   int res;
   printf ("\nServer wird beendet - Lösche Semaphor %d.\n", sem_delete);
   if (semctl (sem_delete, 0, IPC_RMID, 0) == -1) {
   	   printf ("Fehler beim Löschen des Semaphors.\n");
   }
   /* Delete Seg when exit process*/
   res = shmctl (shm_delete, IPC_RMID, NULL);
   if (res == -1)
      printf ("Fehler bei shmctl() shmid %d, Kommando %d\n",
          shm_delete, IPC_RMID);   
   return;
}

int com(int sd, struct sockaddr_in sin, int *sin_len) {
	int sem_set_id; //ID of sem set
	int rc;
	union semun sunion{
		int val;
		struct semid_ds *buf;
		ushort * array;
	} sem_val;
	
	/*create a sem set with ID 250, with one sem, only owner can access*/
	sem_set_id = semget(SEM_ID, 1, IPC_CREAT | 0600);
    if (sem_set_id == -1) {
    	perror("main: semget");
    	exit(1);
    	}
    	
    /*Initialize sem*/
	sem_val.val = 1;
	rc = semctl(sem_set_id, 0, SETVAL, sem_val);
	if (rc == -1) {
		perror("main: semctl");
		exit(1);
	}
	sem_delete = sem_set_id;
	atexit (&delete);
	
    int n_sd;
    
    // for the command the client is typing in
    char inputCommand[30];
    
    char delimiter[] = " ";
    char *exec;
    
    FILE *sockstream;
    
    while(true) {
        n_sd = accept(sd, &sin, sin_len);
        int pid;
        
        if ((pid = fork()) == -1) {
	        // error (out of memory etc.)
	        
            close(n_sd);
            continue;
        } else if(pid > 0) {
	        // parentprocess
	        
            close(n_sd);
            continue;
        } else if (pid == 0) {
        	// childprocess
        	
	        printf("Debug: Neuer Client per telnet.\n");
	        
	        // client open readable
	        sockstream = fdopen(n_sd, "r+");
	        fprintf(sockstream, "%s", "Willkommen auf dem Datenserver. Bitte nutze GET, PUT oder DEL Kommandos.\n\n");
	        rewind(sockstream);
	        
	        while(true) {
	            sprintf(inputCommand, "");
	            
	            // regex get command
	            fscanf(sockstream, "%50[0-9a-zA-Z ]", inputCommand);
	            rewind(sockstream);
	            
	            printf("Debug: %s eingetippt.\n", inputCommand);
	            
	            // if nothing is typed in (stringlength == 0)
	            if (strlen(inputCommand) == 0) {
	                continue;
	            }
	            
	            // split inputCommand at <delimiter> and return first part
	            exec = strtok(inputCommand, delimiter);
	            
	            printf("Debug: Kommando: %s.\n", exec);
	            
	            // if first part contains get, then ...; if put, then ...
	            if (strcmp(exec, "GET") == 0) {
	                exec = strtok(NULL, delimiter);
	                get(exec, "?", sockstream);
	            } else if (strcmp(exec, "PUT") == 0) {
	                exec = strtok(NULL, delimiter);
	                put(exec, strtok(NULL, delimiter), "?", sockstream);
	            } else if (strcmp(exec, "DEL") == 0) {
	                exec = strtok(NULL, delimiter);
	                del(exec, "?", sockstream);
	            } else if (strcmp(exec, "TARGET") == 0) {
	                exec = strtok(NULL, delimiter);
	                setTarget(exec, "?", sockstream);
	            } else if (strcmp(exec, "EXIT") == 0) {
	                printf("%s", "Debug: Client abgemeldet.\n");
	                
	                fprintf(sockstream, "%s", "Danke und tschö!\n");
	                rewind(sockstream);
	                
	                break;
	            } else {
		            fprintf(sockstream, "%s", "Unbekanntes Kommando!\n");
	                rewind(sockstream);
	            }
	        }
	        
	        close(n_sd);
	    }
    }
    
    return EXIT_SUCCESS;
}