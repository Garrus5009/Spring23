/******************************************************************************
* FILE: atm.cpp
* DESCRIPTION:
*   A Pthreads program which demonstrates the use of mutexes
*   to read input from a text file and safely perform operations
*   on global variables and data structure.
* AUTHOR: Austin Smith
* LAST REVISED: 03/31/23
******************************************************************************/
#include <pthread.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>
#include <string>
using namespace std;
// global variables 
queue<string> transactions;
vector<int> query;
int num_transactions, balance;
int num_threads = 3;
// create mutex variable
pthread_mutex_t mutex;
/* In this function, we can have a thread perform one of 3 transactions:
*  1. Query
*  2. Deposit
*  3. Withdrawal
*  Read the transaction from the vector (or queue?) and perform the 
*  transaction designated by the letter at the start of the string
*/
void *make_transaction(void *threadid){
   // Define tasks for each string in transactions queue
   // start by locking mutex
   string transaction_text;
   int transaction_amt = 0;
   pthread_mutex_lock(&mutex);
   if(!transactions.empty()){
   	if(transactions.front()[0] == 'D'){
   		// increment current balance for deposit
   		transaction_text = transactions.front(); // get amount
   		transactions.pop(); // remove front element from queue
   		transaction_amt = stoi(transaction_text.substr(1)); // get integer amt
   		balance += transaction_amt; // increment
   	}else if(transactions.front()[0] == 'W'){
   		// decrement from current balance for withdrawals
   		transaction_text = transactions.front(); // get amount
   		transactions.pop(); // remove front element from queue
   		transaction_amt = stoi(transaction_text.substr(1)); // get integer amt
   		balance -= transaction_amt; // decrement
   	}else if(transactions.front()[0] == 'Q'){
   		// console output of balance from query transaction
   		cout << "Current Balance: $" << balance << ".00" << endl;
   		query.push_back(balance);
   		transactions.pop(); // remove front element from queue
   	}
   }
   // unlock mutex
   pthread_mutex_unlock(&mutex);
   pthread_exit(NULL);
}

int main(int argc, char *argv[]){
   string file_name;

   file_name = argv[1]; // define file name as second CLI argument
   // Read the input file
   ifstream file(file_name);
   // Determine numer of rows in matrix
   file >> num_transactions;
   
   // initialize mutex
   pthread_mutex_init(&mutex,NULL);
   // create array of threads
   pthread_t threads[num_threads];
   int taskids[num_threads];
   /*
   *  Read in each line from the file into a string vector
   */
   string from_file;
   for(int i = 0; i < num_transactions; i++){
   	file >> from_file;
   	transactions.push(from_file);
   }
   while(!transactions.empty()){
	   for(int i = 0; i < num_threads; i++){
	   	taskids[i] = i;
	   	if(pthread_create(&threads[i], NULL, make_transaction, (void *)&taskids[i])){
	   		cout << "error creating thread" << endl;
	   		return -1;
	   	}
	   }
   }
   // close the file
   file.close();
   
   // wait for threads to finish
   for(int t = 0; t < num_threads;t++){
	   if(pthread_join(threads[t],NULL)){
	    	cout << "error joining threads." << endl;
	  	exit (-1); 
	   }
   }
   /* output final balance to check against visual estimate
   *  output method was not specified for this assignment,
   *  so I'm including both console output as well as file
   *  output, to cover my bases.
   */
   ofstream out_file("balance.txt");
   for(int x : query){
   	out_file << "Current Balance: $" << x << ".00\n";
   }
   out_file.close();
   
   pthread_exit(NULL);
}
