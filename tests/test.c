// test.c

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


#define SERVER_PORT 4003

typedef enum {false, true} bool;

// Funktionsdeklarationen:
// „com“ zur Kommunikation
// „aus“ zum Auszahlen eines Betrags
// „ein“ zum Einzahlen eines Betrags
// „stand“ zur Kontostandsabfrage

int com(int sd, struct sockaddr_in sin, int *sin_len);
int aus(FILE *sockstream, int *sum);
int ein(FILE *sockstream, int *sum);
int stand(FILE *sockstream, int *sum);

int main() {
    int sd;
    struct sockaddr_in sin;
    // Das struct „sin“ wird mit geeigneten Werten initialisiert.
    // Dabei entspricht AF_INET dem ARPA Internet Protokoll.
    // INADDR_ANY wird mit der IP des Hosts, auf dem der Server
    // läuft, initialisiert.
    int sin_len = sizeof (sin);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);
    // Hier wird das Socket erzeugt, bekommt einen Namen zugewiesen,
    // und es wird dafür gesorgt, dass es auf ankommende
    // Verbindungsanfragen lauscht.
    sd = socket(AF_INET, SOCK_STREAM, 0);
    bind(sd, (struct sockaddr *)& sin,
    sizeof(sin));
    listen(sd, 5);
    // Der Socket-Deskriptor wird an die Hauptfunktion übergeben.
    com(sd, sin, &sin_len);
    // Sobald das Programm aus der Hauptfunktion zurückkehrt, wird
    // das Socket geschlossen.
    unlink((const char *)& sin);
    return(EXIT_SUCCESS);
}
   
   
int com(int sd, struct sockaddr_in sin, int *sin_len) {
    // Einige Variablen: „n_sd“ ist der neue Socket-Deskriptor,
    // „choose“ wird entsprechend der gewünschten Operation gesetzt
    // und „sum“ ist das Startkapital auf dem Konto.
    int n_sd, choose, sum = 1000;
    FILE *sockstream;
    char *wel = "Herzlich Willkommen bei der G-Bank.\n";
    char *next = "Geld abheben (1)  Geld einzahlen (2)  Kontostand abfragen (3)\n  Verbindung beenden (4)\n  Ihre Eingabe: ";
            
    char *ciao = "Vielen Dank, Sie haben einen einfachen Geldautomaten sehr glücklich gemacht.\n\n";
    
    while(true) {
            n_sd = accept(sd, &sin, sin_len);
            sockstream = fdopen(n_sd, "r+");
            fprintf(sockstream, "%s", wel);
            rewind(sockstream);
            while(true) {
                // Zu Beginn der Schleife wird gefragt, was der Kunde mit
                // seinem Konto machen will.
                fprintf(sockstream, "%s", next);
                rewind(sockstream);
                fscanf(sockstream, "%i", &choose);
                rewind(sockstream);
                
                // Nachdem der Kunde gewählt hat, wird in ein entsprechendes
                // Unterprogramm gesprungen.
                if (choose == 1) {
                        aus(sockstream, &sum);
                }
                if (choose == 2) {
                        ein(sockstream, &sum);
                }
                if (choose == 3) {
                        stand(sockstream, &sum);
                }
                if (choose == 4) {
                        fprintf(sockstream, "%s", ciao);
                        rewind(sockstream);
                        
						break; 
				}
			}
	     // Sobald die innere Schleife verlassen wird, wird auch das nicht
	     // mehr benötigte Socket geschlossen.
	     close(n_sd);
	}
	
	return(EXIT_SUCCESS);
}


// Die Hilfsfunktionen für „com“:
int aus(FILE *sockstream, int *sum) {
     int in_sum;
     char *ask = "Wieviel Geld möchten Sie abheben? ";
     char *tell = "Ihr neuer Kontostand: ";
     fprintf(sockstream, "%s", ask);
     rewind(sockstream);
     fscanf(sockstream, "%i", &in_sum);
     rewind(sockstream);
     *sum -= in_sum;
     fprintf(sockstream, "%s%i\n\n", tell, *sum);
     rewind(sockstream);
	 return(EXIT_SUCCESS);
}

int ein(FILE *sockstream, int *sum) {
     int in_sum;
     char *ask = "Wieviel Geld möchten Sie einzahlen? ";
     char *tell = "Ihr neuer Kontostand: ";
     fprintf(sockstream, "%s", ask);
     rewind(sockstream);
     fscanf(sockstream, "%i", &in_sum);
     rewind(sockstream);
     *sum += in_sum;
     fprintf(sockstream, "%s%i\n\n", tell, *sum);
     rewind(sockstream);
     return(EXIT_SUCCESS);
}


int stand(FILE *sockstream, int *sum) {
    char *tell = "Ihr aktueller Kontostand: ";
    fprintf(sockstream, "%s%i\n\n", tell, *sum);
    rewind(sockstream);
    return(EXIT_SUCCESS);
}