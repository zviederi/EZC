#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int mainCL(int size_arr)
{
    int i, j;
    float *A;
    float *B;
    float *C;
    
    clock_t start_l, end_l;
    double gpu_time_used;   
    
    /* Idelām atmiņās apgabalā vietu(malloc) */
    A = (float *)malloc(size_arr*size_arr*sizeof(float));
    B = (float *)malloc(size_arr*size_arr*sizeof(float));
    C = (float *)malloc(size_arr*size_arr*sizeof(float));
    
    for(i=0;i<size_arr;i++)
    {
      for(j=0;j<size_arr;j++)
      {
       A[i*size_arr+j] = i*size_arr+j+1;
       B[i*size_arr+j] = j*size_arr+i+1;
      }
    }
    
    start_l = clock();
     for(i=0;i<size_arr;i++){
         for (j=0; j < size_arr; j++)
          {
              C[(i*size_arr)+j] = (A[(i*size_arr)+j])*pow(sin(-M_PI),cos(M_PI)) + (pow(sin(-M_PI),cos(M_PI)) * B[(i*size_arr)+j]);                           
          }
      }
     end_l = clock();
     
    
    /* Izvadām inicalizētos datus */
    for(i=0;i<size_arr;i++)
    {
      for(j=0;j<size_arr;j++)
      {
       printf("%7.2f calculation with %7.2f  = %7.2f \n", A[i*size_arr+j], B[i*size_arr+j], C[i*size_arr+j]);
      }
    }
      gpu_time_used = ((double) (end_l - start_l)) / CLOCKS_PER_SEC;
     printf("== Calculation execution time ==\n");
     printf("%.2f sec.\n",gpu_time_used);     
}

int main(int argc, char *argv[])
{
    /* Piefiksējam izpildes laiku */
    clock_t start, end;
    double gpu_time_used;
         
    /* starts pirms datu inicializācijas */  
    start = clock();
      
    char * pEnd;
    int size_array = strtol(argv[1], &pEnd,10);
    if (size_array <= 0)
      {
        printf("== No integer type or integer is negative ==\n", size_array);
        return 0;
      }
    else
      mainCL(size_array);

     /* pievifiksējam prgrammas izpildes beigu laiku */
     end = clock();
     gpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
     printf("== Total execution time ==\n");
     printf("%.2f sec.\n",gpu_time_used);
   
}


