#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <fstream>
#include "hash_function.h"
#include "tdata.h"
#include "piece.h"
#include <cstring>
#include <map>
#include <vector>
#include <thread>
#include <sstream>

using namespace std;


extern map < string , map< string , int > > bitmap;
extern map < string , string > torrentpath;
extern vector <string > downloaded ;
extern vector <string > downloading ;


void upload(string final_bitmap , string filepath , int newSocket)
{
//    cout<<final_bitmap<<endl<<filepath<<endl;
    char buffer[1024];
    string fullFilePath = "";
    fullFilePath +=filepath;
   
        //    cout<<"inside"<<endl;
           
            unsigned long chunkSize = 1024;
        //    cout<<fullFilePath<<endl;
            FILE *fp = fopen(&fullFilePath[0], "rb");
        //    cout<<fullFilePath<<endl;
            fseek(fp, 0, SEEK_END);
            unsigned long filesize = ftell(fp);
            rewind(fp);
            int num_times;
            if(filesize % (512*1024) != 0)
                num_times = filesize / (512*1024) + 1;
            else
                num_times = filesize / (512*1024);
         //   cout<<filesize<<endl;
         //   cout<<num_times<<endl;
            // string bitmap = "11111111111";
            unsigned long recvd;
            int i=0;
            if(fp!=NULL)
            {
                // store read data into buffer
                unsigned long sent = 0;
                for(int i =0 ; i < num_times ; i++)
                {    
                    if(final_bitmap[i] == '1')
                    { 
                        for(int j = 0; j < 512 ; j++)
                        {
                            if( (filesize - sent) >= 1024 )
                            {
                                fread(buffer, sizeof(char), 1024, fp);
                                send(newSocket, buffer, 1024, 0);
                                recvd = recv(newSocket , buffer , 1024 ,0);
                                sent += 1024;
                            }
                            else
                            {
                                fread(buffer , sizeof(char) , filesize-sent , fp);
                                send(newSocket, buffer, 1024, 0);
                                recvd = recv(newSocket , buffer , strlen(buffer) ,0);
       //                         cout<<recvd;
                                break;
                            }
                        }
                    }
                    else
                        fseek(fp, 512*1024, SEEK_CUR);
                }
                memset(&buffer , '\0' , 1024);
                strcpy(buffer , "Done");
                send(newSocket, buffer, 1024, 0);
     //           cout<<i<<endl;
                
            }
            else
            {
                printf("Not opened file");
            }
    close(newSocket);
}


void give_map( int id , int newSocket )
{   
            //to transfer data between client and server
            char buffer[2048];
            //to store fullfile path corresponding to the hash recieved
            string fullFilePath;
            //to store hash recieved
            string hash_recieved ="";
            //to store the bitmap
            string to_hold_bit;
         
            memset(&buffer , '\0' , 2048);
            strcpy(buffer , "give_hash");
   //         cout<<buffer<<endl;
            send(newSocket , buffer, 2048 , 0);
            bzero(buffer , 2048);
            recv(newSocket , buffer , 2048 , 0);
   //         cout<<"Hash Recieved inside give pieces "<<endl;
   //         cout<<buffer<<endl;
            hash_recieved += buffer;
   //         cout<<hash_recieved<<endl;
            map < string , map< string , int > > :: iterator it ;
            map <string , int > ::iterator inner_it;  

            for(it = bitmap.begin() ; it != bitmap.end() ; it++)
            {
  //              cout<<it->first<<endl;
                if(strcmp(&hash_recieved[0] , &(it->first)[0]) == 0)
                {
                    for(inner_it = it->second.begin() ; inner_it != it->second.end() ; inner_it++ )
                    {
  //                      cout<<"Inside"<<endl;
                        to_hold_bit += inner_it->second+'0';
  //                      cout<<inner_it->second<<endl;
                    }
                }
            }

//            cout<<"Bitmap_manipulated"<<endl;
            int send_size = strlen(&to_hold_bit[0]);
//            cout<<"size"<<send_size<<endl;
            bzero(buffer , 1024);
            strcpy( buffer , &(to_string( send_size ) )[0] );
//            cout<<buffer<<endl;
            send(newSocket , buffer , 2048 , 0);

            bzero(buffer , 2048);
            strcpy(buffer , &(to_hold_bit)[0]);
//            cout<<buffer<<endl;
            send(newSocket , buffer , 2048 , 0);
            
//            cout<<"Bitmap_sent"<<endl;
}
                
void recieve_bitmap( int id , int newSocket)
{
    char buffer[2048];
    string final_bitmap="" ;
    string fullFilePath;
    string hash_recieved="";
    int recieving_size;
    
    recv(newSocket , buffer , strlen(buffer) , 0);
    recieving_size = atoi(buffer);

    strcpy(buffer , "give_hash");
    send(newSocket , buffer, 2048 , 0);
    
    bzero(buffer, 2048);

    recv(newSocket , buffer , 2048 , 0);
//    cout<<"Hash Recieved bit sent "<<endl;
//    cout<<buffer<<endl;

    hash_recieved +=buffer;
    //cout<<hash_recieved<<endl;
    
    fullFilePath = torrentpath[hash_recieved];
  //  cout<<fullFilePath<<endl;
    bzero(buffer ,  2048);  
    
    recv(newSocket , buffer , 2048 , 0);
    final_bitmap +=buffer;
//    cout<<final_bitmap<<endl;

    upload(final_bitmap , fullFilePath , newSocket);
    
}



