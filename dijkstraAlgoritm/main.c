#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __APPLE__
    #include <OpenCL/cl.h>
#else
    #include <CL/cl.h>
#endif

#include "util.h"
#include "graph.h"
#include "dijkstra.h"

#define MAX_SOURCE_SIZE (0x100000)
#define NUM_ASYNCHRONOUS_ITERATIONS 10 



cl_program loadAndBuildProgram(cl_context context, cl_device_id device_id, const char* fileName)
{

  cl_int error;

  cl_program program  = NULL;
  

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
  

  program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &error);
  if (program == NULL || error != CL_SUCCESS)
  {
    printf(" Failed to create CL programm from source. \n");
    //printf("%s\n", oclErrorString(error));
    return NULL;
  }

  error = clBuildProgram(program,1,&device_id,NULL,NULL,NULL);
  if (error != CL_SUCCESS)
  { 
    char cBuildLog[10240];
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(cBuildLog), cBuildLog, NULL );
    printf(" Error in programm \n");
    printf("%s", cBuildLog);
    return NULL;
  }
  free(source_str);
  return program;
}


void allocateOCLBuffers(cl_context gpuContext, cl_command_queue commandQueue,  GraphData *graph,
                        cl_mem *vertexArrayDevice, cl_mem *edgeArrayDevice, cl_mem *weightArrayDevice,
                        cl_mem *maskArrayDevice, cl_mem *costArrayDevice, cl_mem *updatingCostArrayDevice,
                        size_t globalWorkSize)
{
    cl_int errNum;
    cl_mem hostVertexArrayBuffer;
    cl_mem hostEdgeArrayBuffer;
    cl_mem hostWeightArrayBuffer;

    hostVertexArrayBuffer = clCreateBuffer(gpuContext, CL_MEM_COPY_HOST_PTR | CL_MEM_ALLOC_HOST_PTR,sizeof(int) * graph->vertexCount, graph->vertexArray, &errNum);
    #define NUM_ASYNCHRONOUS_ITERATIONS 10 
    hostEdgeArrayBuffer = clCreateBuffer(gpuContext, CL_MEM_COPY_HOST_PTR | CL_MEM_ALLOC_HOST_PTR,sizeof(int) * graph->edgeCount, graph->edgeArray, &errNum);
    hostWeightArrayBuffer = clCreateBuffer(gpuContext, CL_MEM_COPY_HOST_PTR | CL_MEM_ALLOC_HOST_PTR,sizeof(float) * graph->edgeCount, graph->weightArray, &errNum);


    *vertexArrayDevice = clCreateBuffer(gpuContext, CL_MEM_READ_ONLY, sizeof(int) * globalWorkSize, NULL, &errNum);
    *edgeArrayDevice = clCreateBuffer(gpuContext, CL_MEM_READ_ONLY, sizeof(int) * graph->edgeCount, NULL, &errNum);
    *weightArrayDevice = clCreateBuffer(gpuContext, CL_MEM_READ_ONLY, sizeof(float) * graph->edgeCount, NULL, &errNum);
    *maskArrayDevice = clCreateBuffer(gpuContext, CL_MEM_READ_WRITE, sizeof(int) * globalWorkSize, NULL, &errNum);
    *costArrayDevice = clCreateBuffer(gpuContext, CL_MEM_READ_WRITE, sizeof(float) * globalWorkSize, NULL, &errNum);
    *updatingCostArrayDevice = clCreateBuffer(gpuContext, CL_MEM_READ_WRITE, sizeof(float) * globalWorkSize, NULL, &errNum);
    errNum = clEnqueueCopyBuffer(commandQueue, hostVertexArrayBuffer, *vertexArrayDevice, 0, 0,sizeof(int) * graph->vertexCount, 0, NULL, NULL);
    errNum = clEnqueueCopyBuffer(commandQueue, hostEdgeArrayBuffer, *edgeArrayDevice, 0, 0,sizeof(int) * graph->edgeCount, 0, NULL, NULL);
    errNum = clEnqueueCopyBuffer(commandQueue, hostWeightArrayBuffer, *weightArrayDevice, 0, 0,sizeof(float) * graph->edgeCount, 0, NULL, NULL);
    clReleaseMemObject(hostVertexArrayBuffer);
    clReleaseMemObject(hostEdgeArrayBuffer);
    clReleaseMemObject(hostWeightArrayBuffer);
}

