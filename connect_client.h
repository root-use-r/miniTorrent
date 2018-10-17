#include <iostream>
#include <vector>
#include <cstring>
#include <map>
using namespace std;


extern map < string , map< string , int > > bitmap;

void connect_client( string listen_port , string ip);
vector < string > tokenize( char buffer[] );
void upload(string final_bitmap , string filepath , int newSocket);
void give_map( int id , int newSocket );
void recieve_bitmap( int id , int newSocket);
void download( int num_times , int clientSocket, string dest_file);
void send_bitmap(string rcv_bitmap , int clientSocket , string hash , string dest_file);
void connect_server( string ip_add ,  string hash , string dest_file);
void show();