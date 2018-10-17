#include <iostream>
#include <vector>
#include <utility>
#include <cstring>

using namespace std; 

void ppc( vector <string> str , int m_client , int n_piece , int count[] );
void display(vector < pair<int , int > > &cal_size , int n );
void insert ( string bitmap , int n , int client );
void calculate_size(int n , vector < pair<int, int> > &cal_size);
void main_algo( vector < pair <int , int> > &cal_size , int n_piece , int m_client , int count[] );
void average( int n_piece , int m_client , int count[] );
vector <string> return_bitmap( vector <string> &str ,int m_client );
