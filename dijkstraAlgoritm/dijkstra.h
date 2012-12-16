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
  if (error == CL_OUT_OF_HOST_MEMORY)
  {
    context = createContext();
  }
  else if (error != CL_SUCCESS)
  {
    printf(" Error at clCreateContext! \n");
    printf("%s\n", oclErrorString(error));
    return NULL;
  }
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