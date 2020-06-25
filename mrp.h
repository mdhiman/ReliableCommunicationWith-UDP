#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // Internet family of protocols
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int r_socket(int , int , int );

int r_bind(int , const struct sockaddr*,socklen_t );

ssize_t r_sendto(int, const void *, size_t , int ,const struct sockaddr *, socklen_t );

int r_close(int );

ssize_t r_recvfrom(int , const void* , size_t , int ,const struct sockaddr *, socklen_t );

int dropMessage(float);

int isUMTEmptyAndAllRecvdRead();

void killAndDeleteEverything(int);
