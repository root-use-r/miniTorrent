#include <cstring>
#include <iostream>

using namespace std ;


void formatting( char * rcv_sha1 , string ip , int port , char * file );
void write_to_log( string );
void read_to_log(string );
void update_data( string sha1 , string ip_port );
void display();
string get_data( string sha1 );
string find_src_file(string sent_src_directory);
int remove_data(string hash , string ip_port);
void formatting( char * rcv_sha1 , string ip_port , int port , char * file );
void remove_formatting( char * rcv_sha1 , string ip_port , int port  );
string remove_find_src_file(string sent_src_directory);
