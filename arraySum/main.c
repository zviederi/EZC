#include <time.h>
#include "arraySum.h"

int main()
{

  /* Piefiksējam izpildes laiku */
  clock_t start, end;
  double gpu_time_used;   
      
  /* starts pirms datu inicializācijas */        
  start = clock();
      
  int size_array = 10000;
  mainCL(size_array);

  /* pievifiksējam prgrammas izpildes beigu laiku */
  end = clock();
  
  gpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  printf("== Total execution time ==\n");
  printf("%f \n",gpu_time_used);
   
}
