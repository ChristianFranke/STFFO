// function to return local filname by key
char * getFilename(char* filename) {
    char* response = malloc(50);
    
    sprintf(response, "db/%s.txt", filename);
    
    return response;
}

void* createSharedMemory(size_t size) {
	// open memory read / write
	int protection = PROT_READ | PROT_WRITE;
	
	int visibility = MAP_ANONYMOUS | MAP_SHARED;
	
	return mmap(NULL, size, protection, visibility, 0, 0);
}

struct entry_s {
	char *key;
	char *value;
	struct entry_s *next;
};

typedef struct entry_s entry_t;

struct hashtable_s {
	int size;
	struct entry_s **table;	
};

typedef struct hashtable_s hashtable_t;


/* Create a new hashtable. */
hashtable_t *ht_create( int size ) {

	hashtable_t *hashtable = NULL;
	int i;

	if( size < 1 ) return NULL;

	/* Allocate the table itself. */
	if( ( hashtable = malloc( sizeof( hashtable_t ) ) ) == NULL ) {
		return NULL;
	}

	/* Allocate pointers to the head nodes. */
	if( ( hashtable->table = malloc( sizeof( entry_t * ) * size ) ) == NULL ) {
		return NULL;
	}
	for( i = 0; i < size; i++ ) {
		hashtable->table[i] = NULL;
	}

	hashtable->size = size;

	return hashtable;	
}

/* Hash a string for a particular hash table. */
int ht_hash( hashtable_t *hashtable, char *key ) {

	unsigned long int hashval;
	int i = 0;

	/* Convert our string to an integer */
	while( hashval < ULONG_MAX && i < strlen( key ) ) {
		hashval = hashval << 8;
		hashval += key[ i ];
		i++;
	}

	return hashval % hashtable->size;
}

/* Create a key-value pair. */
entry_t *ht_newpair( char *key, char *value ) {
	entry_t *newpair;

	if( ( newpair = malloc( sizeof( entry_t ) ) ) == NULL ) {
		return NULL;
	}

	if( ( newpair->key = strdup( key ) ) == NULL ) {
		return NULL;
	}

	if( ( newpair->value = strdup( value ) ) == NULL ) {
		return NULL;
	}

	newpair->next = NULL;

	return newpair;
}

/* Insert a key-value pair into a hash table. */
void ht_set( hashtable_t *hashtable, char *key, char *value ) {
	int bin = 0;
	entry_t *newpair = NULL;
	entry_t *next = NULL;
	entry_t *last = NULL;

	bin = ht_hash( hashtable, key );

	next = hashtable->table[ bin ];

	while( next != NULL && next->key != NULL && strcmp( key, next->key ) > 0 ) {
		last = next;
		next = next->next;
	}

	/* There's already a pair.  Let's replace that string. */
	if( next != NULL && next->key != NULL && strcmp( key, next->key ) == 0 ) {

		free( next->value );
		next->value = strdup( value );

	/* Nope, could't find it.  Time to grow a pair. */
	} else {
		newpair = ht_newpair( key, value );

		/* We're at the start of the linked list in this bin. */
		if( next == hashtable->table[ bin ] ) {
			newpair->next = next;
			hashtable->table[ bin ] = newpair;
	
		/* We're at the end of the linked list in this bin. */
		} else if ( next == NULL ) {
			last->next = newpair;
	
		/* We're in the middle of the list. */
		} else  {
			newpair->next = next;
			last->next = newpair;
		}
	}
}

/* Retrieve a key-value pair from a hash table. */
char *ht_get( hashtable_t *hashtable, char *key ) {
	int bin = 0;
	entry_t *pair;

	bin = ht_hash( hashtable, key );

	/* Step through the bin, looking for our value. */
	pair = hashtable->table[ bin ];
	while( pair != NULL && pair->key != NULL && strcmp( key, pair->key ) > 0 ) {
		pair = pair->next;
	}

	/* Did we actually find anything? */
	if( pair == NULL || pair->key == NULL || strcmp( key, pair->key ) != 0 ) {
		return NULL;

	} else {
		return pair->value;
	}
	
}

// function to return string by key
int get(char* key, char* res, FILE *sockstream) {
	
	semop(sem_set_id, &sem_lock, 1);
    
	printf("Debug: Hole Datensatz \"%s\".\n", key);
    
    if (currentTarget == ROM) {
		hashtable_t *hashtable = ht_create(65536);
		
		void* shmem = createSharedMemory(ht_hash(hashtable, key));
	    
	    fprintf(sockstream, "%s geschrieben.\n", shmem);
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
    semop(sem_set_id, &sem_unlock, 1);
    
    return EXIT_SUCCESS;
}

// function to write value by key
int put(char* key, char* value, char* res, FILE *sockstream) {
    
	semop(sem_set_id, &sem_lock, 1);
    
	printf("Debug: Speichere Datensatz \"%s\" mit Inhalt \"%s\".\n", key, value);
    
    if (currentTarget == ROM) {
		hashtable_t *hashtable = ht_create(65536);
		
		void* shmem = createSharedMemory(ht_hash(hashtable, key));
		memcpy(shmem, value, sizeof(value));
	    
	    fprintf(sockstream, "%s geschrieben.\n", key);
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
    semop(sem_set_id, &sem_unlock, 1);
    
    return EXIT_SUCCESS;
}

// function to delete by key
int del(char* key, char* res, FILE *sockstream) {
	
	semop(sem_set_id, &sem_lock, 1);
    
	printf("Debug: Lösche Datensatz \"%s\".\n", key);
    
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
    
    semop(sem_set_id, &sem_unlock, 1);
    
    return EXIT_SUCCESS;
}

int setTarget(char* key, char* res, FILE *sockstream) {
	
	semop(sem_set_id, &sem_lock, 1);
    
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
    
    semop(sem_set_id, &sem_unlock, 1);
    
    return EXIT_SUCCESS;
}