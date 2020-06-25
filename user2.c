/****************************************
 * UDP Client : client_udp.c
 ****************************************/

#include <stdio.h>
 #include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // Internet family of protocols
#include <arpa/inet.h>
#include <unistd.h>
#define SOCK_MRP 1000
#define SERVERIP "127.0.0.1"
#define PORT     54000
#define BUFFSIZE 1024

 main()
 {
 	int socDes ;
    struct sockaddr_in	thisAddr, thatAddr;
    int dataBytes=8, thatAddrLen ;
    char buff[BUFFSIZE] ;
    int dataSent;
    char sentdata;
     if((socDes = r_socket(PF_INET, SOCK_MRP, 0)) == -1) 
     {
        perror("cannot create socket") ;
        return 0 ;
    } 

      thatAddr.sin_family = PF_INET ;
    thatAddr.sin_addr.s_addr = inet_addr(SERVERIP) ; // Server IP
                                        // Converts to 32-bit number
    thatAddr.sin_port = htons(PORT) ;  // Port number - byte order
   /* sentdata='d';

    if((dataSent = r_sendto(socDes, &sentdata, dataBytes, 0,
                      (struct sockaddr *)&thatAddr, sizeof(thatAddr))) < 0) 
         {
        perror("cannot send") ;
        return 0 ;
    }*/

        fgets(buff,BUFFSIZE,stdin);
    int i;
    for(i=0;i<(int)strlen(buff)-1;i++)
    {
    	sentdata=buff[i];
    	 if((dataSent = r_sendto(socDes, &sentdata, dataBytes, 0,
                      (struct sockaddr *)&thatAddr, sizeof(thatAddr))) < 0) 
    	 {
        perror("cannot send") ;
        return 0 ;
    	}
    }
    r_close(socDes);

 }