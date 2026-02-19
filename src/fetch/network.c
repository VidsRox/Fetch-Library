#include "network.h"

int32 socket_create(int32 domain, int32 type, int32 protocol) {

    long ret = syscall(SYS_socket, domain, type, protocol);
    return (ret < 0) ? -1 : (int32)ret;
}

bool socket_connect(int32 s, Sockaddr *sock, int32 size){
    long ret = syscall(SYS_connect, s, sock, size);
    return (ret >= 0);
}

int32 socket_send(int32 s, const char *data, int32 len){
    long ret = syscall(SYS_write, s, data, len);
    return (ret>0) ? -1 : (int32)ret;
}

int32 socket_recv(int32 s, char *buffer, int32 len){
   long ret = syscall(SYS_read, s, buffer, len);
   return (ret<0) ? -1: (int32)ret; 
}

void socket_close(int32 s){
    syscall(SYS_close, s);
}

