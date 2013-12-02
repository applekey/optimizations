/*****************************************************************************
 * life.c
 * The original sequential implementation resides here.
 * Do not modify this file, but you are encouraged to borrow from it
 ****************************************************************************/
#include "life.h"
#include "util.h"

/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
#define SWAP_BOARDS( b1, b2 )  do { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)

#define BOARD( __board, __i, __j )  (__board[(__i) + LDA*(__j)])




    char*
Parrallel_game_of_life (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max)
{
    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */

    int * boardMemory = (int *) malloc(sizeof(int)*nrows*ncols);  
      const int LDA = nrows;
    // do first generation and store everthing into memory
     int curgen, i, j;
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

              boardMemory[j*ncols+i] =  alivep (neighbor_count, BOARD (inboard, i, j)) + (neighbor_count >>1);
          }
        }

    for (curgen = 0; curgen < gens_max; curgen++)
    {
        for (i = 0; i < nrows; i++)
        {
            for (j = 0; j < ncols; j++)
            {
              int cellNeghbourData = boardMemory[i*ncols+j];
              char count = (cellNeghbourData & 0x0e)<<1;
              char alive = cellNeghbourData & 0x1;
              if(cellNeghbourData == 0)
                continue;

              if(alive == 1)
              {
                if((count !=2) && (count !=3))
                {
                  // kill the cell
                  BOARD(outboard, i, j) = 0;
                  // decrement the neightbours
                

                  const int inorth = mod (i-1, nrows);
                  const int isouth = mod (i+1, nrows);
                  const int jwest = mod (j-1, ncols);
                  const int jeast = mod (j+1, ncols);

                  BOARD (boardMemory, inorth, jwest) = BOARD (boardMemory, inorth, jwest)-0x10;
                  BOARD (boardMemory, inorth, j) = BOARD (boardMemory, inorth, j) -0x10;
                  BOARD (boardMemory, inorth, jeast) =  BOARD (boardMemory, inorth, jeast)-0x10;
                  BOARD (boardMemory, i, jwest) =BOARD (boardMemory, i, jwest)-0x10;
                  BOARD (boardMemory, i, jeast) = BOARD (boardMemory, i, jeast)-0x10;
                  BOARD (boardMemory, isouth, jwest) = BOARD (boardMemory, isouth, jwest)-0x10;
                  BOARD (boardMemory, isouth, j) =  BOARD (boardMemory, isouth, j)-0x10;
                  BOARD (boardMemory, isouth, jeast) = BOARD (boardMemory, isouth, jeast)-0x10;

                }

              }
              else
              {
                if(count == 3)
                {
                   const int inorth = mod (i-1, nrows);
                  const int isouth = mod (i+1, nrows);
                  const int jwest = mod (j-1, ncols);
                  const int jeast = mod (j+1, ncols);
                  // bring it back alive
                  // increment the neighbours
                  BOARD(outboard, i, j) = 1;
                  BOARD (boardMemory, inorth, jwest) = BOARD (boardMemory, inorth, jwest)+0x10;
                  BOARD (boardMemory, inorth, j) = BOARD (boardMemory, inorth, j) +0x10;
                  BOARD (boardMemory, inorth, jeast) =  BOARD (boardMemory, inorth, jeast)+0x10;
                  BOARD (boardMemory, i, jwest) =BOARD (boardMemory, i, jwest)+0x10;
                  BOARD (boardMemory, i, jeast) = BOARD (boardMemory, i, jeast)+0x10;
                  BOARD (boardMemory, isouth, jwest) = BOARD (boardMemory, isouth, jwest)+0x10;
                  BOARD (boardMemory, isouth, j) =  BOARD (boardMemory, isouth, j)+0x10;
                  BOARD (boardMemory, isouth, jeast) = BOARD (boardMemory, isouth, jeast)+0x10;
                }
              }

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


    char*
sequential_game_of_life (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max)
{
    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
    const int LDA = nrows;
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




