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
    unsigned char * checkBoard = (unsigned char *) malloc(sizeof(unsigned char)*nrows*ncols);

    memset(boardMemory, 0x0, sizeof(unsigned char)*nrows*ncols); 
    memset(checkBoard, 0x0, sizeof(unsigned char)*nrows*ncols); 

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


           // char alive = alivep (neighbor_count, BOARD (inboard, i, j));
           // char result = alive+ (neighbor_count <<1);
           // boardMemory[i*ncols+j] =   result;
           // printf("alive is %d and neightbour is %d\n",alive,neighbor_count);
           // printf("result" BYTETOBINARYPATTERN,BYTETOBINARY(result));
           // printf("\n");

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
              if(cellNeghbourData != 0)
              {

                unsigned int count = (cellNeghbourData )>>1;
            
                //printf("count is %d, alive is %d\n",count,cellNeghbourData & 0x1);

                if(cellNeghbourData & 0x1)
                {
                  if((count !=3) && (count !=2))
                  {
                    // kill the cell
                     BOARD(inboard, i, j) = 0x0;
                    // decrement the neightbours
                    
                    int inorth = mod (i-1, nrows);
                    int isouth = mod (i+1, nrows);
                    int jwest = mod (j-1, ncols);
                    int jeast = mod (j+1, ncols);

                    boardMemory[rowOffset+j] &= ~0x01;

                    //printf("before"BYTETOBINARYPATTERN,BYTETOBINARY(boardMemory[inorth*ncols+jwest]));
                    //printf("\n");
                  
                   
                    

                    //printf("after "BYTETOBINARYPATTERN,BYTETOBINARY(boardMemory[inorth*ncols+jwest]));
                    //printf("\n");
                    // printf("\n");
                    boardMemory[inorth*ncols+jwest] -=0x2;
                    boardMemory[inorth*ncols+j] -=0x2;
                    boardMemory[inorth*ncols+jeast] -=0x2;
                    
                    boardMemory[i*ncols+ jwest] -=0x2;
                    boardMemory[i*ncols+ jeast] -=0x2;

                    boardMemory[isouth*ncols+jwest] -=0x2;
                    boardMemory[isouth*ncols+isouth] -=0x2;
                    boardMemory[isouth*ncols+jeast] -=0x2;

                
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

                    //printf("%d,%d,%d,%d\n",inorth,isouth,jwest,jeast);

                    // bring it back alive
                    // increment the neighbours

                       printf("start\n");
                    unsigned char cellNeghbourData1 = boardMemory[inorth*ncols+jwest];
                   count = (cellNeghbourData1 )>>1;
                    printf("%d\n",count);

                      unsigned char cellNeghbourData2 = boardMemory[inorth*ncols+j];
                     count = (cellNeghbourData2 )>>1;
                    printf("%d\n",count);

                      unsigned char cellNeghbourData3 = boardMemory[inorth*ncols+jeast];
                    count = (cellNeghbourData3 )>>1;
                    printf("%d\n",count);

                      unsigned char cellNeghbourData4 = boardMemory[i*ncols+ jwest];
                    count = (cellNeghbourData4 )>>1;
                    printf("%d\n",count);

                      unsigned char cellNeghbourData5 = boardMemory[i*ncols+ jeast];
                   count = (cellNeghbourData5 )>>1;
                    printf("%d\n",count);

                      unsigned char cellNeghbourData6 = boardMemory[isouth*ncols+jwest];
                     count = (cellNeghbourData6 )>>1;
                    printf("%d\n",count);

                      unsigned char cellNeghbourData7 = boardMemory[isouth*ncols+isouth];
                     count = (cellNeghbourData7 )>>1;
                    printf("%d\n",count);

                    unsigned char cellNeghbourData8 = boardMemory[isouth*ncols+jeast];
                     count = (cellNeghbourData8 )>>1;
                    printf("%d\n",count);

                    printf("update\n");


                    boardMemory[rowOffset+j] |= 0x1;

                    BOARD(inboard, i, j) = 0x1;

                    boardMemory[inorth*ncols+jwest] +=0x2;
                    boardMemory[inorth*ncols+j] +=0x2;
                    boardMemory[inorth*ncols+jeast] +=0x2;
                    
                    boardMemory[i*ncols+ jwest] +=0x2;
                    boardMemory[i*ncols+ jeast] +=0x2;
                    
                    boardMemory[isouth*ncols+jwest] +=0x2;
                    boardMemory[isouth*ncols+isouth] +=0x2;
                    boardMemory[isouth*ncols+jeast] +=0x2;

                            cellNeghbourData1 = boardMemory[inorth*ncols+jwest];
                    count = (cellNeghbourData1 )>>1;
                    printf("%d\n",count);

                       cellNeghbourData2 = boardMemory[inorth*ncols+j];
                     count = (cellNeghbourData2 )>>1;
                    printf("%d\n",count);

                       cellNeghbourData3 = boardMemory[inorth*ncols+jeast];
                     count = (cellNeghbourData3 )>>1;
                    printf("%d\n",count);

                     cellNeghbourData4 = boardMemory[i*ncols+ jwest];
                    count = (cellNeghbourData4 )>>1;
                    printf("%d\n",count);

                       cellNeghbourData5 = boardMemory[i*ncols+ jeast];
                     count = (cellNeghbourData5 )>>1;
                    printf("%d\n",count);

                       cellNeghbourData6 = boardMemory[isouth*ncols+jwest];
                    count = (cellNeghbourData6 )>>1;
                    printf("%d\n",count);

                       cellNeghbourData7 = boardMemory[isouth*ncols+isouth];
                    count = (cellNeghbourData7 )>>1;
                    printf("%d\n",count);

                     cellNeghbourData8 = boardMemory[isouth*ncols+jeast];
                    count = (cellNeghbourData8 )>>1;
                    printf("%d\n",count);


                    countChange++;
                  }
                }
              }

            }
        }

         // just some verification code
     
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

           checkBoard[i*ncols+j] = (neighbor_count <<1) + alive;  // set all as dead

          }
        }
        // just some verification code end
      
        for(i=0;i<nrows*ncols;i++)
        {
           printf("board"BYTETOBINARYPATTERN,BYTETOBINARY(boardMemory[i]));
            printf("\n");

             printf("check"BYTETOBINARYPATTERN,BYTETOBINARY(checkBoard[i]));
            printf("\n");
             
          if(boardMemory[i]!= checkBoard[i])
          {
            printf("error\n");

           
          }
          printf("\n");
            printf("%d \n",i);

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
    free(boardMemory);
    free(tmpMemory);
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




