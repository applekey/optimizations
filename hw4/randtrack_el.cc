
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#include "defs.h"
#include "hash_list_lock.h"

#define SAMPLES_TO_COLLECT   10000000
#define RAND_NUM_UPPER_BOUND   100000
#define NUM_SEED_STREAMS            4

/* 
 * ECE454 Students: 
 * Please fill in the following team struct 
 */
team_t team = {
    "pokemon",                  /* Team name */

    "Chen Hao Zhang",                    /* First member full name */
    "999272228",                 /* First member student number */
    "chenhao.zhang@mail.utoronto.ca",                 /* First member email address */

    "Vincent Chen",                           /* Second member full name */
    "996952114",                           /* Second member student number */
    "vincenttt.chen@mail.utoronto.ca"                            /* Second member email address */
};

/**********************************************************************************
We use what we build earlier from list_lock. There are two layer locks, the first
layer is element_lock, the second layer is list_lock. We have one lock for every key.
We defined the locks as an array of size RAND_NUM_UPPER_BOUND.

Before we look up a key, we lock the element_lock, if the key does not exist, then
we lock the list before inserting it. We lock the list to make sure that race does
not happen.
**********************************************************************************/

unsigned num_threads;
unsigned samples_to_skip;
pthread_mutex_t element_lock[RAND_NUM_UPPER_BOUND]; 

class sample;

class sample {
  unsigned my_key;
 public:
  sample *next;
  unsigned count;
  pthread_mutex_t lock;

  sample(unsigned the_key){
    pthread_mutex_init(&lock, NULL);
    my_key = the_key; count = 0;
  };
  unsigned key(){return my_key;}
  void print(FILE *f){printf("%d %d\n",my_key,count);}
  
  // ~sample()
  // {
  //   delete lock;
  // }
};

// This instantiates an empty hash table
// it is a C++ template, which means we define the types for
// the element and key value here: element is "class sample" and
// key value is "unsigned".  
hash<sample,unsigned> h;


void *parallel_streams (void *counter){
  int upperbound, lowerbound;
  int i, j, k, rnum;
  unsigned key;
  sample *s;
  upperbound = (*(int*)counter+1) * (NUM_SEED_STREAMS/num_threads);
  lowerbound = (*(int*)counter) * (NUM_SEED_STREAMS/num_threads);
  //printf("in thread thread number %d, upperbound %d, lowerbound %d\n", *(int*)counter, upperbound, lowerbound);
  
  for (i = lowerbound; i < upperbound; i++) {
    rnum = i;
    // collect a number of samples
    for (j=0; j<SAMPLES_TO_COLLECT; j++){

      // skip a number of samples
      for (k=0; k<samples_to_skip; k++){
       rnum = rand_r((unsigned int*)&rnum);
      }

      // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
      key = rnum % RAND_NUM_UPPER_BOUND;
      pthread_mutex_lock (&element_lock[key]); //lock element_lock
      // if this sample has not been counted before
      if (!(s = h.lookup(key))){
         
      // insert a new element for it into the hash table
      // Key does not exist in hash, we need to insert new key, so we lock the list to avoid possible races
        s = new sample(key);
        h.lock_list(key); // Lock list
        h.insert(s);
        h.unlock_list(key);// Unlock list
      }
      
      // increment the count for the sample
      s->count++;
      pthread_mutex_unlock (&element_lock[key]);
    }
  } 
}

int  
main (int argc, char* argv[]){
  int i,j,k;
  int *rnum;
  unsigned key;
  sample *s;
  pthread_t *threads;
  pthread_attr_t attr;

  // Print out team information
  printf( "Team Name: %s\n", team.team );
  printf( "\n" );
  printf( "Student 1 Name: %s\n", team.name1 );
  printf( "Student 1 Student Number: %s\n", team.number1 );
  printf( "Student 1 Email: %s\n", team.email1 );
  printf( "\n" );
  printf( "Student 2 Name: %s\n", team.name2 );
  printf( "Student 2 Student Number: %s\n", team.number2 );
  printf( "Student 2 Email: %s\n", team.email2 );
  printf( "\n" );

  // Parse program arguments
  if (argc != 3){
    printf("Usage: %s <num_threads> <samples_to_skip>\n", argv[0]);
    exit(1);  
  }
  sscanf(argv[1], " %d", &num_threads); // not used in this single-threaded version
  sscanf(argv[2], " %d", &samples_to_skip);

  for (i = 0; i < RAND_NUM_UPPER_BOUND; ++i) {
    pthread_mutex_init(&element_lock[i], NULL);
  }

  // initialize a 16K-entry (2**14) hash of empty lists
  h.setup(14);
  threads = (pthread_t *) malloc (num_threads * sizeof(pthread_t));
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  rnum = (int *) malloc (num_threads * sizeof(int));
  // process streams starting with different initial numbers
  
  for (i=0; i<num_threads; i++){
    rnum[i] = i;
    pthread_create (&threads[i], NULL, parallel_streams, (void *)&(rnum[i]));
  }
  
  for (i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }
  pthread_attr_destroy(&attr);
  // print a list of the frequency of all samples
  h.print();
  pthread_exit(NULL);
}
