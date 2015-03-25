#include "Kernel.hpp"
#include "Context.hpp"
#include "Device.hpp"
#include "SException.hpp"
#include <iostream>

using namespace std;
using namespace scow;

static char* Read_Source_File(const char *filename)
{
    long int size = 0, res = 0;

    char *src = NULL;

    FILE *file = fopen(filename, "rb");

    if (!file)
        return NULL;

    if (fseek(file, 0, SEEK_END)) {
        fclose(file);
        return NULL;
    }

    size = ftell(file);
    if (size == 0) {
        fclose(file);
        return NULL;
    }

    rewind(file);

    src = (char *) calloc(size + 1, sizeof(char));
    if (!src) {
        src = NULL;
        fclose(file);
        return src;
    }

    res = fread(src, 1, sizeof(char) * size, file);
    if (res != sizeof(char) * size) {
        fclose(file);
        free(src);

        return (char*)NULL;
    }

    src[size] = '\0'; /* NULL terminated */
    fclose(file);

    return src;
}

Kernel::Kernel(
    Context &ctx,
    Device& device,
    const string kernel_name,
    const string kernel_path,
    const string parameters):

    name(kernel_name)
{
    ret_code ret = CL_SUCCESS;
    char* source = Read_Source_File(kernel_path.c_str());

    if (!source)
        throw(KernelFileNotFound());

    program = clCreateProgramWithSource(
                  ctx.getContext(),
                  1,
                  const_cast<const char**>(&source),
                  NULL,
                  &ret);

    free(source);

    ret |= clBuildProgram(
               program,
               0,
               NULL,
               parameters.c_str(),
               NULL,
               NULL);

    if (ret != CL_SUCCESS) {

        size_t len = 0;
        char *buffer;

        clGetProgramBuildInfo(
            program,
            device.getID(),
            CL_PROGRAM_BUILD_LOG,
            0,
            NULL,
            &len);

        buffer = (char*)calloc(len, sizeof(char));

        clGetProgramBuildInfo(
            program,
            device.getID(),
            CL_PROGRAM_BUILD_LOG,
            len,
            buffer,
            NULL);

        clReleaseProgram(program);
        cerr << buffer << endl;
        free(buffer);
        throw ret;
    }

    kernel = clCreateKernel(program, name.c_str(), &ret);
    if (ret != CL_SUCCESS)
        throw(ret);
}

Kernel::~Kernel()
{
    clReleaseProgram(program);
    clReleaseKernel(kernel);
}

EnqueueKernel Kernel::operator()(
    initializer_list<size_t> global_wsize,
    initializer_list<size_t> local_wsize,
    initializer_list<AbstractKernelArg*> enq_args,
    cl_uint num_events_in_wait_list,
    const size_t* global_work_offset,
    const cl_event* enq_event_wait_list)
{
    if (!global_wsize.size())
        throw(InvalidGlobalGroupSize());

    if (local_wsize.size() && local_wsize.size() != global_wsize.size())
        throw(InvalidLocalGroupSize());

    return EnqueueKernel(
               kernel,
               enq_args,
               global_wsize,
               local_wsize,
               global_work_offset,
               num_events_in_wait_list,
               enq_event_wait_list);
}

EnqueueKernel::~EnqueueKernel()
{}

EnqueueKernel::EnqueueKernel(
    cl_kernel& enq_kernel,
    initializer_list<AbstractKernelArg*> enq_kernel_args,
    initializer_list<size_t> enq_global_work_size,
    initializer_list<size_t> enq_local_work_size,
    const size_t* enq_global_work_offset,
    cl_uint enq_num_events_in_wait_list,
    const cl_event* enq_event_wait_list):

    kernel(enq_kernel),
    args(enq_kernel_args),
    work_dim(enq_global_work_size.size()),
    global_work_offset(enq_global_work_offset),
    global_work_size(enq_global_work_size),
    local_work_size(enq_local_work_size),
    num_events_in_wait_list(enq_num_events_in_wait_list),
    event_wait_list(enq_event_wait_list)
{}

void EnqueueKernel::execute(CmdQueue &queue)
{
    cl_event* p_evt = static_cast<cl_event*>(NULL);
    if (p_event)
        p_evt = &getEventToBind();

    ret_code ret = CL_SUCCESS;
    cl_uint index = 0;

    // Range-based loop is unapplicable
    for (auto arg = args.begin(); arg != args.end(); arg++) {
        ret = clSetKernelArg(
                  kernel,
                  index++,
                  (*arg)->getSize(),
                  (*arg)->getPtr());

        if (ret != CL_SUCCESS)
            throw(SException(ret));
    }

    ret = clEnqueueNDRangeKernel(
              queue.getQueue(),
              kernel,
              work_dim,
              global_work_offset,
              &global_work_size[0],
              local_work_size.size() ? &local_work_size[0] : (size_t*)NULL,
              num_events_in_wait_list,
              event_wait_list,
              p_evt);

    if (ret != CL_SUCCESS)
        throw(SException(ret));

    cout << args.size() << endl;
    for (auto arg : args)
        free(arg);
}

EnqueueKernel& EnqueueKernel::operator >>(Event &event)
{
    p_event = &event;
    return *this;
}


AbstractKernelArg::AbstractKernelArg()
{}

AbstractKernelArg::~AbstractKernelArg()
{}
