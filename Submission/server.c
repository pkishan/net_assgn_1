#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char **argv)
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 
    int len;
    char writeBuff[1024];       //This buffer is used to write the text from file to buffer and used to transfer data
    char readBuff[1024];        //This buffer is used to read the file name from the client
    char num_chunk[1024];       //This buffer is used to store the number of chunks
    int port;
    struct stat st;             //This structute is used to find the length of the file

    if(argc != 2)               //Checking if all the required arguments are given or not
    {
        fprintf(stderr,"\n Usage: %s <port number> \n", argv[0]);
        return 1;
    }

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0))<0)//Checking if the socket is created or not
    {
        fprintf(stderr,"\n Error : Could not create socket");
    } 

    bzero((char *)writeBuff, sizeof(writeBuff));    //Setting all the elemets of buffer to 0
    bzero((char *)readBuff, sizeof(readBuff));
    bzero((char *)&serv_addr, sizeof(serv_addr));
    bzero((char *)num_chunk, sizeof(num_chunk));

    port = atoi(argv[1]);       //Port store the port number that is given in the initial arguments
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); //Binding the server to the server address

	if(listen(listenfd,1)!=0)      //Check if the server has started listening or not
	{
		printf("Unable to listen \n");
        return 1;
	} 

    else
    {
        printf("Server is Listening \n");
    }

    while(1)        //An infinte loop to keep the server running
    {
        if((connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL)) <0) //This is to check if a client is connected to the server or not
        {
            printf("Could not create new socket \n");
            return 1;
        }

        FILE *fp = fdopen(connfd,"r+b");        //Converting the socket so that it behaves like a file stream
        fwrite("Connection Succeful, Please give a file name\n", 1, 45, fp);
        while(1)    //This infinite loop is to listen to multiple requests from the same client
        {
            int a = fread(readBuff, 1, 100, fp);    //This buf is used to read the name of the file from client
            if(feof(fp))
            {
                printf("Client has closed the connection, again listening \n");
                break;
            }

            int i =0;
            while(1)    //This loop is to change the readBuff so that we can open a file
            {

                if (readBuff[i] == '\n')
                {
                    readBuff[i] = '\0';
                    break;
                }
                i = i + 1;


            }
            FILE *fp1 = fopen(readBuff,"rb");       //Opening the file that the client requested for

            if(fp1==NULL)                           //Checking if the file is present or not
            {
                fwrite("1", 1, 1, fp);
                continue;
            }   

            else
            {
                bzero((char *)num_chunk, sizeof(num_chunk));
                
                fwrite("0", 1, 1, fp);
                
                int filesize = 0;

                stat(readBuff, &st);                //This is used to get the size of the file
                filesize = st.st_size;

                if(filesize%1023 ==0)               //Here we are finding the number of chunks that will be transfered
                    filesize = filesize/1023;
                
                else
                    filesize = filesize/1023 +1 ;
                
                sprintf(num_chunk, "%d", filesize);
                fwrite(num_chunk, 1, 1024, fp);     //Sending the client the number of chunks that the server will send
                
                int i =0;
                
                while(i<filesize)                   //Sending the client chunks of data from the file requested
                {
                    bzero((char *)writeBuff, sizeof(writeBuff));
                    int b = fread(writeBuff,1,1023,fp1);
                    printf("Bytes read %d \n", b);        

                    if(b > 0)
                    {
                        int a = fwrite(writeBuff,1,1023, fp);
                        fflush(fp);
                    }

                    if (i == filesize -1 || b < 1023 )      //Checking if the bits sends are lees than what we expected
                    {
                        if (feof(fp1))
                            printf("File transfer complete\n");
            
                        if (ferror(fp1))
                            printf("Incomplete file transfer, error\n");
                        fflush(fp);
                        break;
                    }
                    i = i + 1;

                }

                fclose(fp1);                    //Closing the file that was requested by the client
            }
        }

        close(connfd);                          //Closing the socket connection
        sleep(1);
    }

}