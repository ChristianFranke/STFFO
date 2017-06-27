# STFFO
STFFO - Simple Telnetserver for Filesystem Operations

## dependencies
* a compiler for C: gcc or clang

## installation

### compile
```bash
gcc -o server server.c
```

### start server
The first argument is the port of the telnet server you want to start.
```bash
./server 3210
```

### compile & start server
```bash
gcc -o server server.c && ./server 3210
```
## using

if the server is running, connect by telnet:
```bash
$ telnet localhost 3210
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Willkommen auf dem Datenserver. Bitte nutze GET, PUT oder DEL Kommandos.

PUT key value
GET key
value
DEL key
TARGET HDD
Speicherziel auf HDD geändert.
```

The key-value database is persistent stored in ./db.
