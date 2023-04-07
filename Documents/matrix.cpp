/******************************************************************************
* FILE: matrix.cpp
* DESCRIPTION:
*   A Pthreads program which demonstrates one safe way
*   to read input from a text file and output to a new file.
* AUTHOR: Austin Smith
* LAST REVISED: 03/31/23
******************************************************************************/
#include <pthread.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
using namespace std;
// global variables 
vector<vector<int>> input;
vector<pair<int, int>> possible_max;
int num_threads = 0;
int max_row = -1;
int max_sum = -1;
pthread_mutex_t mutex;

void *sum_row(void *threadid){
   // get task id for thread
   int *id_ptr, taskid;
   id_ptr = (int *) threadid;
   taskid = *id_ptr;
   
   // define start/end parameters, based on idea of
   // splitting up the work based on number of threads
   int starting_row = taskid * (input.size() / num_threads);
   int ending_row = (taskid == num_threads - 1) ? input.size() : (taskid + 1) * (input.size() / num_threads);
   // get sum of current row
   for(int i = starting_row; i < ending_row; i++){
   	int current_sum = 0; // initialize current_sum to zero at start of each new iteration
   	for(int j = 0; j < input[i].size(); j++){
   		current_sum += input[i][j]; // increment by each input value
   	}
   	pair<int, int> row_sum;
   	row_sum.first = i;
   	row_sum.second = current_sum;
   	// lock mutex for accessing global variables
   	pthread_mutex_lock(&mutex);
   	possible_max.push_back(row_sum);
   	// unlock mutex
   	pthread_mutex_unlock(&mutex);
   }
   // exit the thread
   return NULL;
}

int main(int argc, char *argv[]){
   string file_name;
if (argc > 1){
   num_threads = stoi(argv[1]); // define number of threads as first CLI argument
   file_name = argv[2]; // define file name as second CLI argument
   // Read the input file
   ifstream file(file_name);
   // Determine numer of rows in matrix
   int num_rows;
   file >> num_rows;
   // Resize the vector based on this value
   // Resizing got rid of segmentation fault error
   input.resize(num_rows);
   for(int i = 0; i < num_rows; i++){
   	input[i].resize(num_rows);
   	// read in value from file into vector<vector<int>>
   	for(int j = 0; j < num_rows; j++){
   		file >> input[i][j];
   	}
   }
   // close the file
   file.close();
   
   // initialize mutex
   pthread_mutex_init(&mutex,NULL);
   
   // create the worker threads
   pthread_t threads[num_threads];
   int taskids[num_threads];
   for(int t = 0; t < num_threads; t++){
   	taskids[t] = t;
   	if(pthread_create(&threads[t], NULL, sum_row, (void *)&taskids[t])){
   		cout << "error creating thread" << endl;
   		return -1;
   	}
   }
   
   // wait for threads to finish
   for(int t = 0; t < num_threads;t++){
	   if(pthread_join(threads[t],NULL)){
	    	cout << "error joining threads." << endl;
	  	exit (-1); 
	   }
   }
   for(pair<int, int> max : possible_max){
   	if(max.second > max_sum){
   		max_sum = max.second;
   	}
   }
   // attempting to account for multiple max values
   vector<pair<int, int>> multiple_maxes;
   for(int i = 0; i < possible_max.size(); i++){
   	if(possible_max[i].second == max_sum){
   		multiple_maxes.push_back(possible_max[i]);
   	}
   }
   // write the max_sum and the max_row to output txt file
   ofstream out_file("output.txt");
   out_file << "Maximum Row(s) and value: " << "\n";
   for(int i = 0; i < multiple_maxes.size(); i++){
   	out_file << "Row : " << multiple_maxes[i].first << " Value: " << multiple_maxes[i].second << "\n";
   }
   out_file.close();
   
   pthread_exit(NULL);
   }
}

