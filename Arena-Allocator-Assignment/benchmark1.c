#include "mavalloc.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#define TEST_SIZE 1000
#define BLOCK_SIZE 50
#define ITERATIONS 1000

int main( int argc, char * argv[] )
{
  FILE * file = fopen("benchmark1.csv", "w");
  fprintf(file, "malloc\n");
  int i;
  for(i = 0; i < ITERATIONS; i++)
  {
    clock_t start = clock();
    void * list[TEST_SIZE];
    int i;
    for(i = 0; i < TEST_SIZE; i++)
    {
      list[i] = (char *) malloc(BLOCK_SIZE);
    }
    //Free every even numbered entry
    for(i = 0; i < TEST_SIZE; i += 2)
    {
      free(list[i]);
      //Also going to set all freed pointers to NULL so we don't get any invalid pointer shennanigans
      list[i] = NULL;
    }
  
    //Also free every other odd numbered index, so we get some nice holes to fill
    for(i = 1; i < TEST_SIZE; i += 4)
    {
      free(list[i]);
      //Also going to set all freed pointers to NULL so we don't get any invalid pointer shennanigans
      list[i] = NULL;
    }
  
    //Allocate all the holes now, but bigger
    for(i = 0; i < TEST_SIZE; i += 1)
    {
      if(list[i] == NULL)
      {
        list[i] = (char *) malloc(BLOCK_SIZE * 2);
      }
    }
  
    //Free the whole thing now
    for(i = 0; i < TEST_SIZE; i++)
    {
      if(list[i] != NULL)
      {
        free(list[i]);
      }
    }
    clock_t end = clock();
    double execTime = end - start;
    execTime = execTime / (CLOCKS_PER_SEC / 1000);
    fprintf(file, "%lf\n", execTime);
  }
  fclose(file);
  return 0;
}