void connect_client( string lis_port , string ip_recvd)
{
    string listen_port(lis_port);
    string ip(ip_recvd);

    int id = 0;
    int sockfd , ret ;
    struct sockaddr_in serverAddr;
    
    int newSocket;
    struct sockaddr_in newAddr;

    socklen_t addr_size;
    char buffer[2048];
    sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if(sockfd < 0)
    {
        printf("[-]Error in connection.\n");
        return;
    //    exit(1);
    }
  //  printf("[+]Server Socket created succesfully\n");

    memset(&serverAddr , '\0' , sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(&lis_port[0]) ); 
    serverAddr.sin_addr.s_addr = inet_addr(&ip[0]);

    ret = bind( sockfd , (struct sockaddr*)&serverAddr , sizeof(serverAddr));
    
    if(ret < 0)
    {
        printf("[-]Error in binding");
        exit(1);
    }

//    printf("[+]Bind to the Port.\n");

    if(listen(sockfd ,10) == 0 )
    {
//        printf("[+]Listening...");
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
          //  printf("Connection accepted from %s:%d\n",inet_ntoa(newAddr.sin_addr),ntohs(newAddr.sin_port));
            
            memset(&buffer , '\0' , strlen(buffer));

            recv(newSocket , buffer , 2048 , 0);
            
            if(strcmp(buffer , "give_piece" ) == 0)
            {    
        //        cout<<buffer<<endl;
                thread t1( give_map , id , newSocket );
                t1.detach();
            }
            else
            {
             //   cout<<buffer<<endl;
                thread t2(recieve_bitmap  , id , newSocket);
                t2.detach();
            }
        }
}


vector < string > tokenize( char buffer[] )
{
       // cout<<buffer<<endl;
		vector <string> tokenized_array;	
        char waste_array[50][500];
        int  arg_index = 0;
        int index = 0 ;
        char* token = strtok(buffer , ";");
        while( token != NULL )
        {
            strcpy( waste_array[index++] , token)  ;
            token = strtok(NULL,";");
        }

        for(int i = 0 ; i < index ; i++)
		{       
		    token = strtok(waste_array[i] , "@");
            while( token != NULL )
            {
                tokenized_array.push_back(token);
                token = strtok(NULL,"@");
      //          cout<<tokenized_array[i]<<endl;
               	token = strtok(NULL,"@");
            }
        }

return tokenized_array ;
}



void download( int num_times , int clientSocket , string dest_file)
{
    
    int count = 0 ;
    char buffer[1024];
    char fullFilePath[500];
    strcpy(fullFilePath , &dest_file[0]);
    //cout<<"d file "<<fullFilePath<<endl;

    memset(&buffer , '\0' , 1024);
    
    strcpy(buffer , "send");
    send(clientSocket , buffer , strlen(buffer) , 0);
  //  cout<<num_times<<" "<<endl;

    ofstream output;
    output.open(fullFilePath , ios::out | ios::app | ios::binary );
 
    for(int i = 0 ; i < num_times ; i++ )
    {
        for(int j = 0 ; j < 512 ; i++ )
            {
                memset(&buffer , '\0' , 1024);
                
                recv(clientSocket , buffer , 1024 , 0);
                
                if(strcmp(buffer ,"Done") == 0 )
                    break;
                
                output.write(buffer , 1024 );
            
                memset(&buffer , '\0' , 1024);
                
                strcpy(buffer , "recieved");
                
                send(clientSocket , buffer , 1024 , 0);
                count++;
            }
    }
    output.close();
    printf("[+]FILE_PATH_IN_CURRENT_CLIENT\n");

    vector <string> :: iterator it ;
    for(it = downloading.begin() ; it !=downloading.end() ;it++ )
    {
        if(*it == dest_file )
            downloading.erase(it , it+1);
    }
    downloaded.push_back(dest_file);
 
}

void send_bitmap(string rcv_bitmap , int clientSocket , string h , string dest_file )    
{   
    string hash = h;
    char buffer[2048];
    int sending_size = strlen(&rcv_bitmap[0]);
    strcpy(buffer, "send");
//    cout<<hash<<endl;
    send(clientSocket , buffer ,2048 , 0);
    
    send(clientSocket , buffer , strlen(buffer) , 0);
    strcpy(buffer , &to_string(strlen(&rcv_bitmap[0]))[0] );
    
    recv(clientSocket , buffer , strlen(buffer), 0 );
    if( strcmp(buffer , "give_hash")==0);
        {
            strcpy(buffer , &hash[0]);
//            cout<<buffer<<endl;
            send(clientSocket , buffer , strlen(buffer) , 0);
//            cout<<"hash sent "<<endl;
        }
    bzero(buffer , 2048);

//    cout<<rcv_bitmap<<endl;
    strcpy(buffer , &(rcv_bitmap)[0]);
    send(clientSocket , buffer , 2048 , 0);

    download( (strlen(&rcv_bitmap[0])) , clientSocket , dest_file);
}


