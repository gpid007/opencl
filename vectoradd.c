/* https://wiki.aalto.fi/display/hpec/opencl+tutorial */
#include <stdio.h>
#include <stdlib.h>
  
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include <CL/cl_ext.h>
 
/* A kernel which does nothing */
const char *source_str =
    "__kernel void vectorAdd(__global const float *a,__global const float *b,__global float *c){int index=get_global_id(0);c[index]=a[index]+b[index];}";
 
int main(void) 
{
    /* Get platform and device information */
    cl_platform_id      platform_id = NULL;
    cl_device_id        device_id = NULL;  
    cl_uint             num_devices;
    cl_uint             num_platforms;
    cl_int              ret;
    cl_mem d_a;
    cl_mem d_b;
    cl_mem d_c;
    float *test;
    float *test2;
    int n,i,bytes;
    n=10000000;
    test=(float*)malloc(n*sizeof(float));
    test2=(float*)malloc(n*sizeof(float));
    for(i=0;i<n;i++){
      test[i]=5*i;
      test2[i]=3*i;
     
    }
    bytes=n*sizeof(float);

    ret = clGetPlatformIDs(
        1
        , &platform_id
        , &num_platforms
    );

    ret = clGetDeviceIDs( 
        platform_id
        , CL_DEVICE_TYPE_GPU
        , 1
        , &device_id
        , &num_devices
    );
  
    /* Create an OpenCL context */
    cl_context context = clCreateContext( 
            NULL
            , 1
            , &device_id
            , NULL
            , NULL
            , &ret
    );
  
    /* Create a command queue */
    cl_command_queue command_queue = clCreateCommandQueue(
        context
        , device_id
        , 0
        , &ret
    );
  
    /* Create a program from the kernel source */
    cl_program program = clCreateProgramWithSource(
        context
        , 1
        , &source_str
        , NULL
        , &ret
    );
  
    /* Build the program */
    ret = clBuildProgram(
        program
        , 1
        , &device_id
        , NULL
        , NULL
        , NULL
    );
  
    /* Create the OpenCL kernel */
    cl_kernel kernel = clCreateKernel(
        program
        , "vectorAdd"
        , &ret
    );
  

    d_a = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
    d_b = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
    d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bytes, NULL, NULL);

    ret = clEnqueueWriteBuffer(command_queue, d_a, CL_TRUE, 0,bytes,test,0,NULL,NULL);    
    ret = clEnqueueWriteBuffer(command_queue, d_b, CL_TRUE, 0,bytes,test2,0,NULL,NULL);

    ret  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
    ret  = clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
    ret  = clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
    

    
    /* Execute the OpenCL kernel */
    size_t global_item_size = 1;
    size_t local_item_size = 1;
    ret = clEnqueueNDRangeKernel(
        command_queue
        , kernel
        , 1
        , NULL
        , &global_item_size
        , &local_item_size
        , 0
        , NULL
        , NULL
    );

    ret=clEnqueueReadBuffer(command_queue, d_c, CL_TRUE, 0, bytes,test,0,NULL,NULL);
    
    /*ret = clFlush(command_queue);
      ret = clFinish(command_queue);*/




    float k=0;
    for(i=0;i<n;i++){k=k+test[i];}
    
    if (ret == CL_SUCCESS)
      {
	
	printf("Success   %f \n",k);
    }
    else
    {
        printf("OpenCL error executing kernel: %d\n", ret);
    }
    /* Clean up */
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    return 0;
}
