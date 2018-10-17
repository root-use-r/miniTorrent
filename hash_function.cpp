#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <map>
#include <vector>

using namespace std;

extern map < string , map< string , int > > bitmap;
extern map < string , string > torrentpath;


/*	void display_bitmap()
	{
		map < string ,map< string ,int > >:: iterator out_it;
		map <string , int > :: iterator in_it;

		for( out_it = bitmap.begin() ; out_it != bitmap.end() ; out_it++)
		{
			cout<<out_it->first<<endl;
			for( in_it = out_it->second.begin() ; in_it != out_it->second.end() ;in_it++ )
			{
				cout<<in_it->first<<" ";
				cout<<in_it->second<<endl;
			}
		}
	}
*/

string find_src_file(string sent_src_directory) //Returns file name i.e. file name after '/'
{
	int len_sent_src_file ,index_of ;
	string src_file;
	
	len_sent_src_file = strlen(&sent_src_directory[0]);
	index_of = sent_src_directory.find_last_of('/');
	
	src_file = sent_src_directory.substr(index_of + 1,len_sent_src_file);
	
	return src_file;
}

void calculate_hash(unsigned char *input_buffer , unsigned long size )
{
	unsigned char output_buffer[50];
	SHA1(input_buffer, size, output_buffer);

	for(int i = 0 ; i  < 20 ; i++)
	{
		input_buffer[i] = output_buffer[i];
	}	

}


int hash_function( char *FilePath, char *filename ,unsigned char *tracker_buffer)
 {
 		char fullFilePath[500] ;
	 	strcpy(fullFilePath , FilePath);
	 	unsigned long chunkSize = 524288;
		unsigned long size ;
		
	 	struct stat stat_structure;
		if(stat(fullFilePath , &stat_structure) == 0 )
			size = stat_structure.st_size;
		

 		unsigned long remainder = size%chunkSize;
		int quotient  = size/chunkSize;
		int counter = 1;
		int index = 0 ;

        string fulltorrent;
		FILE *fp;
		map <string , int > temp_bit;

		ifstream fileStream;

		unsigned char *buffer = new unsigned char[chunkSize];
        unsigned char *temp_buffer = new unsigned char[50];
        unsigned char *to_store_hash_buffer = new unsigned char[512*1024];
        unsigned char *temp_to_store_hash_buffer = new unsigned char[50];
        
		fileStream.open(fullFilePath, ios::in | ios::binary);
		
		if (fileStream.is_open()) 
		{
	 		
	 		ofstream output;
	 		
			while (!fileStream.eof()) {
	 			
	 			fulltorrent.clear();
//         		fulltorrent.append("/home/rootuser/Desktop/");
		        fulltorrent.append(filename);

		        fp =fopen( fulltorrent.c_str() ,"a");

	            if (fp != NULL) 
	            { 
	            
	                fileStream.read((char *)buffer,chunkSize);
	            	
					if(counter > quotient)
						{
							calculate_hash(buffer, remainder-1 );
						}
					else
						{
							calculate_hash(buffer, strlen((char *)buffer));
						}
					
					for (int k = 0; k < 10; k++) 
      					  {
      					  	fprintf(fp ,"%02x", buffer[k]);
      					  	sprintf( (char *)&tracker_buffer[index*2] ,"%02x", buffer[k] );
      					  	sprintf( (char *)&temp_buffer[k*2] ,"%02x", buffer[k] );
      					  	sprintf( (char *)&to_store_hash_buffer[index*2] ,"%02x", buffer[k] );
      					  	index++;
      					  }

      					  //to store 20byte hash in bitmap 
					string temp_hash((char *)temp_buffer);
					//temp_bitmap to store in bitmap 
					temp_bit[temp_hash] = 1;
   					fclose(fp);
	                counter++;
	            }
	        }

	        delete(buffer);
	        fileStream.close();
	    }
	    else 
	    	{ 
	    		cout << "[-]FILE_NOT_FOUND\n" << endl; 
	    		return -1;
	    	}

		fp =fopen( fulltorrent.c_str() ,"a");
		if(fp != NULL)
		{
			fprintf(fp, "\n" );
			fprintf(fp ,"127.0.0.1:8888\n");
			fprintf(fp , "< TRACKER URL2 >\n");
			fprintf(fp, "%s\n",fullFilePath );
			fprintf(fp, "%lu\n",size );
		}
		fclose(fp);

	//storing filepath and mtorrent corresponding to it;

	//calculate hash again to store it in the map of bitmap 
	calculate_hash(to_store_hash_buffer , 2*index);

	//storing in hex
	for (int k = 0; k < 10; k++) 
	  {
	  	sprintf( (char *)&temp_to_store_hash_buffer[k*2] ,"%02x", to_store_hash_buffer[k] );
	  }
	//convert unsigned char * to string (hex)
	string to_store_hbuffer((char *)temp_to_store_hash_buffer);

	//storing full file path in bitmap map 
	string to_store_FilePath(FilePath);

	//storing everything in bitmap
	bitmap.insert( make_pair(to_store_hbuffer , temp_bit ) );

	//storing full file path corressponding to the hash
	torrentpath[to_store_hbuffer] = to_store_FilePath;

	delete(temp_buffer);
    delete(to_store_hash_buffer);
    delete(temp_to_store_hash_buffer);
  
    //return size of the string generated. twice because at one time 2bytes written in hex 
	return index*2;
	}

	//used in get command
	string get_from_torrent(char *FilePath)
	{
		char fullFilePath[500] ;
		unsigned char buffer[5000];
		char to_tracker_buffer[5000];
		int index=0;

	 	strcpy(fullFilePath , FilePath);

	 	ifstream infile(fullFilePath);
  		string sha1 ;
		if(infile)
		{
				getline(infile , sha1 );
		}
    
	    infile.close();

		unsigned long size = strlen( &sha1[0] );
		strcpy( (char *)buffer , &sha1[0] );
		calculate_hash( (unsigned char *)buffer , size );
		for(int k = 0 ;k < 10 ; k++ )
            {
                sprintf( &to_tracker_buffer[index*2] ,"%02x", buffer[k] );
                index++;
            }
       	string to_return(to_tracker_buffer) ;
        return to_return;
	}
