#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else 
#include <CL/cl.h>
#endif

#include "util.h"

#define MAX_SOURCE_SIZE (0x100000)



/* 
  Izveidojam OpenCL saturu no pirmājām pieejamām iekartām,
  tādām kā GPU vai CPU
*/

cl_context createContext()
{
  /* OpenCL kļūdu mainīgais */
  cl_int error;
  /* OpenCL platformas numurs */
  cl_uint num_platforms;
  /* OpenCL platformas id */
  cl_platform_id* platforms_id = NULL;
  /* Definējam mainīgo priekš paltformu skaita */
  cl_uint platform_count;
  /* Definējam mainīgo priekš iekārtu skaita */
  cl_uint device_count;
  /* Definēja OpenCL saturu*/
  cl_context context = NULL;
  /* Ierīces id */
  cl_device_id* devices_id = NULL;
  cl_device_id device_id = NULL;
  /* Ierīces numurs*/
  cl_uint num_devices;
  /* maksimāli vienību skaits masīva */
  cl_uint max_cmpute_units = 0;

  /* Definējam priekš cikla mainīgo */
  int i,j;
  
  /* Iegūstam informāciju par platformām */
  clGetPlatformIDs(0, NULL, &platform_count);
  platforms_id = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platform_count);
  error = clGetPlatformIDs(platform_count,platforms_id,&num_platforms);
  if (error != CL_SUCCESS || num_platforms <=0 )
  { 
    printf(" Failded to find any OpenCL platforms! \n");
    printf("%s\n", oclErrorString(error));
    return NULL;
  }
  /* Atrodam labāko ierīci no visām  platformām ar maksīmālo paralēlo aprēķinu vienību skaitu */
  for (i = 0; i < platform_count; i++) {

    clGetDeviceIDs(platforms_id[i], CL_DEVICE_TYPE_ALL, 0, NULL, &device_count);
    devices_id = (cl_device_id*) malloc(sizeof(cl_device_id) * device_count);

    error = clGetDeviceIDs(platforms_id[i],CL_DEVICE_TYPE_ALL,device_count,devices_id,NULL);
    if (error != CL_SUCCESS)
    { 
      printf(" Error at clGetDeviceIDs! \n");
      printf("%s\n", oclErrorString(error));
      return NULL;
    }

      for (j = 0; j < device_count; j++) {

        cl_uint max_cmpute_units_tmp;
        clGetDeviceInfo(devices_id[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(max_cmpute_units), &max_cmpute_units_tmp, NULL);
        if (max_cmpute_units < max_cmpute_units_tmp)
        {
            max_cmpute_units = max_cmpute_units_tmp;
            device_id = devices_id[j];
        }
  
          /* Definējam OpenCL iestatījumus */
          /*cl_context_properties contextProperties[] =  { CL_CONTEXT_PLATFORM, (cl_context_properties)platforms_id[i], 0 };
          context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU, NULL, NULL, &error);
            if (error != CL_SUCCESS)
            { 
              printf(" Could not create GPU context! \n");
              printf("%s\n", oclErrorString(error));
              printf(" Trying CPU... ");
              context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_CPU, NULL, NULL, &error);
              if (error != CL_SUCCESS)
              {
                printf(" Failed to create on OpenCL GPU or CPU context! \n");
                printf("%s\n", oclErrorString(error));
                return NULL;
              }
            }*/
      }
      
  }
  context = clCreateContext(NULL,1,&device_id,NULL,NULL,&error);
  return context;
}

cl_command_queue createCommandQueue(cl_context context, cl_device_id *device_id)
{
  /* OpenCL kļūdu mainīgais */
  cl_int error;
  /* OpenCL iekārtas id */
  cl_device_id *devices_id;
  /* Definējam OpenCL komandas rindu */
  cl_command_queue command_queue = NULL;
  /* Iekārtas atmiņas lielums */
  size_t deviceBufferSize = -1;
  
  /* Iegūstam iekārtas atmiņas izmēru */
  error = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
  if(error != CL_SUCCESS)
  {
    printf(" Failed call to clGetContextInfo! \n");
    printf("%s\n", oclErrorString(error));
    return NULL;
  }
  if (deviceBufferSize <= 0)
  {
    printf(" No devices available! ");
    return NULL;
  }
  
  /* Atbrīvojam atmiņus apgabalu priekš OpenCL iekārtas */
  devices_id = malloc(deviceBufferSize / sizeof(cl_device_id));
  error = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices_id, NULL);
  if (error != CL_SUCCESS)
  {
    printf(" Failed to get device IDs \n");
    printf("%s\n", oclErrorString(error));
    return NULL;
  }
  
  /* Izveidojam OpenCL komandas rindu */
  command_queue = clCreateCommandQueue(context,devices_id[0],0,NULL);
  if (command_queue == NULL)
  {

    printf(" Failed to create commandQueue for device 0 \n");
    return NULL;
  }
  
  *device_id = devices_id[0];
  return command_queue;
}

