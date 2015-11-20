#include "Context.hpp"
#include <SException.hpp>

namespace rustling
{
Context::Context(
    std::initializer_list<Device> devices,
    const cl_context_properties *properties,
    pfn_notify callback,
    void *user_data)
{
    *this = Context(std::vector<Device>(devices),
        properties,
        callback,
        user_data);
}

Context::Context(
        std::vector<Device> devices,
        const cl_context_properties *properties,
        pfn_notify callback,
        void *user_data)
{
    auto i = 0;
    ret_code ret;
    cl_device_id *devs = (cl_device_id*)calloc(devices.size(), sizeof(*devs));

    for (auto device : devices)
        devs[i++] = device.getID();

    context = clCreateContext(
                  properties,
                  devices.size(),
                  devs,
                  callback,
                  user_data,
                  &ret);
    free(devs);

    if (ret != CL_SUCCESS)
        throw(SException(ret));
}

Context::Context(Context const &other)
{
    this->context = other.context;
    auto ret = clRetainContext(this->context);
    if (ret != CL_SUCCESS)
        throw(SException(ret));
}

Context Context::operator=(Context const &other)
{
    this->context = other.context;
    auto ret = clRetainContext(this->context);
    if (ret != CL_SUCCESS)
        throw(SException(ret));

    return *this;
}

Context::~Context()
{
    clReleaseContext(context);
}

cl_context& Context::getContext()
{
    return context;
}
}
