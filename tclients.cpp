#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include "hash_function.h"
#include "connect_client.h"
#include "piece.h"
#include <cstdio>
#include <fstream>
#include <map>
#include <vector> 
#include <thread>

using namespace std;

#define PORT 4444

//#define LISTEN_PORT 20000

map < string , map< string , int > > bitmap;
map < string , string > torrentpath;
vector < string > downloaded ;
vector <string > downloading ;

int fired = 0;



int main(int argc, char *argv[])
{
    int clientSocket , ret ;
    struct sockaddr_in serverAddr;
    char buffer[1024];
    pid_t childpid;

    string ip_client  = "";
    string LISTEN_PORT = "";
    string ip_tracker ="";
    string tracker_port ="";


    char* token = strtok(argv[1] , ":");
    ip_client += token;
    token = strtok(NULL," ");
    LISTEN_PORT += token;
    token = strtok(argv[2] , ":");
    ip_tracker += token;
    token = strtok(NULL," ");
    tracker_port += token;

    clientSocket = socket(AF_INET , SOCK_STREAM , 0);
    if(clientSocket < 0)
    {
        printf("[-]Error in connection.\n");
        exit(1);
    }
    printf("[+]Client Socket created succesfully\n");

    memset(&serverAddr , '\0' , sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi( &tracker_port[0] ) ); 
    serverAddr.sin_addr.s_addr = inet_addr( &ip_tracker[0] );

    ret = connect( clientSocket , (struct sockaddr*)&serverAddr , sizeof(serverAddr));
    if(ret < 0)
    {
        printf("[-]Error in connection");
        exit(1);
    }
    printf("[+]Connected to the Server.\n");



    while(1)
        {
            memset(&buffer , '\0' , 1024);
            printf("Client :  ");
            bzero(buffer , 1024);
            scanf("%[^\n]%*c", &buffer[0]);

            char tokenized_array[50][500];
            int  arg_index = 0; 
            char* token = strtok(buffer , " ");

            while( token != NULL)
            {
                strcpy( tokenized_array[arg_index++] , token)  ;
                token = strtok(NULL," ");
            //    cout<<tokenized_array[0];
            }

            if( strcmp( tokenized_array[0] ,"exit" ) == 0 )
            {
                send(clientSocket , buffer, 1024 , 0);
                close(clientSocket);
                printf("[-]Disconnected from server.\n");
                exit(1);
            }

            else if( strcmp( tokenized_array[0] ,"share" ) == 0 )
            {
                unsigned int length , index =0;
                unsigned char to_tracker_buffer[512*1024];
                memset((char *)&to_tracker_buffer, '\0' , strlen((char *)to_tracker_buffer));
                memset(&buffer, '\0' , strlen(buffer));
              
                length = hash_function( tokenized_array[1] , tokenized_array[2] ,to_tracker_buffer);

                calculate_hash( to_tracker_buffer , length);
                for(int k = 0 ;k < 10 ; k++ )
                {
                    sprintf( &buffer[index*2] ,"%02x", to_tracker_buffer[k] );
                    index++;
                }
                strcat( buffer , " " );
                strcat( buffer , tokenized_array[1] );
                strcat( buffer , " " );
                strcat( buffer , "share");
                strcat( buffer , " ");
                strcat( buffer , &(LISTEN_PORT)[0]);
                strcat( buffer , " ");
                send(clientSocket , buffer, 1024 , 0);
                recv(clientSocket  , buffer , 1024 ,0);
               // cout<<buffer<<":"<<tokenized_array[1]<<endl;
                bzero(buffer , 1024);
                cout<<"[+]SUCCESS : "<<tokenized_array[1]<<endl;
                if(fired == 0)
                    {
                        thread t1( connect_client, LISTEN_PORT , ip_client );
                        t1.detach();
                        fired = 1; 
                    }
            }
            else if( strcmp( tokenized_array[0] ,"get") == 0 )
            {
                memset(&buffer, '\0' , strlen(buffer));
                strcpy(buffer , "seederlist");
                strcat(buffer , " ");
                downloading.push_back(tokenized_array[1]);

                string seed_sha1 = get_from_torrent(tokenized_array[1]);
                strcat( buffer , &seed_sha1[0] );
            //    cout<<buffer<<endl;
                send(clientSocket , buffer, 1024 , 0);
                memset(&buffer, '\0' , strlen(buffer));
                recv(clientSocket , buffer , 1024 ,0);
               
                string ip_tokens(buffer);
                string dest_file(tokenized_array[2]);

                thread t2(connect_server , ip_tokens  , seed_sha1 , dest_file );     
                t2.detach();
                continue;

            }
            else if(strcmp( tokenized_array[0] ,"remove") == 0)
            {

                string seed_sha1 = get_from_torrent(tokenized_array[1]);
                strcat(buffer , " ");
                strcat( buffer , &seed_sha1[0] );
                strcat( buffer , " ");
                strcat( buffer , &LISTEN_PORT[0]);
                //cout<<tokenized_array[1]<<endl;
                remove(tokenized_array[1]);
            //    cout<<buffer<<endl;
                send(clientSocket , buffer , 1024 , 0 );
                
            }
            else if( (strcmp( tokenized_array[0] , "show") == 0 ) && (strcmp( tokenized_array[1] , "downloads") == 0 ))
            {
                show();
            }
            else
            {
                printf("[-]INVALID_ARGUMENTS\n");
            }
            
                
            
        }
        return 0 ;
}