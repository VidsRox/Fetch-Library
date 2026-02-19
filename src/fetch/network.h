#ifndef NETWORK_H
#define NETWORK_H

#pragma once
#include <sys/syscall.h>

#define AF_INET      2 //IPv4
#define SOCK_STREAM  1

typedef unsigned char bool;
typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;
typedef unsigned int Inaddr;


// Why does this structure exist -> Matches the kernel's definition exactly
// From Linux kernel: include/uapi/linux/in.h
struct s_sockaddr {
    int16 family;
    int16 port;
    Inaddr addr;
    int8 padding[8];
};
typedef struct s_sockaddr Sockaddr;


#define $c (char *)
#define $i (int)
#define $8 (int8 *)
#define $6 (int16)
#define $2 (int32)
#define $4 (int64)


//system call wrapper
long int syscall(long, ...);

//socket operations
int32 socket_create(int32 domain, int32 type, int32 protocol);

bool socket_connect(int32 s, Sockaddr* sock, int32 size);

int32 socket_send(int32 s, const char *data, int32 len);

int32 socket_recv(int32 s, char *buffer, int32 len);

void socket_close(int32 s);

#endif  // NETWORK_H
