# STFFO
STFFO - Simple Telnetserver for Filesystem Operations

## compile
```bash
gcc -o server server.c
```

## start server
The first argument is the port of the telnet server you want to start.
```bash
./server 3210
```

## compile & start server
```bash
gcc -o server server.c && ./server 3210
```
