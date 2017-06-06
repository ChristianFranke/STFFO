int com(int sd, struct sockaddr_in sin, int *sin_len);
int get(char* key, char* res, FILE *sockstream);
int put(char* key, char* value, char* res, FILE *sockstream);
int del(char* key, char* res, FILE *sockstream);
int setTarget(char* key, char* res, FILE *sockstream);
char * getFilename(char* filename);

#define SERVER_PORT 4004
typedef enum {false, true} bool;