#include <stdlib.h>
#include <stdio.h>       
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

int main() {

    int i, j;
    char* value;
    size_t valueSize;
    cl_uint platformCount;
    cl_platform_id* platforms;
    cl_uint deviceCount;
    cl_device_id* devices;
    cl_uint maxComputeUnits;                                         

    // get all platforms
    clGetPlatformIDs(0, NULL, &platformCount);
    platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount);
    clGetPlatformIDs(platformCount, platforms, NULL);

    for (i = 0; i < platformCount; i++) {
          
         // print platform name
         clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 0, NULL, &valueSize);
	       value = (char*) malloc(valueSize);
	       clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, valueSize, value, NULL);
	       printf("%d.%d Platform name: %s\n", i+1, 1, value);
	       free(value);
	       
	       // print platform name
         clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, 0, NULL, &valueSize);
	       value = (char*) malloc(valueSize);
	       clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, valueSize, value, NULL);
	       printf("%d.%d Platform Vendor: %s\n", i+1, 2, value);
	       free(value);
	       
	       // print platform name
         clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, 0, NULL, &valueSize);
	       value = (char*) malloc(valueSize);
	       clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, valueSize, value, NULL);
	       printf("%d.%d Platform version: %s\n", i+1, 3, value);
	       free(value);

        // get all devices
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
        devices = (cl_device_id*) malloc(sizeof(cl_device_id) * deviceCount);
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL);

        // for each device print critical attributes
        for (j = 0; j < deviceCount; j++) {

            // print device name
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, valueSize, value, NULL);
            printf("%d.%d Device: %s\n", i+1, j+4, value);
            free(value);

            // print hardware device version
            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, valueSize, value, NULL);
            printf(" %d.%d Hardware version: %s\n", j+4, 1, value);
            free(value);

            // print hardware device id
            cl_uint vendor_id;
            clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR_ID, sizeof(vendor_id), &vendor_id, NULL);
            printf(" %d.%d Device vendor id: %u\n", j+4, 2, vendor_id);

            // print hardware device type
            cl_device_type device_type;
            clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL);
            switch (device_type)
                {
                case CL_DEVICE_TYPE_GPU:
                printf(" %d.%d Device type: CL_DEVICE_TYPE_GPU\n", j+4, 3);
                break;
                case CL_DEVICE_TYPE_CPU:
                printf(" %d.%d Device type: CL_DEVICE_TYPE_CPU\n", j+4, 3);
                break;
                case CL_DEVICE_TYPE_ACCELERATOR:
                printf(" %d.%d Device type: CL_DEVICE_TYPE_ACCELERATOR\n", j+4, 3);
                break;
                case CL_DEVICE_TYPE_DEFAULT:
                printf(" %d.%d Device type: CL_DEVICE_TYPE_DEFAULT\n", j+4, 3);
                break;
                }

            // print software driver version
            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, valueSize, value, NULL);
            printf(" %d.%d Software version: %s\n", j+4, 4, value);
            free(value);

            // print c version supported by compiler for device
            clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, sizeof(valueSize), &valueSize, NULL);
            printf(" %d.%d OpenCL C version: %u\n", j+4, 5, valueSize);
            free(value);   
            
            // print parallel compute units
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS,sizeof(maxComputeUnits), &maxComputeUnits, NULL);
            printf(" %d.%d Parallel compute units: %d\n", j+4, 6, maxComputeUnits);
            
            // print device max work item dimensions
            size_t workitem_dims;
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(workitem_dims), &workitem_dims, NULL);
            printf(" %d.%d Device max work item dimensions: %u\n", j+4, 7, workitem_dims);
            
            // print devices max work item sizes
            size_t workitem_size[3];
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workitem_size), &workitem_size, NULL);
            printf(" %d.%d Device max work item sizes: %u %u %u\n", i+4, 8,workitem_size[0], workitem_size[1], workitem_size[2]);
            
            // print device max work group size
            size_t workgroup_size;
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(workgroup_size), &workgroup_size, NULL);
            printf(" %d.%d Device max work group size: %u\n", j+4, 9, workgroup_size);            
            
            // print device max clock frequency
            cl_uint clock_frequency;
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clock_frequency), &clock_frequency, NULL);
            printf(" %d.%d Device max clock frequency: %u MHz\n", j+4, 10, clock_frequency);

            // print device address bits
            cl_uint addr_bits;
            clGetDeviceInfo(devices[j], CL_DEVICE_ADDRESS_BITS, sizeof(addr_bits), &addr_bits, NULL);
            printf(" %d.%d Device address bits: %u\n", j+4, 11, addr_bits);

            // print device max mem alloc size
            cl_ulong max_mem_alloc_size;
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(max_mem_alloc_size), &max_mem_alloc_size, NULL);
            printf(" %d.%d Device max mem alloc size: %u MByte\n", j+4, 12, (unsigned int)(max_mem_alloc_size / (1024 * 1024)));

            // print device global mem size
            cl_ulong mem_size;
            clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
            printf(" %d.%d Device global mem size: %u MByte\n", j+4, 13, (unsigned int)(mem_size / (1024 * 1024)));

            // print device error correction support
            cl_bool error_correction_support;
            clGetDeviceInfo(devices[j], CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(error_correction_support), &error_correction_support, NULL);
            printf(" %d.%d Device error correction support %s\n", j+4, 14, error_correction_support == CL_TRUE ? "yes" : "no");

            // print device local memory type
            cl_device_local_mem_type local_mem_type;
            clGetDeviceInfo(devices[j], CL_DEVICE_LOCAL_MEM_TYPE, sizeof(local_mem_type), &local_mem_type, NULL);
            printf(" %d.%d Device local memory type: t%s\n", j+4, 15, local_mem_type == 1 ? "local" : "global");

            // print device local memory size
            clGetDeviceInfo(devices[j], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
            printf(" %d.%d Device local memory size: %u KByte\n", j+4, 16, (unsigned int)(mem_size / 1024));

            // print device mac constant buffer size
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(mem_size), &mem_size, NULL);
            printf(" %d.%d Device mac constant buffer size: %u KByte\n", j+4, 17, (unsigned int)(mem_size / 1024));

        }
        
        printf("====\n");

    }

    free(platforms);
    return 0;

}
