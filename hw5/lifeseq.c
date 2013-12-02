/*****************************************************************************
 * life.c
 * The original sequential implementation resides here.
 * Do not modify this file, but you are encouraged to borrow from it
 ****************************************************************************/
#include "life.h"
#include "util.h"

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
     unsigned  int curgen, i, j;
     for (i = 0; i < nrows; i++)
      {
          for (j = 0; j < ncols; j++)
          {
             unsigned int inorth = mod (i-1, nrows);
             unsigned  int isouth = mod (i+1, nrows);
             unsigned  int jwest = mod (j-1, ncols);
             unsigned  int jeast = mod (j+1, ncols);

            char neighbor_count = 
                  BOARD (inboard, inorth, jwest) + 
                  BOARD (inboard, inorth, j) + 
                  BOARD (inboard, inorth, jeast) + 
                  BOARD (inboard, i, jwest) +
                  BOARD (inboard, i, jeast) + 
                  BOARD (inboard, isouth, jwest) +
                  BOARD (inboard, isouth, j) + 
                  BOARD (inboard, isouth, jeast);

             //printf("count is %d, alive is %d\n",count,alive);
           // char alive = alivep (neighbor_count, BOARD (inboard, i, j));
           // int result = alive+ (neighbor_count <<1);
           // boardMemory[i*ncols+j] =   result;
           // printf("alive is %d and neightbour is %d\n",alive,neighbor_count);
           // printf("result" BYTETOBINARYPATTERN,BYTETOBINARY(result));
           // printf("\n");

           boardMemory[j*ncols+i] =  alivep (neighbor_count, BOARD (inboard, i, j)) + (neighbor_count <<1);

          }
        }

    int countChange;
    for (curgen = 0; curgen < gens_max; curgen++)
    {
        countChange= 0;
        for (i = 0; i < nrows; i++)
        {
            int rowOffset = i*ncols;
            for (j = 0; j < ncols; j++)
            {
              int cellNeghbourData = boardMemory[rowOffset+j];
              if(cellNeghbourData == 0)
                continue;

              char count = (cellNeghbourData & 0x1e)>>1;
              char alive = cellNeghbourData & 0x1;
              //printf("count is %d, alive is %d\n",count,alive);
              

              if(alive == 0x1)
              {
                if((count !=0x10) && (count !=0x11))
                {
                  // kill the cell
                  BOARD(outboard, i, j) = 0;
                  // decrement the neightbours
                  
                  unsigned  int inorth = mod (i-1, nrows);
                  unsigned  int isouth = mod (i+1, nrows);
                  unsigned  int jwest = mod (j-1, ncols);
                  unsigned  int jeast = mod (j+1, ncols);

                  boardMemory[rowOffset+j] = boardMemory[rowOffset+j] & 0x1e;

                  // BOARD (boardMemory, inorth, jwest) -=0x10;
                  // BOARD (boardMemory, inorth, j) -= 0x10;
                  // BOARD (boardMemory, inorth, jeast) -= 0x10;
                  // BOARD (boardMemory, i, jwest) -=0x10;
                  // BOARD (boardMemory, i, jeast) -=0x10;
                  // BOARD (boardMemory, isouth, jwest) -=0x10;
                  // BOARD (boardMemory, isouth, j) -=0x10;
                  // BOARD (boardMemory, isouth, jeast) -=0x10;

                  boardMemory[inorth*ncols+jwest] -=0x10;
                  boardMemory[inorth*ncols+j] -=0x10;
                  boardMemory[inorth*ncols+jeast] -=0x10;
                  boardMemory[i*ncols+ jwest] -=0x10;
                  boardMemory[i*ncols+ jeast] -=0x10;
                  boardMemory[isouth*ncols+isouth] -=0x10;
                  boardMemory[isouth*ncols+jeast] -=0x10;

                  countChange++;

                }

              }
              else
              {
                if(count == 0x11)
                {
                  unsigned  int inorth = mod (i-1, nrows);
                  unsigned  int isouth = mod (i+1, nrows);
                  unsigned  int jwest = mod (j-1, ncols);
                  unsigned  int jeast = mod (j+1, ncols);
                  // bring it back alive
                  // increment the neighbours
                  boardMemory[rowOffset+j] = boardMemory[rowOffset+j] | 0x1;

                  BOARD(outboard, i, j) = 1;

                   boardMemory[inorth*ncols+jwest] +=0x10;
                  boardMemory[inorth*ncols+j] +=0x10;
                  boardMemory[inorth*ncols+jeast] +=0x10;
                  boardMemory[i*ncols+ jwest] +=0x10;
                  boardMemory[i*ncols+ jeast] +=0x10;
                  boardMemory[isouth*ncols+isouth] +=0x10;
                  boardMemory[isouth*ncols+jeast] +=0x10;
                  countChange++;
                }
              }

            }
        }
        printf("count is %d\n",countChange);
        //SWAP_BOARDS( outboard, inboard );

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
        int count = 0;
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

                char a = BOARD(outboard, i, j);
                char b = alivep (neighbor_count, BOARD (inboard, i, j));

                if(a!= b)
                  count ++;

                BOARD(outboard, i, j) = b;

            }
        }
        printf("count is %d\n",count);
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




