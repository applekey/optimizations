/*****************************************************************************
 * life.c
 * The original sequential implementation resides here.
 * Do not modify this file, but you are encouraged to borrow from it

 This file also contains the parallel implentation
 ****************************************************************************/
#include "life.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
 #include <pthread.h>

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0) 

/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
#define SWAP_BOARDS( b1, b2 )  do { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)

#define BOARD( __board, __i, __j )  (__board[LDA*(__i) + (__j)])
//define BOARD( __board, __i, __j )  (__board[(__i) + LDA*(__j)])


#define NUM_THREADS 4

void *initialize_map_parallel (void *partition_data) {
    // See header file for more details
    // We are reading each cells and its surrounding neighbours and storing it into a map
    struct partition *board_data = (struct partition *)partition_data;
    int i, j;
    char *inboard = board_data->inboard;
    int i_start, i_end, j_start, j_end;
    i_start = board_data->i_start;
    i_end = board_data->i_end;
    j_start = board_data->j_start;
    j_end = board_data->j_end;
    int nrows = board_data->nrows;
    int ncols = board_data->ncols;
    unsigned char *boardMemory = board_data->boardMemory;
    const int LDA = ncols;
     for (i = i_start; i < i_end; i++)
      {
          for (j = j_start; j < j_end; j++)
          {
               int inorth = mod (i-1, nrows);
               int isouth = mod (i+1, nrows);
               int jwest = mod (j-1, ncols);
               int jeast = mod (j+1, ncols);

            unsigned char neighbor_count = 
                  (BOARD (inboard, inorth, jwest) & 0x1) + 
                  (BOARD (inboard, inorth, j) & 0x1)+ 
                  (BOARD (inboard, inorth, jeast) & 0x1)+ 
                  (BOARD (inboard, i, jwest) & 0x1)+
                  (BOARD (inboard, i, jeast) & 0x1)+ 
                  (BOARD (inboard, isouth, jwest) & 0x1)+
                  (BOARD (inboard, isouth, j) & 0x1)+ 
                  (BOARD (inboard, isouth, jeast)& 0x1);

           unsigned char alive = BOARD (inboard, i, j) & 0x1;
      
           boardMemory[i*ncols+j] = (neighbor_count <<1) + alive; // store the alive bit as well as the neightbourhood count

          }
      }
      return NULL;
}

