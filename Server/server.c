/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <dirent.h>

#define BUFSIZE 1024
#define NMBEB 1

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char **argv) 
{
    int sockfd, portno, n;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];
    char command[BUFSIZE];
    char file_name[BUFSIZE];

    /* check command line arguments are valid in count */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       //exit(0);
    }
    hostname = argv[1];
    // converting string input into an integer.
    portno = atoi(argv[2]);
    if(portno < 5001 || portno > 65534)
    {
        error("Port should be grater than 5000 and less than 65535");
    }

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    while(1)
    {
      /* get a message from the user */
      bzero(buf, BUFSIZE);
      bzero(command, BUFSIZE);
      bzero(file_name, BUFSIZE);
      printf("Please enter msg: ");
      fgets(buf, BUFSIZE, stdin);
      serverlen = sizeof(serveraddr);
      int count=0;
      	DIR* directory;
	struct dirent *entry;
	directory=opendir("/mnt/c/Users/gurra/Pictures/a");
	while((entry= readdir(directory))!= NULL)
        {
        count++;
        }
    closedir(directory);
    directory=opendir("/mnt/c/Users/gurra/Pictures/a");
        printf("\r count is %d\n",count);
        bzero(command, 1024);
        sprintf(command, "%d", count);
	n = sendto(sockfd, command, strlen(command), 0, (struct sockaddr*)&serveraddr, serverlen);
        if (n < 0)
        {
        	error("ERROR in sendto");
        }         
        bzero(command, 1024);
	char *token = strtok(buf, " \t\n");
      	if (token != NULL) 
      	{
          strcpy(command, token); // Copy the first token to cmd
        }
        
        
while(count--)
{
		entry= readdir(directory);
		strcpy(file_name, entry->d_name);
    if(strcmp(file_name, ".")==0)
    {
      bzero(file_name, BUFSIZE);
      continue;
    }
    if(strcmp(file_name, "..")==0)
    {
      bzero(file_name, BUFSIZE);
      continue;
    }
		//strcpy(file_name, entry->d_name);
		//printf("%s",file_name);
		serverlen = sizeof(serveraddr);
        n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&serveraddr, serverlen);
        if (n < 0) 
          error("ERROR in sendto");
        n = sendto(sockfd, command, strlen(command), 0, (struct sockaddr*)&serveraddr, serverlen);
        if (n < 0) 
          error("ERROR in sendto");
        n = sendto(sockfd, file_name, strlen(file_name), 0, (struct sockaddr*)&serveraddr, serverlen);
        if (n < 0) 
          error("ERROR in sendto");

      bzero(buf,1024);
      

      if (!(strcmp(command,"put")))
      {
        printf("File to be transferred is : %s\n",file_name);
        char full_path[200];
        snprintf(full_path, 200, "/mnt/c/Users/gurra/Pictures/a/%s", file_name);
        FILE* file_read = fopen(full_path, "rb");

        //FILE* file_read= fopen(file_name,"rb");
        if(file_read== NULL)
        {
          printf("The file %s requested  DOES NOT EXIST \n", file_name);
          continue;
        }
        // serverlen = sizeof(serveraddr);
        // n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);
        // if (n < 0) 
        //   error("ERROR in sendto");

        // now should send the file size to the server.
        // Seek to the end of the file to determine its size
        fseek(file_read, 0, SEEK_END);
        // Get the file size
        int file_size= ftell(file_read);
        //fclose(file_read);
        printf("File_size is determined as %d \n",file_size);
        bzero(buf, 1024);
        sprintf(buf, "%d", file_size);
        n = sendto(sockfd, buf, strlen(buf), 0,
               (struct sockaddr *) &serveraddr, serverlen);
        if (n < 0)
          error("ERROR in sendto");
        bzero(buf, 1024);
        fseek(file_read, 0, SEEK_SET);
        // have to send file data 1024 bytes at atime.
          //int cumulatives_bytes_transferred=0;
          int byte_transfer_size=1024;
          for(int cumulatives_bytes_transferred=0; file_size>=cumulatives_bytes_transferred;cumulatives_bytes_transferred+=byte_transfer_size)
          {
            bzero(buf, sizeof(buf));
            // considering last transfer case.
            if(file_size- cumulatives_bytes_transferred < 1024)
            {
              byte_transfer_size= file_size- cumulatives_bytes_transferred;
            }
            int bytes_sent=fread(buf, 1, byte_transfer_size, file_read);
            n = sendto(sockfd, buf, bytes_sent, 0, (struct sockaddr *)&serveraddr, serverlen);
            if (n < 0)
              error("ERROR in sendto");
            // cumulative count increments by an amount of byte_transfer_size.
            printf("Sent %d bytes of data in total till now from %d in total\n",cumulatives_bytes_transferred,file_size);
              
            if(byte_transfer_size<1024)
            {
                printf("file transfer complete from  server\n");
                fclose(file_read);
                remove(full_path);
                break;
            }
          }
      }
	
		
		
}
    }

     return 0;
}