int main()
{
    int generateVerts = 4;
    int generateEdgesPerVert = 1;
    int numSources = 2;
    
    GraphData graph;
    generateRandomGraph(&graph, generateVerts, generateEdgesPerVert);    
     for(int i = 0; i < graph.edgeCount; i++)
    {
        printf("%i -> %i (%f) \n", graph.vertexArray[i],graph.edgeArray[i],graph.weightArray[i]);
    }    
    
    cl_platform_id platform = NULL;
    cl_device_id device_id = NULL;
    cl_context context = NULL;
    cl_program program = NULL;
    cl_int error;
    cl_command_queue command_queue = NULL;

    context = createContext();
      if (context == NULL)
      {
        printf(" Failed to create OpenCL context! \n");
        return EXIT_FAILURE;
      }
      
    command_queue = createCommandQueue(context, &device_id);
      if (command_queue == NULL)
      {
        return EXIT_FAILURE;
      }
      
    program = loadAndBuildProgram( context, device_id, "dijkstra.cl" );
      if (program <= 0 )
      {
          printf(" Failed to build programm \n");
          return EXIT_FAILURE;
      }
      
    size_t maxWorkGroupSize;
    clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize, NULL);
    printf(" Max workgroup size: %u \n", maxWorkGroupSize);
   
    int sourceVertices[numSources];
    
    for(int source = 0; source < numSources; source++)
    {
        sourceVertices[source] =  source % graph.vertexCount;
    }
    printf("Vertex count: %u\n", graph.vertexCount);
   
    int *sourceVertArray = (int*) malloc(sizeof(int) * sizeof(sourceVertices));
    //copy(sourceVertices[0], sourceVertices[numSources], sourceVertArray);
    for (int i = 0; i < numSources; i++)
    {
      sourceVertArray[i] = sourceVertices[i];
    }
    float *results = (float*) malloc(sizeof(float) * sizeof(sourceVertices) * graph.vertexCount);
    printf(" Computing: %i \n", sizeof(sourceVertices));
    
    size_t localWorkSize = maxWorkGroupSize;
    size_t globalWorkSize = roundWorkSizeUp(localWorkSize, graph.vertexCount);
    
    cl_mem vertexArrayDevice;
    cl_mem edgeArrayDevice;
    cl_mem weightArrayDevice;
    cl_mem maskArrayDevice;
    cl_mem costArrayDevice;
    cl_mem updatingCostArrayDevice;
    
    allocateOCLBuffers(context, command_queue, &graph, &vertexArrayDevice, &edgeArrayDevice, &weightArrayDevice, &maskArrayDevice, &costArrayDevice, &updatingCostArrayDevice, globalWorkSize);
    
    cl_kernel initializeBuffersKernel;
    initializeBuffersKernel = clCreateKernel(program, "initializeBuffers", &error);
    
    error |= clSetKernelArg(initializeBuffersKernel, 0, sizeof(cl_mem), &maskArrayDevice);
    error |= clSetKernelArg(initializeBuffersKernel, 1, sizeof(cl_mem), &costArrayDevice);
    error |= clSetKernelArg(initializeBuffersKernel, 2, sizeof(cl_mem), &updatingCostArrayDevice);

    // 3 set below in loop
    error |= clSetKernelArg(initializeBuffersKernel, 4, sizeof(int), &graph.vertexCount);
    
    cl_kernel ssspKernel1;
    ssspKernel1 = clCreateKernel(program, "OCL_SSSP_KERNEL1", &error);
    
    error |= clSetKernelArg(ssspKernel1, 0, sizeof(cl_mem), &vertexArrayDevice);
    error |= clSetKernelArg(ssspKernel1, 1, sizeof(cl_mem), &edgeArrayDevice);
    error |= clSetKernelArg(ssspKernel1, 2, sizeof(cl_mem), &weightArrayDevice);
    error |= clSetKernelArg(ssspKernel1, 3, sizeof(cl_mem), &maskArrayDevice);
    error |= clSetKernelArg(ssspKernel1, 4, sizeof(cl_mem), &costArrayDevice);
    error |= clSetKernelArg(ssspKernel1, 5, sizeof(cl_mem), &updatingCostArrayDevice);
    error |= clSetKernelArg(ssspKernel1, 6, sizeof(int), &graph.vertexCount);
    error |= clSetKernelArg(ssspKernel1, 7, sizeof(int), &graph.edgeCount);

    cl_kernel ssspKernel2;
    ssspKernel2 = clCreateKernel(program, "OCL_SSSP_KERNEL2", &error);

    error |= clSetKernelArg(ssspKernel2, 0, sizeof(cl_mem), &vertexArrayDevice);
    error |= clSetKernelArg(ssspKernel2, 1, sizeof(cl_mem), &edgeArrayDevice);
    error |= clSetKernelArg(ssspKernel2, 2, sizeof(cl_mem), &weightArrayDevice);
    error |= clSetKernelArg(ssspKernel2, 3, sizeof(cl_mem), &maskArrayDevice);
    error |= clSetKernelArg(ssspKernel2, 4, sizeof(cl_mem), &costArrayDevice);
    error |= clSetKernelArg(ssspKernel2, 5, sizeof(cl_mem), &updatingCostArrayDevice);
    error |= clSetKernelArg(ssspKernel2, 6, sizeof(int), &graph.vertexCount);
    
    int *maskArrayHost = (int*) malloc(sizeof(int) * graph.vertexCount);
    
    for ( int i = 0 ; i < numSources; i++ )
    {

        error |= clSetKernelArg(initializeBuffersKernel, 3, sizeof(int), &sourceVertices[i]);        
        
        size_t localWorkSize = maxWorkGroupSize;
        size_t globalWorkSize = roundWorkSizeUp(localWorkSize, graph.vertexCount);
        
        error = clEnqueueNDRangeKernel(command_queue, initializeBuffersKernel, 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, NULL);
        
        cl_event readDone;
        error = clEnqueueReadBuffer(command_queue, maskArrayDevice, CL_FALSE, 0, sizeof(int) * graph.vertexCount, maskArrayHost, 0, NULL, &readDone);

        clWaitForEvents(1, &readDone);
        while(!maskArrayEmpty(maskArrayHost, graph.vertexCount))
        {
          for(int asyncIter = 0; asyncIter < NUM_ASYNCHRONOUS_ITERATIONS; asyncIter++)
              {
                  size_t localWorkSize = maxWorkGroupSize;
                  size_t globalWorkSize = roundWorkSizeUp(localWorkSize, graph.vertexCount);
                  
                  error = clEnqueueNDRangeKernel(command_queue, ssspKernel1, 1, 0, &globalWorkSize, &localWorkSize, 0, NULL, NULL);
                  
                  error = clEnqueueNDRangeKernel(command_queue, ssspKernel2, 1, 0, &globalWorkSize, &localWorkSize,0, NULL, NULL);

              }
          error = clEnqueueReadBuffer(command_queue, maskArrayDevice, CL_FALSE, 0, sizeof(int) * graph.vertexCount, maskArrayHost, 0, NULL, &readDone);
          clWaitForEvents(1, &readDone);
        }
        error = clEnqueueReadBuffer(command_queue, costArrayDevice, CL_FALSE, 0, sizeof(float) * graph.vertexCount, &results[i*graph.vertexCount], 0, NULL, &readDone);
        if (error != CL_SUCCESS)
          {
            printf(" Failed to read buffer \n");
            return EXIT_FAILURE;
          }


           
        clWaitForEvents(1, &readDone);
    }
    
for ( int k = 0 ; k < (numSources*graph.vertexCount); k++ )
{
  printf("-->%f\n",results[k]);
}
    
    
    free (maskArrayHost);
    clReleaseMemObject(vertexArrayDevice);
    clReleaseMemObject(edgeArrayDevice);
    clReleaseMemObject(weightArrayDevice);
    clReleaseMemObject(maskArrayDevice);
    clReleaseMemObject(costArrayDevice);
    clReleaseMemObject(updatingCostArrayDevice);
    
    clReleaseKernel(initializeBuffersKernel);
    clReleaseKernel(ssspKernel1);

    clReleaseCommandQueue(command_queue);
    clReleaseProgram(program);
    clReleaseContext(context);
    
    free(sourceVertArray);  
    free(results);       
}