void connect_server( string ip_tt , string hash , string dest_file )
{
    string ip_to_token(ip_tt);
    //cout<<ip_to_token<<endl;
    vector <string> ip_add (tokenize(&ip_to_token[0]));
    int m_clients = ip_add.size();
    //cout<<hash<<endl;
   
    string ip[m_clients][2] ;
   for(int i = 0 ;i < m_clients ;i++)
    {
            int arg_index=0;
            char* token = strtok(&ip_add[0][0] , ":");
            while( token != NULL )
            {
                strcpy( &ip[i][arg_index++][0] , token)  ;
                token = strtok(NULL,":");
            }
    }

//    cout<<m_clients<<endl;
    
    vector<string> rcv_bitmap;
     
    for(int i=0; i < m_clients ; i++)
    {
        int port = atoi(&ip[i][1][0]);

        int clientSocket , ret ;
        struct sockaddr_in serverAddr;
        char buffer[2048];
        pid_t childpid;

        clientSocket = socket(AF_INET , SOCK_STREAM , 0);
        if(clientSocket < 0)
        {
            printf("[-]Error in connection.\n");
            exit(1);
        }
//        printf("[+]Client Socket created succesfully\n");

        memset(&serverAddr , '\0' , sizeof(serverAddr));

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port); 
        serverAddr.sin_addr.s_addr = inet_addr(&ip[i][0][0]);

        ret = connect( clientSocket , (struct sockaddr*)&serverAddr , sizeof(serverAddr));
        if(ret < 0)
        {
            printf("[-]Error in connection");
            exit(1);
        }
//        printf("[+]Connected to the Server.\n");

        bzero(buffer  , 2048);
        strcpy(buffer , "give_piece");
        send(clientSocket , buffer , 2048 , 0);
//        cout<<buffer<<endl;
        
        recv(clientSocket , buffer , 2048 , 0);
        bzero(buffer , 2048);
        if(strcmp(buffer , "give_hash"))
           {
                strcpy( buffer , &hash[0] );
//                cout<<buffer<<endl;
                send(clientSocket , buffer , strlen(buffer) , 0);
           }
//        cout<<"Hash Sent "<<endl;
        int bit_map_size;
        string to_hold_bit="";
        recv(clientSocket , buffer , 2048 , 0);
//        cout<<buffer<<endl;
        bit_map_size = atoi(buffer);
       
//        cout<<bit_map_size<<endl;
       
            bzero(buffer , 2048);
            recv(clientSocket , buffer , 2048 , 0);
            to_hold_bit += buffer;
            //cout<<to_hold_bit<<endl;

        bzero(buffer , 2048);
//        cout<<"Bit Map recieved"<<endl;
        
//        cout<<to_hold_bit<<endl;

        rcv_bitmap.push_back(to_hold_bit );
        close(clientSocket);
    }


    vector <string> final_bitmap(return_bitmap(rcv_bitmap , m_clients ));

    thread t[m_clients];

//    cout<<m_clients<<endl;


    for(int i = 0 ; i < m_clients ; i++)
    {   
        int port = atoi(&ip[i][1][0]);
//        cout<<port<<endl;
//        cout<<ip[i][0]<<endl;
        int clientSocket , ret ;
        struct sockaddr_in serverAddr;
        char buffer[2048];
        pid_t childpid;

        clientSocket = socket(AF_INET , SOCK_STREAM , 0);
        if(clientSocket < 0)
        {
            printf("[-]Error in connection.\n");
            exit(1);
        }
//        printf("[+]Client Socket created succesfully\n");

        memset(&serverAddr , '\0' , sizeof(serverAddr));

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port); 
        serverAddr.sin_addr.s_addr = inet_addr(&ip[i][0][0]);

        ret = connect( clientSocket , (struct sockaddr*)&serverAddr , sizeof(serverAddr));
        if(ret < 0)
        {
 //           printf("[-]Error in connection");
            exit(1);
        }
//        printf("[+]Connected to the Server.\n");

         t[i] = thread(send_bitmap , rcv_bitmap[i] , clientSocket , hash , dest_file); 
    }  
    for(int i = 0 ; i < m_clients ;i++)
        t[i].join();
}



void show()
{
    vector <string>::iterator it ;

    if(downloading.size() == 0)
    {
        cout<<"DOWNLOADING : None"<<endl;
    }
    else
    {
        cout<<"DOWNLOADING : ";

        for(it = downloading.begin() ; it != downloading.end() ; it++)
        {
            cout<<"\t"<<*it<<" : [D]"<<endl;
        }
    }
    if(downloaded.size() == 0)
    {
        cout<<"DOWNLOADED : None"<<endl;
    }
    else
    {
        cout<<"DOWNLOADED : ";
    for(it = downloaded.begin() ; it != downloaded.end() ; it++)
        {
            cout<<"\t"<<*it<<" : [S]"<<endl;
        }
    }


}