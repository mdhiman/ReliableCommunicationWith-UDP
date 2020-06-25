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
    int dataBytes, thatAddrLen ;
    char buff ;
    char sentdata;
    int flag=0;
     if((socDes = r_socket(PF_INET, SOCK_MRP, 0)) == -1) 
     {
        perror("cannot create socket") ;
        return 0 ;
    } 
       

    thisAddr.sin_family = PF_INET ;
    thisAddr.sin_addr.s_addr = inet_addr( "127.0.0.1"); // Server IP
                                        // Converts to 32-bit number
    thisAddr.sin_port = htons(PORT) ;  // Port number - byte order

 if((r_bind(socDes, (struct sockaddr *)&thisAddr, sizeof(thisAddr))) < 0) {
        perror("cannot bind") ;
        return 0 ;
    }
 
    /*if((dataBytes=r_recvfrom(socDes, &buff, BUFFSIZE-1, 0,

                           (struct sockaddr *)&thatAddr, &thatAddrLen)) < 0)
            {
                 perror("cannot receive") ;
                 return 0 ;
             }
             printf("message  from  ...%c\n",buff );*/


while(flag==0)
{
    //printf("hello..\n");
	 if((dataBytes=r_recvfrom(socDes, &buff, BUFFSIZE-1, 0,

                           (struct sockaddr *)&thatAddr, &thatAddrLen)) < 0)
	 		{
                 perror("cannot receive") ;
                 return 0 ;
             }

            else
            {
                
            	sentdata=buff;
                printf("%c",sentdata);
                if(sentdata=='$')
                    flag=1;
            }
}

 }