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
#include <sys/sem.h>
#include <netinet/in.h>

#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <limits.h>

#include "server.h"
#include "com.inc.c"
#include "util.inc.c"
#include "semph.c"





int main(int argc, char *argv[]) {
    
    if (argc < 2) {
        printf ("Bitte Aufruf mit %s <serverPort> bspw. %s 4001.\n", argv[0], argv[0]);
        exit (1);
    }
    
    // string2int
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
    fflush(stdout);
    
    
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