void *parallel_streams (void *partition_data) {
    //See header for more details
    struct partition *board_data = (struct partition *)partition_data;
    int curgen, i, j;
    int gens_max = board_data->gens_max;
    int thread_id = board_data->thread_id;
    char *inboard = board_data->inboard;
    unsigned char *boardMemory = board_data->boardMemory;
    unsigned char *tmpMemory = board_data->tmpMemory;
    int nrows = board_data->nrows;
    int ncols = board_data->ncols;
    int i_start, i_end, j_start, j_end;
    i_start = board_data->i_start;
    i_end = board_data->i_end;
    j_start = board_data->j_start;
    j_end = board_data->j_end;
    unsigned int partition_size = (i_end - i_start) *(j_end - j_start);
    const int LDA = ncols;
    
    for (curgen = 0; curgen < gens_max; curgen++)
    {
        int err = pthread_barrier_wait(&barr);
         if(err != 0 && err != PTHREAD_BARRIER_SERIAL_THREAD) {
          printf("Could not wait on barr\n");
          exit(-1);}

        memmove(&tmpMemory[i_start*ncols+j_start], &boardMemory[i_start*ncols + j_start], partition_size);

          err = pthread_barrier_wait(&barr);
         if(err != 0 && err != PTHREAD_BARRIER_SERIAL_THREAD) {
          printf("Could not wait on barr\n");
          exit(-1);}

        for (i = i_start; i < i_end; i++)
        {
            int rowOffset = i*ncols;
            for (j = j_start; j < j_end; j++)
            {
              
              // temMemory stores the map of the current generation
              unsigned char cellNeghbourData = tmpMemory[rowOffset+j];

              // if a cell is dead and has no alive neighbours, move on
              if(cellNeghbourData != 0x0)
              {
                unsigned int count = (cellNeghbourData )>>1;
                int inorth = mod (i-1, nrows);
                int isouth = mod (i+1, nrows);
                int jwest = mod (j-1, ncols);
                int jeast = mod (j+1, ncols);

                if(cellNeghbourData & 0x1)
                {
                  if((count !=3) && (count !=2))
                  {
                   
                    // if the state needs to change,
                    // minus 1 from the 8 surrounding cells

                    // This big branch checks for whether the cell is in a critical region, if in a critial region, acquire lock
                    if ( i == i_start || i == i_start+1|| i == (i_end-1) ||i == (i_end-2) ) 
                    {
                      pthread_mutex_lock (&global_lock);
                      
                      boardMemory[rowOffset+j] &= ~0x01;
                      BOARD(inboard, i, j) = 0;
                      boardMemory[inorth*ncols+jwest] -=0x2;
                      boardMemory[inorth*ncols+j] -=0x2;
                      boardMemory[inorth*ncols+jeast] -=0x2;
                      
                      boardMemory[i*ncols+ jwest] -=0x2;
                      boardMemory[i*ncols+ jeast] -=0x2;

                      boardMemory[isouth*ncols+jwest] -=0x2;
                      boardMemory[isouth*ncols+j] -=0x2;
                      boardMemory[isouth*ncols+jeast] -=0x2;

                      pthread_mutex_unlock (&global_lock);
                    }
                    else
                    {
                        boardMemory[rowOffset+j] &= ~0x01;
                        BOARD(inboard, i, j) = 0;
                        boardMemory[inorth*ncols+jwest] -=0x2;
                        boardMemory[inorth*ncols+j] -=0x2;
                        boardMemory[inorth*ncols+jeast] -=0x2;
                        
                        boardMemory[i*ncols+ jwest] -=0x2;
                        boardMemory[i*ncols+ jeast] -=0x2;

                        boardMemory[isouth*ncols+jwest] -=0x2;
                        boardMemory[isouth*ncols+j] -=0x2;
                        boardMemory[isouth*ncols+jeast] -=0x2;
                    }
                
                  }

                }
                else
                {
                  if(count == 3)
                  {
                    
                     // if the state needs to change,
                    // plus 1 from the 8 surrounding cells


                    // This big branch checks for whether the cell is in a critical region, if in a critial region, acquire lock
                    if ( i == i_start || i == i_start+1|| i == (i_end-1) ||i == (i_end-2) ) 
                    {
                        pthread_mutex_lock (&global_lock);
                        
                        boardMemory[rowOffset+j] |= 0x1;
                        BOARD(inboard, i, j) = 1;

                        boardMemory[inorth*ncols+jwest] +=0x2;
                        boardMemory[inorth*ncols+j] +=0x2;
                        boardMemory[inorth*ncols+jeast] +=0x2;
                        
                        boardMemory[i*ncols+ jwest] +=0x2;
                        boardMemory[i*ncols+ jeast] +=0x2;
                        
                        boardMemory[isouth*ncols+jwest] +=0x2;
                        boardMemory[isouth*ncols+j] +=0x2;
                        boardMemory[isouth*ncols+jeast] +=0x2;
                        pthread_mutex_unlock (&global_lock);

                    }
                    else
                    {

                        boardMemory[rowOffset+j] |= 0x1;
                        BOARD(inboard, i, j) = 1;

                        boardMemory[inorth*ncols+jwest] +=0x2;
                        boardMemory[inorth*ncols+j] +=0x2;
                        boardMemory[inorth*ncols+jeast] +=0x2;
                        
                        boardMemory[i*ncols+ jwest] +=0x2;
                        boardMemory[i*ncols+ jeast] +=0x2;
                        
                        boardMemory[isouth*ncols+jwest] +=0x2;
                        boardMemory[isouth*ncols+j] +=0x2;
                        boardMemory[isouth*ncols+jeast] +=0x2;
                    }
                
                  }
                }
              }

            }
          }
    }
    return NULL;
}


    char*
