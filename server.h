int com(int sd, struct sockaddr_in sin, int *sin_len);
int get(char* key, char* res, FILE *sockstream);
int put(char* key, char* value, char* res, FILE *sockstream);
int del(char* key, char* res, FILE *sockstream);
int setTarget(char* key, char* res, FILE *sockstream);
char * getFilename(char* filename);
void* createSharedMemory(size_t size);

#define SERVER_PORT 4004
#define ROM 0
typedef enum {false, true} bool;

#define SEGSIZE sizeof(int)

// target for saving buckets
typedef enum { HDD = 1, RAM = 2 } target;
target currentTarget = RAM;

// keyValueStorage for ram saving
struct keyValueStorage {
    char key[64];
    char value[1024];
};
 
struct keyValueStorage keys[1024];

// semaphores 
int sem_id;
struct sembuf enterRead, leaveRead, enterWrite, leaveWrite;
unsigned short marker[2];
int rc;