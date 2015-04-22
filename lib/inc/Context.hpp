#pragma once
#include <initializer_list>
#include "Common.hpp"
#include "Device.hpp"

namespace rustling
{

typedef void (CL_CALLBACK *pfn_notify)(
    const char *errinfo,
    const void *private_info,
    size_t cb,
    void *user_data);

class Context
{
protected:
    cl_context context;

public:
    Context(
        std::initializer_list<Device> devices,
        const cl_context_properties *properties = 0x0,
        pfn_notify callback = 0x0,
        void *user_data = 0x0);
    ~Context();
    cl_context& getContext();
};
}
