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

    char * boardMemory = (char *) malloc(sizeof(char)*nrows*ncols);  
      const int LDA = nrows;
    // do first generation and store everthing into memory
       int curgen, i, j;
     for (i = 0; i < nrows; i++)
      {
          for (j = 0; j < ncols; j++)
          {
             int inorth = mod (i-1, nrows);
               int isouth = mod (i+1, nrows);
               int jwest = mod (j-1, ncols);
               int jeast = mod (j+1, ncols);

            char neighbor_count = 
                  BOARD (inboard, inorth, jwest) + 
                  BOARD (inboard, inorth, j) + 
                  BOARD (inboard, inorth, jeast) + 
                  BOARD (inboard, i, jwest) +
                  BOARD (inboard, i, jeast) + 
                  BOARD (inboard, isouth, jwest) +
                  BOARD (inboard, isouth, j) + 
                  BOARD (inboard, isouth, jeast);


           // char alive = alivep (neighbor_count, BOARD (inboard, i, j));
           // char result = alive+ (neighbor_count <<1);
           // boardMemory[i*ncols+j] =   result;
           // printf("alive is %d and neightbour is %d\n",alive,neighbor_count);
           // printf("result" BYTETOBINARYPATTERN,BYTETOBINARY(result));
           // printf("\n");

           boardMemory[j*ncols+i] = (neighbor_count <<1);  // set all as dead

          }
        }

    for (curgen = 0; curgen < gens_max; curgen++)
    {
        int countChange= 0;
        for (i = 0; i < nrows; i++)
        {
            int rowOffset = i*ncols;
            for (j = 0; j < ncols; j++)
            {
              char cellNeghbourData = boardMemory[rowOffset+j];
              if(cellNeghbourData != 0)
              {


              

                char count = (cellNeghbourData & 0x1e)>>1;
                char alive = cellNeghbourData & 0x1;
                
                // if(curgen>0)
                //   printf("count is %d, alive is %d\n",count,alive);
                

                if(alive == 1)
                {
                  if((count !=3) && (count !=2))
                  {
                    // kill the cell
                     BOARD(inboard, i, j) = 0;
                    // decrement the neightbours
                    
                      int inorth = mod (i-1, nrows);
                      int isouth = mod (i+1, nrows);
                      int jwest = mod (j-1, ncols);
                      int jeast = mod (j+1, ncols);

                    boardMemory[rowOffset+j] = boardMemory[rowOffset+j] & 0x1e;

                    boardMemory[inorth*ncols+jwest] -=2;
                    boardMemory[inorth*ncols+j] -=2;
                    boardMemory[inorth*ncols+jeast] -=2;
                    
                    boardMemory[i*ncols+ jwest] -=2;
                    boardMemory[i*ncols+ jeast] -=2;

                    boardMemory[isouth*ncols+jwest] -=2;
                    boardMemory[isouth*ncols+isouth] -=2;
                    boardMemory[isouth*ncols+jeast] -=2;

                    countChange++;

                  }

                }
                else
                {
                  if(count == 3)
                  {
                      int inorth = mod (i-1, nrows);
                      int isouth = mod (i+1, nrows);
                      int jwest = mod (j-1, ncols);
                      int jeast = mod (j+1, ncols);
                    // bring it back alive
                    // increment the neighbours
                    boardMemory[rowOffset+j] = boardMemory[rowOffset+j] | 0x1;

                    BOARD(inboard, i, j) = 1;

                    boardMemory[inorth*ncols+jwest] +=2;
                    boardMemory[inorth*ncols+j] +=2;
                    boardMemory[inorth*ncols+jeast] +=2;
                    
                    boardMemory[i*ncols+ jwest] +=2;
                    boardMemory[i*ncols+ jeast] +=2;
                    
                    boardMemory[isouth*ncols+jwest] +=2;
                    boardMemory[isouth*ncols+isouth] +=2;
                    boardMemory[isouth*ncols+jeast] +=2;
                    countChange++;
                  }
                }
              }
            }
        }
       //printf("count is %d\n",countChange);
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

    printf("called\n");
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
        //printf("count is %d\n",count);
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




