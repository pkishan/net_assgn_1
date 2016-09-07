#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <sys/stat.h>


int main(int argc, char **argv)
{
    int sockfd = 0;                 //This is the socket descriptor
    int n = 0;
    char recvBuff[1023] ;           //This buffer that is used to receive data in chunks from the client  
    char num_chunk[1024];           //This buffer is used to store the total number of chunks that we will be receieving
    struct sockaddr_in serv_addr;   // This is for the socket address
    char stat[1];                   
    int port;                       //This store the port number that we will be using
    struct stat;


    if(argc != 3)                   // Checking if all the required arguments are given or not
    {
        printf("\n Usage: %s <ip of server, port number> \n",argv[0]);
        return 1;
    } 

    memset(recvBuff, '0',sizeof(recvBuff));     //This sets the whole buffer to '0'
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)      //Checking if the socket had been created or not
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr));     //Setting the buffer to '0'

    port = atoi(argv[2]);                           //Getting the socket port number from argv
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);               

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)         //Conver the IP address input that is given
    {
        printf("\n inet_pton error occured\n");         // Return an error if we are not able to do the conversion.
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  //Establishing a connection to the server
    {
       printf("\nError : Connect Failed \n");
       return 1;
    } 
    FILE *fp = fdopen(sockfd, "r+b");       //Converting a socket to behave like file stream

    char welcome[45];
    fread(welcome, 1, 45, fp);
    fputs(welcome, stdout);

    while(1)
    {
        char buf[100];

        fputs("\nEnter File Name: \n",stdout);    //Asking the file name of file 
        fflush(stdout);
        fgets(buf,100,stdin);                   //We take input from stdin


            fwrite(buf,1,100,fp);
            
            
            fflush(fp);
            int check = fread(stat, 1, 1, fp);   //Sending a bit to check if the file exists or not
            if (check == 0)     //Checking if we are getting data from the server
            {
                printf("\nError : Could not reach the server \n");
                return 1;
            }           
 
            if ((int)stat[0] == 49)             //Cehcking if the file exits or not
            {
                printf("\n No such file exists \n");
                continue;
            }
 
            fread(num_chunk, 1, 1024, fp);      //This reads the number of chunks that will be transfered
        
            int num = atoi(num_chunk);
            int j = 0;
 
            while (j < num)                     // A while loop to read the chunks that are being send by the server
            {
                n = fread(recvBuff,1, 1023,fp);
        
                fputs(recvBuff,stdout);         //Printing the received chunks in the stdout
                fflush(stdout);
                if(n <1023 && j <num -1)
                {
                    printf("\n Read Error \n");
                    break;
                }
               j = j + 1; 
            } 
            
             printf("\n");
 
    } 
    return 0;
}