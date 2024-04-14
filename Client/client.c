

/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>

#define BUFSIZE 1024

/*
 * error - wrapper for perror
 */
void error(char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char **argv) 
{
  int sockfd; /* socket */
  int portno; /* port to listen on */
  int clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
  char buf[BUFSIZE]; /* message buf */
  char command[BUFSIZE];// to store the command part
  char file_name[BUFSIZE]; // tostore the file name.
  char *hostaddrp; /* dotted decimal host addr string */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */

  /* 
   * check command line arguments 
   */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  portno = atoi(argv[1]);

  /* 
   * socket: create the parent socket 
   */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
  optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
	     (const void *)&optval , sizeof(int));

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)portno);

  /* 
   * bind: associate the parent socket with a port 
   */
  if (bind(sockfd, (struct sockaddr *) &serveraddr, 
	   sizeof(serveraddr)) < 0) 
    error("ERROR on binding");

  /* 
   * main loop: wait for a datagram, then echo it
   */
  
  while (1) {

clientlen = sizeof(clientaddr);
    /*
     * recvfrom: receive a UDP datagram from a client
     */
     bzero(command,1024);
     bzero(file_name,1024);
    bzero(buf, BUFSIZE);
    
          n = recvfrom(sockfd, buf, 1024, 0,  (struct sockaddr *)&clientaddr, &clientlen);
          if (n < 0) 
            error("ERROR in recvfrom");

          //In the case of file existing we first receive the size of the file in bytes.
          int count = atoi(buf);
          bzero(buf, BUFSIZE);
          count= count-2;
          
while(count--)

{
	n = recvfrom(sockfd, buf, BUFSIZE, 0,
		 (struct sockaddr *) &clientaddr, &clientlen);
    if (n < 0)
      error("ERROR in recvfrom");
    n = recvfrom(sockfd, command, BUFSIZE, 0,
		 (struct sockaddr *) &clientaddr, &clientlen);
    if (n < 0)
      error("ERROR in recvfrom");
    n = recvfrom(sockfd, file_name, BUFSIZE, 0,
		 (struct sockaddr *) &clientaddr, &clientlen);
    if (n < 0)
      error("ERROR in recvfrom");
    bzero(buf, BUFSIZE);
    
        if(!(strcmp(command,"put")))
    {
          printf("Receiving file size\n");
          n = recvfrom(sockfd, buf, 1024, 0,  (struct sockaddr *)&clientaddr, &clientlen);
          if (n < 0) 
            error("ERROR in recvfrom");

          //In the case of file existing we first receive the size of the file in bytes.
          int file_size = atoi(buf);
          char *endptr;
          file_size = strtol(buf, &endptr, 10);
          bzero(buf,1024);
          printf("Receiving file %s from client  of Filesize: %d\n", file_name, file_size);
          FILE* file_write = fopen(file_name, "wb");
          if(file_write == NULL)
          {
              error("Error in opening the file to write the data that is to be recieved");
          }
          //int cumulatives_bytes_transferred=0;
          int byte_transfer_size=1024;
          for(int cumulatives_bytes_transferred=0; file_size>=cumulatives_bytes_transferred;cumulatives_bytes_transferred+=byte_transfer_size)
          {
              bzero(buf, 1024);
              // considering last transfer case.
              if(file_size- cumulatives_bytes_transferred < 1024)
              {
                byte_transfer_size= file_size- cumulatives_bytes_transferred;
              }
              n = recvfrom(sockfd, buf, 1024, 0, (struct sockaddr *)&clientaddr, &clientlen);
              if (n < 0)
                error("ERROR in recvfrom");
              // cumulative count increments by an amount of byte_transfer_size.
              printf("Received %d bytes of data in total till now from %d in total\n",cumulatives_bytes_transferred,file_size);
              fwrite(buf, byte_transfer_size, 1, file_write);
              if(byte_transfer_size<1024)
              {
                printf("file transfer complete to client\n");
                //fclose(file_write);
                break;
              }

          }
          fclose(file_write);
    }
      
}          
  }
  return 0;
}