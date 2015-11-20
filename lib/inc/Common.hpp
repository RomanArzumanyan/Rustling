#pragma once
#define __CL_ENABLE_EXCEPTIONS

#include <CL/cl.h>

typedef cl_int ret_code;
typedef cl_ulong timer;

namespace rustling{
    enum class OpenCL_Side
    {
        HOST,
        DEVICE
    };
}