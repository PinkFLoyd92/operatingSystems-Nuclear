#ifndef _SOCKET_H_
#define _SOCKET_H_
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>

#define IP_ADDRESS "http://localhost"
char src[50];

int
doPost(char* route, char* param);

#endif