/* Izveidojam OpenCL izpildes programmu no kodola faila */
cl_program createProgram(cl_context context, cl_device_id device_id, const char* fileName)
{
  /* OpenCL kļūdu mainīgais */
  cl_int error;
  /* Definējam OpenCL programmas mainīgo */
  cl_program program  = NULL;
  
  /* Ielasām kodolu no faila "kernel.cl" */
  FILE *fp;
  size_t source_size;
  char *source_str;
  fp = fopen(fileName, "r");
  if(!fp)
  {
    fprintf(stderr, "Failed to load kernel. \n");
    exit(1);
  }
  source_str = (char *)malloc(MAX_SOURCE_SIZE);
  source_size = fread(source_str,1,MAX_SOURCE_SIZE,fp);
  fclose(fp);
  
  /* Izveidojam programmas kodolu no faila */
  program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &error);
  if (program == NULL || error != CL_SUCCESS)
  {
    printf(" Failed to create CL programm from source. \n");
    printf("%s\n", oclErrorString(error));
    return NULL;
  }
  /* Viedojam programmu */
  error = clBuildProgram(program,1,&device_id,NULL,NULL,NULL);
  if (error != CL_SUCCESS)
  { 
    char buildLog[16384];
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, NULL);
    printf(" Error in programm \n");
    printf("%s", buildLog);
    return NULL;
  }
  free(source_str);
  return program;
}

bool createMemObjects(cl_context context, cl_mem memObjects[3], int size_arr)
{
    /* Izveidojam atmiņas objektu buffer apgabalu */
    memObjects[0] = clCreateBuffer(context,CL_MEM_READ_WRITE,size_arr*size_arr*sizeof(float),NULL,NULL);
    memObjects[1] = clCreateBuffer(context,CL_MEM_READ_WRITE,size_arr*size_arr*sizeof(float),NULL,NULL);
    memObjects[2] = clCreateBuffer(context,CL_MEM_READ_WRITE,size_arr*size_arr*sizeof(float),NULL,NULL);
    
     if (memObjects[0] == NULL || memObjects[1] == NULL || memObjects[2] == NULL)
     {
         printf(" Error creating memory objects! \n");
         return false;
      }

    return true;
}

/* Metode, kas iztīra visu OpenCL izveidojušos resursus */
void cleanup(cl_kernel kernel, cl_program program, cl_mem memObjects[3], cl_command_queue command_queue, cl_context context)
{
  if (kernel != NULL)
    clReleaseKernel(kernel);
  if (program != NULL)
    clReleaseProgram(program);
  if (command_queue != NULL)
    clReleaseCommandQueue(command_queue);
  if (context != NULL)
    clReleaseContext(context);
  for(int i=0;i<3;i++)
  {
    if (memObjects[i] != 0)
      clReleaseMemObject(memObjects[i]);
  }
}

