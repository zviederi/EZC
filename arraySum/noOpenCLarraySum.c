#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int mainCL(int size_arr)
{
    int i, j;
    float *A;
    float *B;
    float *C;
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
    
     for(i=0;i<size_arr;i++){
         for (j=0; j < size_arr; j++)
          {
              C[i*size_arr+j] = (A[i*size_arr+j]*2*4.5) + (B[i*size_arr+j]/A[i*size_arr+j]);
                              
          }
      }
    
    
    /* Izvadām inicalizētos datus */
    for(i=0;i<size_arr;i++)
    {
      for(j=0;j<size_arr;j++)
      {
       printf("%7.2f + %7.2f  = %7.2f \n", A[i*size_arr+j], B[i*size_arr+j], C[i*size_arr+j]);
      }
    }     
}

int main()
{
      /* Piefiksējam izpildes laiku */
      clock_t start, end;
      double gpu_time_used;   
      /* starts pirms datu inicializācijas */  
      
      start = clock();
      int size_array = 1000;
        mainCL(size_array);


     /* pievifiksējam prgrammas izpildes beigu laiku */
     end = clock();
     gpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
     printf("====\n");
     printf("%f \n",gpu_time_used);
   
}