parallel_game_of_life (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max)
{
    int i;
    pthread_t threads[NUM_THREADS];
    pthread_t threads2[NUM_THREADS];
    struct partition *board_data = (struct partition *) malloc (NUM_THREADS*sizeof(struct partition));

    unsigned char * tmpMemory = (unsigned char *) malloc(sizeof(unsigned char)*nrows*ncols); 
    unsigned char * boardMemory = (unsigned char *) malloc(sizeof(unsigned char)*nrows*ncols);  
    //unsigned char * checkBoard = (unsigned char *) malloc(sizeof(unsigned char)*nrows*ncols);

    memset(boardMemory, 0x0, sizeof(unsigned char)*nrows*ncols); 
    //memset(checkBoard, 0x0, sizeof(unsigned char)*nrows*ncols); 

    //initialize data structure to be passed into parallel_stream
    for (i = 0; i < NUM_THREADS; i++) {
      board_data[i].inboard = inboard;
      board_data[i].nrows = nrows;
      board_data[i].ncols = ncols;
      board_data[i].gens_max = gens_max;
      board_data[i].thread_id = i;
      board_data[i].tmpMemory = tmpMemory;
      board_data[i].boardMemory = boardMemory;
    }

    //The regions are separtated as equal partitions starting from top to botton
    board_data[0].i_start = 0;
    board_data[0].i_end = nrows/4;
    board_data[0].j_start = 0;
    board_data[0].j_end = ncols;

    board_data[1].i_start = nrows/4;
    board_data[1].i_end = nrows/2;
    board_data[1].j_start = 0;
    board_data[1].j_end = ncols;

    board_data[2].i_start = nrows/2;
    board_data[2].i_end = (nrows/4) * 3;
    board_data[2].j_start = 0;
    board_data[2].j_end = ncols;

    board_data[3].i_start = (nrows/4) * 3;
    board_data[3].i_end = nrows;
    board_data[3].j_start = 0;
    board_data[3].j_end = ncols;

    pthread_mutex_init(&global_lock, NULL);
    if(pthread_barrier_init(&barr, NULL, NUM_THREADS))
    {
        printf("barrier creation failed\n");
        return NULL;
    }
    //  These threads for the initialization of the grid map
    for (i = 0; i < NUM_THREADS; ++i)
    {
      if (pthread_create (&threads[i], NULL, initialize_map_parallel, (void *)(&board_data[i]))) {
        printf ("can not create thread \n");
        return NULL;
      }

    }

    for (i = 0; i < NUM_THREADS; i++) {
      pthread_join(threads[i], NULL);
    }

    //  These threads are for the grid
    for (i = 0; i < NUM_THREADS; ++i)
    {
      if (pthread_create (&threads2[i], NULL, parallel_streams, (void *)(&board_data[i]))) {
        printf ("can not create thread \n");
        return NULL;
      }
    }

    for (i = 0; i < NUM_THREADS; i++) {
      pthread_join(threads2[i], NULL);
    }

    /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     */
     free (tmpMemory);
     free (boardMemory);
     free (board_data);
     return inboard;
}




    char*
sequential_game_of_life (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max)
{
    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
    const int LDA = ncols;
    int curgen, i, j;

    for (curgen = 0; curgen < gens_max; curgen++)
    {
        /* HINT: you'll be parallelizing these loop(s) by doing a
           geometric decomposition of the output */
        for (i = 0; i < nrows; i++)
        {
            for (j = 0; j < ncols; j++)
            {
                const int inorth = mod (i-1, nrows);
                const int isouth = mod (i+1, nrows);
                const int jwest = mod (j-1, ncols);
                const int jeast = mod (j+1, ncols);

                const char neighbor_count = 
                    BOARD (inboard, inorth, jwest) + 
                    BOARD (inboard, inorth, j) + 
                    BOARD (inboard, inorth, jeast) + 
                    BOARD (inboard, i, jwest) +
                    BOARD (inboard, i, jeast) + 
                    BOARD (inboard, isouth, jwest) +
                    BOARD (inboard, isouth, j) + 
                    BOARD (inboard, isouth, jeast);

                BOARD(outboard, i, j) = alivep (neighbor_count, BOARD (inboard, i, j));

            }
        }
        SWAP_BOARDS( outboard, inboard );

    }
    /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     */
    return inboard;
}



