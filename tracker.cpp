#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <fstream>
#include "hash_function.h"
#include "tdata.h"
#include <map>


using namespace std;


string file ="";
map < string , string > tracker_data ;



int main(int argc, char *argv[])
{
    int sockfd , ret ;
    struct sockaddr_in serverAddr;
    
    int newSocket;
    struct sockaddr_in newAddr;

    char buffer[1024];
    pid_t childpid;

    socklen_t addr_size;
  
    string myip = "";  
    string PORT = "" ;

 
    char* token = strtok(argv[1] , ":");
    myip += token;
    token = strtok(NULL," ");
    PORT += token;
    token = strtok(argv[2] , " ");
    file += token;

    sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if(sockfd < 0)
    {
        printf("[-]Error in connection.\n");
        exit(1);
    }
    printf("[+]Server Socket created succesfully\n");

    memset(&serverAddr , '\0' , sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons( atoi( &PORT[0] ) ); 
    serverAddr.sin_addr.s_addr = inet_addr(&myip[0]);

    ret = bind( sockfd , (struct sockaddr*)&serverAddr , sizeof(serverAddr));
    
    if(ret < 0)
    {
        printf("[-]Error in binding");
        exit(1);
    }

    printf("[+]Bind to the Port.\n");

    if(listen(sockfd ,10) == 0 )
    {
        printf("[+]Listening...");
    }
    else
    {
        printf("[+]Error in Linking");
    }

    while(1)
        {
            newSocket = accept(sockfd , (struct sockaddr *)&newAddr , &addr_size);

            if(newSocket < 0)
            {
                exit(1);
            }
            printf("Connection accepted from %s:%d\n",inet_ntoa(newAddr.sin_addr),ntohs(newAddr.sin_port));
 
            if((childpid = fork()) == 0)
            {
                close(sockfd);
                read_to_log(file);
                while(1)
                {
                    memset(&buffer , '\0' , 1024);
                    recv(newSocket , buffer , 1024 ,0);
                    char tokenized_array[50][500];
                    int  arg_index = 0; 
                    char* token = strtok(buffer , " ");

                    while( token != NULL)
                    {
                        strcpy( tokenized_array[arg_index++] , token)  ;
                        token = strtok(NULL," ");
                    }
                    if(strcmp( tokenized_array[0] ,"exit" ) == 0 )
                    {
                        write_to_log(file);
                        printf("Disconnected from %s:%d\n",inet_ntoa(newAddr.sin_addr),ntohs(newAddr.sin_port));
                        break;
                    }
                    else if(strcmp(tokenized_array[0] , "remove") != 0 &&  strcmp(tokenized_array[2] ,"share") == 0 )
                    {
//                        cout<<buffer<<endl;
                        string file(tokenized_array[1]); 
                        file = find_src_file(file);
                        strcpy(tokenized_array[1] , &file[0]); 
                        formatting( tokenized_array[0] , inet_ntoa(newAddr.sin_addr) , atoi(tokenized_array[3]) , tokenized_array[1] );
                        memset(&buffer , '\0' , 1024);
                        //strcpy(buffer , "[+]SUCCESS"); 
                        send(newSocket , buffer , 1024 ,0);
                    }
                    else if(strcmp(tokenized_array[0] ,"seederlist") == 0)
                    {
                        string seeders = get_data(tokenized_array[1]);
                        memset(&buffer , '\0' , 1024);
                        strcpy(buffer , &seeders[0] );
                        send(newSocket , buffer, 1024 , 0);
                        memset(&buffer , '\0' , 1024);
                    }
                    else if(strcmp(tokenized_array[0] , "remove") == 0)
                    {
//                        cout<<buffer<<endl;
                        string hash = "";
//                        cout<<"Removing"<<endl;
                        hash += tokenized_array[1];
//                        cout<<"ta "<<tokenized_array[1]<<endl;
                        remove_formatting( &hash[0] , inet_ntoa(newAddr.sin_addr) , atoi(tokenized_array[2]));
                          
                    }
                    else if( strcmp(buffer , "exit") == 0 )
                    {
                        exit(1);
                    }
                    else
                    {

                    }
                }
            }
    }

        close(newSocket);

        return 0 ;
}
