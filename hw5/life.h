#ifndef _life_h
#define _life_h

#include <pthread.h>

pthread_mutex_t global_lock;
pthread_barrier_t barr;

struct partition
{
	char* inboard;
	int nrows;
	int ncols;
	int gens_max;
	int thread_id;
	int i_start;
	int i_end;
	int j_start;
	int j_end;
	unsigned char *boardMemory;
	unsigned char *tmpMemory;
};
/**
 * Given the initial board state in inboard and the board dimensions
 * nrows by ncols, evolve the board state gens_max times by alternating
 * ("ping-ponging") between outboard and inboard.  Return a pointer to 
 * the final board; that pointer will be equal either to outboard or to
 * inboard (but you don't know which).
 */
char*
game_of_life (char* outboard, 
	      char* inboard,
	      const int nrows,
	      const int ncols,
	      const int gens_max);

/**
 * Same output as game_of_life() above, except this is not
 * parallelized.  Useful for checking output.
 */
char*
sequential_game_of_life (char* outboard, 
			 char* inboard,
			 const int nrows,
			 const int ncols,
			 const int gens_max);

char*
parallel_game_of_life (char* outboard, 
			 char* inboard,
			 const int nrows,
			 const int ncols,
			 const int gens_max);

void *initialize_map_parallel (void *partition_data);

void *parallel_streams (void *partition_data);

#endif /* _life_h */
