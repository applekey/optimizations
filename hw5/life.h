#ifndef _life_h
#define _life_h

#include <pthread.h>

// We use a global lock for all the critial regions, assuming the grid size is big,
// the area of the critical region is very small compare to the area of the total grid
// A global lock allows for very simple implementation. No worries on possible deadlocks
pthread_mutex_t global_lock;

// A pthread barrier to make sure that multiple threads complete the generation before starting
// the next one
pthread_barrier_t barr;

// data struct passed into parallel streams
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

// Initialize a bit map of the grip, each cell is one byte
// The least significant bit stores whether the cell is alive or dead
// The next 3 bits stores the amount of alive neignbouring cells
// We read this map sequentially to determine whether the current cell will live or die
// The map is only initialized once
void *initialize_map_parallel (void *partition_data);

// Parallel streams of the game of life
// Using the map that we just created, we dont need to access all eight neighbours of a cell
// We just read the amount of alive neighbours from the map
// When a cell needs to be updated, we also need to update its neighbours
// We will try to lock if the cell is in a critical region
void *parallel_streams (void *partition_data);

#endif /* _life_h */
