#ifndef GRAB_H
#define GRAB_H

#include <stdio.h>
#include <stdlib.h>
#include "network.h"

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;

#define $c (char *)
#define $i (int)
#define $8 (int8 *)
#define $6 (int16)
#define $2 (int32)
#define $4 (int64)
#define true  1
#define false 0

//Content-Type: application/json

typedef unsigned char bool;

enum e_protocol {
    http = 1,
    https = 2
};
typedef enum e_protocol Protocol;

enum e_method {
    head = 1,
    get = 2,
    post = 3,
    put = 4, 
    delete = 5,
    patch = 6
};
typedef enum e_method Method;

struct s_url{
    Protocol protocol;
    int8 domain[128];
    int8 file[128];
};
typedef struct s_url Url;

struct s_header{
    int8 key[64];
    int8 content[256];

};
typedef struct s_header Header;

struct s_cookie{
    int8 key[64];
    int8 content[256];
    int64 timestamp;
    bool secure;
};
typedef struct s_cookie Cookie;

struct s_request {
    Method method;
    Url url;
    int8 *contentType[256];
    Cookie *cookies;
    Header *headers;
    int8 *data;
    int64 size;
};
typedef struct  s_request Request;


struct s_response {
    int16 status;
    int8 *contentType[256];
    Header *headers;
    Cookie *cookies;
    int8 *data;
    int64 size;
};
typedef struct s_response Response;

int main();

#endif  // GRAB_H