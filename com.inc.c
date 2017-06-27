int com(int sd, struct sockaddr_in sin, int *sin_len) {
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
	        
	        // semaphores
			enterRead.sem_num = leaveRead.sem_num = 0;
			enterRead.sem_flg = leaveRead.sem_flg = SEM_UNDO;
			enterRead.sem_op = -1;
			leaveRead.sem_op = 1;

			enterWrite.sem_num = leaveWrite.sem_num = 1;
			enterWrite.sem_flg = leaveWrite.sem_flg = SEM_UNDO;
			enterWrite.sem_op = -1;
			leaveWrite.sem_op = 1;
			
	        
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
	            
	            char res[256];
	            
	            // if first part contains get, then ...; if put, then ...
	            if (strcmp(exec, "GET") == 0) {
	                exec = strtok(NULL, delimiter);
	                get(exec, res, sockstream);
	            } else if (strcmp(exec, "PUT") == 0) {
	                exec = strtok(NULL, delimiter);
	                put(exec, strtok(NULL, delimiter), res, sockstream);
	            } else if (strcmp(exec, "DEL") == 0) {
	                exec = strtok(NULL, delimiter);
	                del(exec, res, sockstream);
	            } else if (strcmp(exec, "TARGET") == 0) {
	                exec = strtok(NULL, delimiter);
	                setTarget(exec, res, sockstream);
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
	        semctl(sem_id, 0, IPC_RMID);
	    }
    }
    
    return EXIT_SUCCESS;
}