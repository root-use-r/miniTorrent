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
#include <list>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

 

vector <int> *details ;
vector <int> *request ;

/*char * return_req_p(int m_client , int n_piece )
{
	return request_pieces(m_client , n_piece);
}
*/




void ppc( vector<string> str , int m_client , int n_piece , int count[] )
{
	for(int i=0 ; i < m_client ; i++ )
	{	
		for(int j = 0 ; j < n_piece ; j++)
		{
			if(str[i][j]=='1')
				count[i]++;
		}
	}
}



void display(vector < pair<int , int > > &cal_size , int n )
{
	vector <pair <int, int> >::iterator i;
	for( i = cal_size.begin() ; i != cal_size.end() ; i++)
		cout<<i->first<<" "<<i->second<<endl;
}

void insert (string bitmap , int n , int client )
{
	for(int i = 0 ; i < n ; i++)
	{
		if( bitmap[i] == '1' )
			details[i].push_back(client);
	}
}

void calculate_size(int n , vector < pair<int, int> > &cal_size)
{

	vector <pair <int, int> >::iterator i;
	for(int i= 0 ; i <n ; i++)
	{
		cal_size.push_back(make_pair( details[i].size() , i ) );
	}
	sort(cal_size.begin() , cal_size.end() );
}

void main_algo( vector < pair <int , int> > &cal_size , int n_piece , int m_client , int count[] )
{
	vector <pair < int , int > >::iterator it ;
	vector <int> :: iterator vec_it ;
	int cl_num;
	int client_num[m_client];
	int client_index ;
	for(it = cal_size.begin() ; it != cal_size.end() ; it++ )
	{
		client_index= 0;
		if(it->first == 1)
		{
			//client number having 1 piece
			cl_num = details[it->second].front();
			//add 1 to that client's pieces vector
			request[it->second].push_back(cl_num);
			//cout<<request[it->second].back()<<" "<<it->second<<endl;
		//	count[cl_num]--;
				
		}
		else if( it->first > 1 )
		{
			client_index= 0;
			for(vec_it = details[it->second].begin() ; vec_it!= details[it->second].end() ; vec_it++ )
				{
					//list of all clients having that piece
					client_num[client_index++] = *vec_it;
				}
				//to find the min value of pieces among the clients
				
				cl_num = client_num[0];
//				cout<<"cl "<<cl_num<<endl;

				int min = count[cl_num];
//				cout<<"min "<<min<<endl;

				for(int i = 1 ; i < client_index ; i++)
				{
					if( count[client_num[i]] <= min && count[client_num[i]] > 0)
						{
							min = count[client_num[i]];
							cl_num = client_num[i];
						}
				}
//								cout<<"cl "<<cl_num<<endl;
				request[it->second].push_back(cl_num);
				count[cl_num]--;
				//cout<<count[cl_num]<<endl;
				//cout<<request[it->second].front()<<" "<<it->second<<endl;
					
//				cout<<endl;
		} 
	}


/*	for(int i = 0  ; i < 3 ; i ++)
	{
		for(vec_it = request[i].begin() ; vec_it != request[i].end() ; vec_it++)
			cout<<*vec_it<<"1";
		cout<<endl;
	}*/
}

void average( int n_piece , int m_client , int count[] )
{
	int avg = n_piece / m_client + 1;
	for( int i=0 ; i < m_client ; i++)
	{
		if( avg < count[i] )
			count[i] = avg ;
	}
	
	int sum = 0;
	for(int i = 0 ; i < m_client ; i++ )
		sum +=count[i];
	
	if(sum > n_piece )
	{
		int max = 0 ;
		while(sum != n_piece)
		{ 
			//cout<<"sum "<<sum<<endl;
			int i = 1;
			while(i < m_client)
				{
					max = (count[max] > count[i])?max:i;
					i++;
				}
			count[max]--;
			sum = 0;
			for(int j = 0 ; j < m_client ; j++ )
				sum +=count[j];
		}
	}

/*	for(int j = 0 ; j < m_client ; j++ )
		cout<<count[j]<<" ";
*/
}

vector <string> return_bitmap( vector <string> &str  , int m_client)
{
	//calculated total number of clients having corresponding pieces
	
	vector < pair<int , int> > cal_size;
	int n_piece = str[0].size();
//	cout<<"piece "<<n_piece<<endl;
	details = new vector<int>[n_piece ];
	request = new vector<int>[n_piece ];

	for(int i = 0 ; i < m_client ; i++)
		insert(str[i] , n_piece , i);
	
	calculate_size(n_piece , cal_size);

	int count[m_client]={0} ;
	ppc(str , m_client , n_piece , count);
	average(n_piece , m_client , count);
	main_algo(cal_size , n_piece , m_client ,count);


	vector <string> piece_req;
	char temp_pr[m_client][n_piece+1];
	int i  ,j ;
	for( i = 0 ; i < m_client ; i++ )
		{
			for( j=0 ; j < n_piece ; j++)
				temp_pr[i][j]='0';
			temp_pr[i][j]='\0';
		}	
	for(int i = 0 ; i < n_piece ; i++ )
		{
			temp_pr[request[i].front()][i]='1';
		}

	for(int i = 0 ; i < n_piece ; i++)
		piece_req.push_back(temp_pr[i]);

/*	for(int i = 0 ; i < m_client ; i++ )
		cout<<piece_req[i]<<endl;
*/
//	cout<<piece_req.size()<<endl;
	return piece_req;

}




