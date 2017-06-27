// function to return local filname by key
char * getFilename(char* filename) {
    char* response = malloc(50);
    
    sprintf(response, "db/%s.txt", filename);
    
    return response;
}

// function to return string by key
int get(char* key, char* res, FILE *sockstream) {
    printf("Debug: Hole Datensatz \"%s\".\n", key);
    
    if (currentTarget == RAM) {
	    for (int i=0; i<sizeof(keys) / sizeof(keys[0]); i++) {
	        if (strncmp(keys[i].key, key, sizeof(keys[i].key)) == 0) {
	            strcpy(res, keys[i].value);
	            
	            fprintf(sockstream, "%s\n", keys[i].value);
	            
	            return EXIT_SUCCESS;
	        }
	    }
    } else {
	    char *filename = getFilename(key);
	    printf("Debug: Dateiname zum Lesen: %s.\n", filename);
	    
	    int c;
	    FILE *file;
	    
	    if (access(filename, F_OK) != -1) {
	        // file exists
	        
	        printf("Debug: Datei \"%s\" existiert.\n", filename);
	        
	        file = fopen(filename, "r");
	        if (file) {
	            while ((c = getc(file)) != EOF) {
	                putchar(c);
	                
	                fprintf(sockstream, "%c", c);
	                rewind(sockstream);
	            }
	            
	            fclose(file);
	            
	            fprintf(sockstream, "%s", "\n");
	        } else {
	            printf("Debug: Konnte Datei \"%s\" nicht öffnen.\n", filename);
	        }
	    } else {
	        printf("Debug: Datei \"%s\" existiert nicht.\n", filename);
	        
	        fprintf(sockstream, "%s existiert nicht.\n", key);
	        rewind(sockstream);
	    }
    }
    
    
    return EXIT_SUCCESS;
}

// function to write value by key
int put(char* key, char* value, char* res, FILE *sockstream) {
    printf("Debug: Speichere Datensatz \"%s\" mit Inhalt \"%s\".\n", key, value);
    
    if (currentTarget == RAM) {	    
	    for (int i=0; i<sizeof(keys) / sizeof(keys[0]); i++) {		    
	        if (strlen(keys[i].key) == 0) {
	            strncpy(keys[i].key, key, sizeof(keys[i].key));
	            strncpy(keys[i].value, value, sizeof(keys[i].value));
	            strncpy(res, value, sizeof(res));
	            
	            fprintf(sockstream, "%s geschrieben.\n", key);
	            
	            return EXIT_SUCCESS;
	        }
	    }
    } else {
	    char *filename = getFilename(key);
	    printf("Debug: Dateiname zum Speichern: %s.\n", filename);
	    
	    int c;
	    FILE *file;
	    
	    file = fopen(filename, "w");
	        
	    if (file == NULL) {
	        printf("Debug: Konnte Datei \"%s\" nicht öffnen.\n", filename);
	    } else {
	        fprintf(file, "%s", value);
	        fclose(file);
	        
	        fprintf(sockstream, "%s geschrieben.\n", key);
	    }
    }
        
    return EXIT_SUCCESS;
}

// function to delete by key
int del(char* key, char* res, FILE *sockstream) {
    printf("Debug: Lösche Datensatz \"%s\".\n", key);
    
    if (currentTarget == RAM) {
	    for (int i=0; i<sizeof(keys) / sizeof(keys[0]); i++) {
	        if (strncmp(keys[i].key, key, sizeof(keys[i].key)) == 0) {
	            strncpy(res, keys[i].value, sizeof(keys[i].value));
	            memset(&keys[i], 0, sizeof(keys[i]));
	            
	            fprintf(sockstream, "%s gelöscht.\n", key);
	            
	            return EXIT_SUCCESS;
	        }
	    }
	    
	    fprintf(sockstream, "\"%s\" existiert nicht.\n\n", key);
	    printf("Debug: \"%s\" existiert nicht.\n", key);
	} else {
		char *filename = getFilename(key);
	    printf("Debug: Dateiname zum Löschen: %s.\n", filename);
	    
	    if (access(filename, F_OK) != -1) {
	        // file exists
	        
	        printf("Debug: Datei \"%s\" existiert.\n", filename);
	        
	        unlink(filename);
	        
	        fprintf(sockstream, "%s gelöscht.\n", key);
	    } else {
	        printf("Debug: Datei \"%s\" existiert nicht.\n", filename);
	    }
	}
    
    return EXIT_SUCCESS;
}

int setTarget(char* key, char* res, FILE *sockstream) {
    printf("Debug: setze Speicherziel auf \"%s\".\n", key);
    
    if (strcmp(key, "HDD") == 0) {
    	// save on HDD
    	
    	currentTarget = HDD;
    } else if (strcmp(key, "RAM") == 0) {
		// save in RAM
		
		currentTarget = RAM;
	} else {
		printf("Debug: Speicherziel \"%s\" nicht gefunden.\n", key);
		fprintf(sockstream, "Speicherziel \"%s\" nicht gefunden.\n", key);
		
		return false;
	}
    
    fprintf(sockstream, "Auf Speicherziel \"%s\" geändert.\n", key);
    
    return EXIT_SUCCESS;
}