/* 
  Galvenā OpenCL izpildes funkcija
*/
int mainCL(int size_arr)
{
        /* Inicializējam OpenCL tips */
        /* OpenCL iekārtas id */
        cl_device_id device_id = NULL;
        /* Definēja OpenCL saturu*/
        cl_context context = NULL;
        /* Definējam OpenCL komandas rindu */
        cl_command_queue command_queue = NULL;
        /* Definējam OpenCL mainīgos */
        cl_mem memObjects[3] = {NULL,NULL,NULL};
        /* Definējam OpenCL programmas mainīgo */
        cl_program program  = NULL;
        /* Deginējam OpenCL kodola mainīg */
        cl_kernel kernel = NULL;
        /* OpenCL kļūdu mainīgais */
        cl_int error;

    /* Standarta C mainīgi, kas nepieciešami programmai */
    int i, j;
    float *A;
    float *B;
    float *C;

    /* Piefiksējam lokālo izpildes laiku */
    clock_t start_l, end_l;
    double gpu_time_used; 

    /* Idelām atmiņās apgabalā vietu(malloc) */
    A = (float *)malloc(size_arr*size_arr*sizeof(float));
    B = (float *)malloc(size_arr*size_arr*sizeof(float));
    C = (float *)malloc(size_arr*size_arr*sizeof(float));



    /* Inicializējam ieejas datus */
    for(i=0;i<size_arr;i++)
    {
      for(j=0;j<size_arr;j++)
      {
       A[i*size_arr+j] = i*size_arr+j+1;
       B[i*size_arr+j] = j*size_arr+i+1;
      }
    }
    
    /* Piefiksējam laiku, kad OpenCL uzsāk savu darbu */
    start_l = clock();
    
    /* Izveidojam OpenCL saturu */
    context = createContext();
      if (context == NULL)
      {
        printf(" Failed to create OpenCL context! \n");
        return EXIT_FAILURE;
      }
    
    /* Izveidojam OpenCL komandas rindu */
    command_queue = createCommandQueue(context, &device_id);
    //command_queue = clCreateCommandQueue(context,device_id,0,&error);
      if (command_queue == NULL)
      {
        cleanup(kernel,program,memObjects,command_queue,context);
        return EXIT_FAILURE;
      }

    /* Izveidojam OpenCL programmu no "kernel.cl" kodola faila */
    program = createProgram(context,device_id,"kernel.cl");

    if (!createMemObjects(context, memObjects, size_arr))
        {
            cleanup(kernel,program,memObjects,command_queue,context);
            return EXIT_FAILURE;
        }

    /* Kopējam datus iekšā atmiņas apgabalā */
    error = clEnqueueWriteBuffer(command_queue,memObjects[0],CL_TRUE,0,size_arr*size_arr*sizeof(float),A,0,NULL,NULL);
    error = clEnqueueWriteBuffer(command_queue,memObjects[1],CL_TRUE,0,size_arr*size_arr*sizeof(float),B,0,NULL,NULL);



    /* Izveidojam OpenCL kodola kopu */
    kernel = clCreateKernel(program, "arraysum", &error);

    /* Pieškiram OpenCL kodolam argumentus */
    error = clSetKernelArg(kernel,0,sizeof(cl_mem),(void *)&memObjects[0]);
    error |= clSetKernelArg(kernel,1,sizeof(cl_mem),(void *)&memObjects[1]);
    error |= clSetKernelArg(kernel,2,sizeof(cl_mem),(void *)&memObjects[2]);
    error |= clSetKernelArg(kernel,3,sizeof(int),&size_arr);
             
    if (error != CL_SUCCESS)
    { 
      printf(" Error at clSetKernelArg! \n");
      printf("%s\n", oclErrorString(error));
      return EXIT_FAILURE;
    }

    size_t local_item_size = 4;
    size_t global_item_size = roundWorkSizeUp(local_item_size, size_arr);
    
    /* Paralēli izpildām OpenCL kodola datus */
    error = clEnqueueNDRangeKernel(command_queue,kernel,1,NULL,&global_item_size,&local_item_size,0,NULL,NULL);
    if (error != CL_SUCCESS)
    { 
      printf(" Error at clEnqueueNDRangeKernel! \n");
      printf("%s\n", oclErrorString(error));
      return EXIT_FAILURE;
    }
    

    /* Pārnesam rezultātu uz hostu */
    error = clEnqueueReadBuffer(command_queue,memObjects[2],CL_TRUE,0,size_arr*size_arr*sizeof(float),C,0,NULL,NULL);
    if (error != CL_SUCCESS)
    { 
      printf(" Error at clEnqueueReadBuffer! \n");
      printf("%s\n", oclErrorString(error));
      return EXIT_FAILURE;
    }
    
    /* Piefiksējam laiku, kad OpenCL beidz savu darbu */
    end_l = clock();
    
    /* Izvadām inicalizētos datus */
    for(i=0;i<size_arr;i++)
    {
      for(j=0;j<size_arr;j++)
      {
       printf("%7.2f calculation with %7.2f = %7.2f \n", A[i*size_arr+j], B[i*size_arr+j], C[i*size_arr+j]);
      }
    } 
    
    /* Izvadām OpenCL izpildes laiku */
    gpu_time_used = ((double) (end_l - start_l)) / CLOCKS_PER_SEC;
    printf("== OpenCL execution time ==\n");
    printf("%f \n",gpu_time_used);
      
    /* Pārbauda, vai kamandas rinda ir izpildījusies uz atbilstošās ierīces */
    error = clFlush(command_queue);

    /* Pārbauda, vai visas komandrindu vērtības tiek atgrieztas sekmīgi */
    error = clFinish(command_queue);

    /* Iztirām OpenCL izviedoto saturu */
    cleanup(kernel,program,memObjects,command_queue,context);
     
    /* Atbrīvojam atmiņu */
    free(A);
    free(B);
    free(C);
    
    return 0;
}






