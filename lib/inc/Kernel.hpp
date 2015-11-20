#pragma once
#include <string>
#include <initializer_list>
#include "Common.hpp"
#include "Command.hpp"
#include "MemObject.hpp"

namespace rustling
{
class Context;
class Device;
class AbstractKernelArg
{
protected:
    void* ptr;
    size_t size;
    AbstractKernelArg();
    virtual ~AbstractKernelArg();
public:
    virtual void* const getPtr() = 0;
    virtual size_t getSize() = 0;
};

template<class T>
class KernelArg: public AbstractKernelArg
{
public:
    KernelArg(T& arg)
    {
        this->ptr = static_cast<void*>(&arg);
        this->size = sizeof(arg);
    }

    ~KernelArg()
    {}

    void* const getPtr()
    {
        return ptr;
    }

    size_t getSize()
    {
        return size;
    }
};

template<class T>
class KernelArg<Buffer<T>>: public AbstractKernelArg
{
public:
    KernelArg(Buffer<T>& arg)
    {
        this->ptr = static_cast<void*>(&arg.getMemObject());
        this->size = sizeof(arg.getMemObject());

        if(this->ptr == static_cast<cl_mem>(NULL))
            throw(SException(-1, "Void OpenCL memory object"));
        if(this->size==0)
            throw(SException(-1, "Zero OpenCL memory object size"));
    }

    ~KernelArg()
    {}

    void* const getPtr()
    {
        return ptr;
    }

    size_t getSize()
    {
        return size;
    }
};

template<class T>
class KernelArg<SubBuffer<T>>: public AbstractKernelArg
{
public:
    KernelArg(SubBuffer<T>& arg)
    {
        this->ptr = static_cast<void*>(&arg.getMemObject());
        this->size = sizeof(arg.getMemObject());

        if(this->ptr == static_cast<cl_mem>(NULL))
            throw(SException(-1, "Void OpenCL memory object"));
        if(this->size==0)
            throw(SException(-1, "Zero OpenCL memory object size"));
    }

    ~KernelArg()
    {}

    void* const getPtr()
    {
        return ptr;
    }

    size_t getSize()
    {
        return size;
    }
};

class EnqueueKernel: public EventCommand
{
protected:
    const cl_uint work_dim, num_events_in_wait_list;
    const std::size_t *global_work_offset;
    std::vector<size_t> global_work_size, local_work_size;
    const cl_event *event_wait_list;
    cl_kernel& kernel;
    const std::initializer_list<AbstractKernelArg*> args;

public:
    EnqueueKernel(cl_kernel& enq_kernel,
        std::initializer_list<AbstractKernelArg*> enq_kernel_args,
        std::initializer_list<size_t> enq_global_work_size,
        std::initializer_list<size_t> enq_local_work_size,
        const size_t *enq_global_work_offset,
        cl_uint enq_num_events_in_wait_list,
        const cl_event* enq_event_wait_list);
    ~EnqueueKernel();
    void execute(CmdQueue& queue);
    //EnqueueKernel& operator >>(Event& event);
};

class Kernel
{
protected:
    std::string name;
    cl_kernel kernel;
    cl_program program;

public:
    Kernel();

    Kernel(const Kernel& other);

    Kernel operator=(const Kernel& other);

    Kernel(
        Context& ctx,
        Device& device,
        const std::string kernel_name,
        const std::string kernel_src,
        const std::string parameters = "",
        bool from_string=false);

    virtual ~Kernel();

    EnqueueKernel operator()(
        std::initializer_list<size_t> global_wsize,
        std::initializer_list<size_t> local_wsize,
        std::initializer_list<AbstractKernelArg*> args,
        cl_uint num_events_in_wait_list = 0,
        const size_t* global_work_offset = static_cast<size_t*>(NULL),
        const cl_event* enq_event_wait_list = static_cast<cl_event*>(NULL));

    const std::string getName() const;
};
}
