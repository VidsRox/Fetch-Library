#ifndef UTILS_H
#define UTILS_H

#pragma once

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;

//byte order conversion
int16 htons(int16 port);
int32 htonl(int32 addr);

//string to IP conversion
int32 inet_addr(const char *ip);

//string utilities (using libc not allowed)
int my_strlen(const char *str);
void my_strcopy(char *dest, const char *src);
int my_strcmp(const char *s1, const char *s2);
void my_sprintf_simple(char *buffer, const char *format, ...);

#endif