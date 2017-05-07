/**
 *  ######################################################################
 *  # STFFO                                                              #
 *  ######################################################################
 *
 *  Simple Telnetserver for Filesystem Operations
 *  
 *  @package STFFO
 *  @git https://github.com/ChristianFranke/STFFO
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>


#define SERVER_PORT 4004

typedef enum {false, true} bool;


int com(int sd, struct sockaddr_in sin, int *sin_len);
int get(char* key, char* res, FILE *sockstream);
int put(char* key, char* value, char* res, FILE *sockstream);
int del(char* key, char* res, FILE *sockstream);
char * getFilename(char* filename);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf ("Bitte Aufruf mit %s <serverPort> bspw. %s 4001.\n", argv[0], argv[0]);
        exit (1);
    }
    
    int portNum = atoi(argv[1]);
    printf("Server starten mit Port %i.\n", portNum);
     
    
    struct sockaddr_in dest; /* socket info about the machine connecting to us */
    struct sockaddr_in serv; /* socket info about our server */
    int mySocket;            /* socket used to listen for incoming connections */
    socklen_t socksize = sizeof(struct sockaddr_in);

    memset(&serv, 0, sizeof(serv));           /* zero the struct before filling the fields */
    serv.sin_family = AF_INET;                /* set the type of connection to TCP/IP */
    serv.sin_addr.s_addr = htonl(INADDR_ANY); /* set our address to any interface */
    serv.sin_port = htons(portNum);           /* set the server port number */    

    mySocket = socket(AF_INET, SOCK_STREAM, 0);
    
    int option = 1;
    setsockopt(mySocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
  
    /* bind serv information to mysocket */
    bind(mySocket, (struct sockaddr *)&serv, sizeof(struct sockaddr));

    /* start listening, allowing a queue of up to 1 pending connection */
    listen(mySocket, 5);
    
    com(mySocket, serv, &socksize);
    
    close(mySocket);
    
    
    return EXIT_SUCCESS;
}

int com(int sd, struct sockaddr_in sin, int *sin_len) {
    int n_sd;
    char inputCommand[30];
    
    char delimiter[] = " ";
    char *exec;
    
    FILE *sockstream;
            
    char *ciao = "Vielen Dank, Sie haben einen einfachen Geldautomaten sehr glücklich gemacht.\n\n";
    
    while(true) {
        n_sd = accept(sd, &sin, sin_len);
        
        printf("Debug: Neuer Client per telnet.\n");
        
        sockstream = fdopen(n_sd, "r+");
        fprintf(sockstream, "%s", "Willkommen auf dem Datenserver. Bitte nutze GET, PUT oder DEL Kommandos.\n\n");
        rewind(sockstream);
        
        while(true) {
            sprintf(inputCommand, "");
            
            fscanf(sockstream, "%50[0-9a-zA-Z ]", inputCommand);
            rewind(sockstream);
            
            printf("Debug: %s eingetippt.\n", inputCommand);
            
            if (strlen(inputCommand) == 0) {
                continue;
            }
                                    
            exec = strtok(inputCommand, delimiter);
            
            printf("Debug: Kommando: %s.\n", exec);
            
            if (strcmp(exec, "GET") == 0) {
                exec = strtok(NULL, delimiter);
                get(exec, "?", sockstream);
            }
            
            if (strcmp(exec, "PUT") == 0) {
                exec = strtok(NULL, delimiter);
                put(exec, strtok(NULL, delimiter), "?", sockstream);
            }
            
            if (strcmp(exec, "DEL") == 0) {
                exec = strtok(NULL, delimiter);
                del(exec, "?", sockstream);
            }
            
            if (strcmp(exec, "EXIT") == 0) {
                printf("%s", "Debug: Client abgemeldet.\n");
                
                fprintf(sockstream, "%s", "Danke und tschö!\n");
                rewind(sockstream);
                
                break;
            } 
        }
        
        close(n_sd);
    }
    
    return EXIT_SUCCESS;
}

char * getFilename(char* filename) {
    char* response = malloc(50);
    
    sprintf(response, "db/%s.txt", filename);
    
    return response;
}

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
    }
        
    return EXIT_SUCCESS;
}

int del(char* key, char* res, FILE *sockstream) {
    printf("Debug: Lösche Datensatz \"%s\".\n", key);
    
    char *filename = getFilename(key);
    printf("Debug: Dateiname zum Löschen: %s.\n", filename);
    
    if (access(filename, F_OK) != -1) {
        // file exists
        
        printf("Debug: Datei \"%s\" existiert.\n", filename);
        
        unlink(filename);
    } else {
        printf("Debug: Datei \"%s\" existiert nicht.\n", filename);
    }
    
    return EXIT_SUCCESS;
}