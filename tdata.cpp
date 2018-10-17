#include <stdio.h>
#include <iostream>
#include <cstring>
#include <map>
#include <vector>
#include <string>
#include <fstream>

using namespace std ;

extern map < string , string > tracker_data ;
extern string file;

void display()
{
    map <string ,string >:: iterator it;

    for( it = tracker_data.begin() ; it != tracker_data.end() ; it++ )
	    {
	    	cout<<it->first<<endl;
	    	cout<<it->second<<endl;
        }
}

void read_to_log(string file)
{
  ifstream infile;
  infile.open(file, ios::in );
  string sha1 , rest;
	if(infile)
	{
		while (getline(infile, sha1)) 
		{
		getline(infile , rest );
		tracker_data[sha1]=rest;
		}
	}
    
    infile.close();
}

void write_to_log(string file)
{
		FILE *fp;
	    fp =fopen( &file[0] ,"w");
	    
	    map <string , string > :: iterator it;
	    for( it = tracker_data.begin() ; it != tracker_data.end() ; it++ )
	    {
        	fprintf( fp, "%s\n", &(it->first)[0] );
            fprintf( fp, "%s\n", &(it->second)[0] );
        }
	    fclose(fp);
}

void update_data( string sha1 , string ip_port )
{

    read_to_log(file);
    map <string , string > ::iterator it;
    int flag = 0 ;
    it = tracker_data.find(sha1);
    it = tracker_data.find(&sha1[0]);
    if( it != tracker_data.end())
    {
		char tokenized_array[500][500];
	    int  arg_index = 0; 
	    char  temp[1000];
	    strcpy(temp ,&(it->second)[0] );
	    char* token = strtok( temp , ";");

	    while( token != NULL)
	    {
	        strcpy( tokenized_array[arg_index] , token) ;
	        strcat(tokenized_array[arg_index++] , ";");
		    token = strtok(NULL,";");
	    }

	    for(int i = 0 ; i < arg_index ; i++)
	    	{
	    		if( strcmp( &ip_port[0] , tokenized_array[i]) == 0 )
	    		{
//	    			cout<<"Already Present\n";
	    			flag = 1; 
	    		}
	    	}
	    if(flag == 0)
		    it->second.append(&ip_port[0]);
	}
	else
	{
//		cout<<"Inserted\n ";
		tracker_data[sha1] = ip_port;	
	}
	write_to_log(file);
}




void formatting( char * rcv_sha1 , string ip_port , int port , char * file )
{
	string filepath(file);
    string sha1(rcv_sha1);
    ip_port.append(":");
    ip_port.append(to_string(port));
    ip_port.append("@");
    ip_port.append(filepath);
    ip_port.append(";");
    update_data(sha1 , ip_port );
}

string get_data(string sha1)
{
	read_to_log(file);
    map <string , string > ::iterator it;
    
    it = tracker_data.find(sha1);
    //it = tracker_data.find(&sha1[0]);
    //cout<<sha1<<endl;
    if(it != tracker_data.end())
    {
    	return it->second;
    }
    else
    {
    	return "NULL";
    }
}

int remove_data(string hash , string ip_port)
{
	read_to_log(file);
	map <string , string > ::iterator it;
   
    int found = 0 ;
    it = tracker_data.find(hash);
    if( it != tracker_data.end())
    {
		char tokenized_array[500][500];
	    int  arg_index = 0; 
	    char  temp[10000];
	    strcpy(temp ,&(it->second)[0] );
	    char* token = strtok( temp , ";");

	    while( token != NULL)
	    {
	        strcpy( tokenized_array[arg_index] , token) ;
	        strcat(tokenized_array[arg_index++] , ";");
		    token = strtok(NULL,";");
	    }
	    it->second="";
	    for(int i = 0 ; i < arg_index ; i++)
	    	{
//	    			    			cout<<ip_port<<endl<<tokenized_array[i]<<endl;
	    		if( strcmp( &ip_port[0] , tokenized_array[i]) == 0 )
	    		{
//	    			cout<<ip_port<<endl<<tokenized_array[i]<<endl;
	    			cout<<"[+]Removed Succesfully"<<endl;
	    			found = 1 ; 
	    		}
	    		else
	    			it->second.append(tokenized_array[i]);
			}
	}
	else
	{
		cout<<"[-]Data Not Found"<<endl;
	}
	write_to_log(file);
    //it = tracker_data.find(&hash[0]);
}


void remove_formatting( char * rcv_sha1 , string ip_port , int port )
{
    string sha1(rcv_sha1);
    ip_port.append(":");
    ip_port.append(to_string(port));

    int a =remove_data(sha1 , ip_port );
    //cout<<"Sent to remove_data"<<endl;
//    cout<<a<<endl;
}

string find_src_file(string sent_src_directory) //Returns file name i.e. file name after '/'
{
	int len_sent_src_file ,index_of , dot;
	string src_file;
	
	len_sent_src_file = strlen(&sent_src_directory[0]);
	index_of = sent_src_directory.find_last_of('/');
	src_file = sent_src_directory.substr(index_of + 1,len_sent_src_file);
	
	return src_file;
}

string remove_find_src_file(string sent_src_directory) //Returns file name i.e. file name after '/'
{
	int len_sent_src_file ,index_of , dot;
	string src_file;
	
	len_sent_src_file = strlen(&sent_src_directory[0]);
	index_of = sent_src_directory.find_last_of('/');
	dot = sent_src_directory.find_last_of('.');
	src_file = sent_src_directory.substr(index_of + 1,dot);
	
	return src_file;
}