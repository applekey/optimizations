/*****************************************************************************
 * life.c
 * The original sequential implementation resides here.
 * Do not modify this file, but you are encouraged to borrow from it
 ****************************************************************************/
#include "life.h"
#include "util.h"
 #include <stdlib.h>
 #include <string.h>

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


    char*
Parrallel_game_of_life (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max)
{
    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
    unsigned char * tmpMemory = (unsigned char *) malloc(sizeof(unsigned char)*nrows*ncols); 
    unsigned char * boardMemory = (unsigned char *) malloc(sizeof(unsigned char)*nrows*ncols);  
    //unsigned char * checkBoard = (unsigned char *) malloc(sizeof(unsigned char)*nrows*ncols);

    memset(boardMemory, 0x0, sizeof(unsigned char)*nrows*ncols); 
    //memset(checkBoard, 0x0, sizeof(unsigned char)*nrows*ncols); 

    const int LDA = ncols;
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
      
           boardMemory[i*ncols+j] = (neighbor_count <<1) + alive;  // set all as dead

          }
        }

    for (curgen = 0; curgen < gens_max; curgen++)
    {
        int countChange= 0;
        memmove(tmpMemory,boardMemory,nrows*ncols);

        for (i = 0; i < nrows; i++)
        {
            int rowOffset = i*ncols;
            for (j = 0; j < ncols; j++)
            {
              unsigned char cellNeghbourData = tmpMemory[rowOffset+j];
              if(cellNeghbourData != 0x0)
              {

                unsigned int count = (cellNeghbourData )>>1;
            
                if(cellNeghbourData & 0x1)
                {
                  if((count !=3) && (count !=2))
                  {
                    // kill the cell
                     BOARD(inboard, i, j) = 0;
                    // decrement the neightbours
                    
                    //printf("change-, %d, %d\n",i,j);
                    int inorth = mod (i-1, nrows);
                    int isouth = mod (i+1, nrows);
                    int jwest = mod (j-1, ncols);
                    int jeast = mod (j+1, ncols);

                    boardMemory[rowOffset+j] &= ~0x01;

                    boardMemory[inorth*ncols+jwest] -=0x2;
                    boardMemory[inorth*ncols+j] -=0x2;
                    boardMemory[inorth*ncols+jeast] -=0x2;
                    
                    boardMemory[i*ncols+ jwest] -=0x2;
                    boardMemory[i*ncols+ jeast] -=0x2;

                    boardMemory[isouth*ncols+jwest] -=0x2;
                    boardMemory[isouth*ncols+j] -=0x2;
                    boardMemory[isouth*ncols+jeast] -=0x2;

                
                    //countChange++;

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

          
                    countChange++;
                  }
                }
              }

            }
        }
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



