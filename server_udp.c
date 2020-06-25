/******************************************
 * UDP Iterative server: server_udp.c
 ******************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // Internet family of protocols
#include <arpa/inet.h>
#include <unistd.h>
#define SOCK_MRP 1000
#define PORT 53000
#define BUFFSIZE 1024

int main(){
    int socDes ;
    struct sockaddr_in	thisAddr, thatAddr;
    int dataBytes, thatAddrLen ;
    char buff[BUFFSIZE] ;

    if((socDes = r_socket(PF_INET, SOCK_MRP, 0)) == -1) {
        perror("cannot create socket") ;
        return 0 ;
    }   

    thisAddr.sin_family = PF_INET ;
    thisAddr.sin_addr.s_addr =inet_addr( "127.0.0.1"); // inet_addr("127.0.0.1") ; 
                                        	  // Converts to 32-bit number
    thisAddr.sin_port = htons(PORT) ;  // Port number - byte order//180030703131

    if((r_bind(socDes, (struct sockaddr *)&thisAddr, sizeof(thisAddr))) < 0) {
        perror("cannot bind") ;
        return 0 ;
    }


    thatAddrLen = sizeof(thatAddr) ;

    if((dataBytes=r_recvfrom(socDes, buff, BUFFSIZE-1, 0,
                           (struct sockaddr *)&thatAddr, &thatAddrLen)) < 0) {
                 perror("cannot receive") ;
                 return 0 ;
             }

    buff[dataBytes] = '\0' ;
    printf("Data received: %s\n", buff) ;
    r_close(socDes);
    return 0 ;
}
