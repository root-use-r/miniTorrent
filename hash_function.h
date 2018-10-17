#include <iostream>
#include <cstring>

using namespace std;

int hash_function( char *fullFilePath , char *filename , unsigned char *tracker_buffer);
void calculate_hash(unsigned char *input_buffer , unsigned long size );
string get_from_torrent(char *FilePath);
void display_bitmap(string FilePath);
string find_src_file(string sent_src_directory);

