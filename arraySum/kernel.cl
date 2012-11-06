#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

__kernel void arraysum(__global const float *a, __global const float *b, __global float *result, int size_arr)
{   
    int j;
    const int i = size_arr*get_global_id(0);
      for (j=0; j < size_arr; j++)
      {
          result[i+j] = (a[i+j] *2*4.5) + (b[i+j]/a[i+j]);
      }
}
