// function to return local filname by key
char * getFilename(char* filename) {
    char* response = malloc(50);
    
    sprintf(response, "db/%s.txt", filename);
    
    return response;
}

// function to return string by key
int get(char* key, char* res, FILE *sockstream) {
    printf("Debug: Hole Datensatz \"%s\".\n", key);
    
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
    
    return EXIT_SUCCESS;
}

// function to write value by key
int put(char* key, char* value, char* res, FILE *sockstream) {
    printf("Debug: Speichere Datensatz \"%s\" mit Inhalt \"%s\".\n", key, value);
    
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
        
    return EXIT_SUCCESS;
}

// function to delete by key
int del(char* key, char* res, FILE *sockstream) {
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
    
    return EXIT_SUCCESS;
}

int setTarget(char* key, char* res, FILE *sockstream) {
    printf("Debug: setze Speicherziel auf \"%s\".\n", key);
    
    fprintf(sockstream, "Auf Speicherziel \"%s\" geändert.\n", key);
    
    return EXIT_SUCCESS;
}