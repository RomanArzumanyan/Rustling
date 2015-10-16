#include "SException.hpp"
#include <sstream>
using namespace std;

namespace rustling
{
SException::SException(cl_int err, const char * errStr) :
    std::exception(),
    err_code(err)
{
    if(errStr)
        message = string(errStr);
}

const char *SException::what()
{
    std::stringstream ss;
    ss << err_code << " " << message;
    return ss.str().c_str();
}

NoPlatformFound::NoPlatformFound() :
    SException(CL_INVALID_PLATFORM, "No OpenCL platform(s) found.")
{}

NoDeviceFound::NoDeviceFound() :
    SException(CL_INVALID_DEVICE, "No OpenCL device(s) found")
{}

InvalidDeviceType::InvalidDeviceType() :
    SException(CL_INVALID_DEVICE, "Invalid OpenCL device")
{}

InvalidPlatform::InvalidPlatform():
    SException(CL_INVALID_PLATFORM, "Invalid OpenCL platform")
{}

InvalidDeviceParamInfo::InvalidDeviceParamInfo():
    SException(CL_INVALID_PLATFORM, "Invalid OpenCL device parameters info")
{}

InvalidBlockingOpStatus::InvalidBlockingOpStatus():
    SException(CL_INVALID_PROPERTY, "Invalid blocking status for command in queue")
{}

CantBindEvent::CantBindEvent():
    SException(CL_INVALID_EVENT, "Can't bind event to command")
{}

NoGeneratedEvent::NoGeneratedEvent():
    SException(CL_INVALID_EVENT, "No OpenCL exception")
{}

InvalidEvent::InvalidEvent():
    SException(CL_INVALID_EVENT, "Invalid OpeNCL event")
{}

MemObjectNotMapped::MemObjectNotMapped():
    SException(CL_INVALID_OPERATION, "Memory object not mapped")
{}

InvalidViewSize::InvalidViewSize():
    SException(CL_INVALID_VALUE, "Invalid view sizes")
{}

KernelFileNotFound::KernelFileNotFound():
    SException(CL_INVALID_VALUE, "Invalid kernel file")
{}

InvalidLocalGroupSize::InvalidLocalGroupSize():
    SException(CL_INVALID_WORK_GROUP_SIZE, "Invalid kernel local group size")
{}

InvalidGlobalGroupSize::InvalidGlobalGroupSize():
    SException(CL_INVALID_WORK_DIMENSION, "Invalid kernel work dimension")
{}

MemObjectsSizeMismatch::MemObjectsSizeMismatch():
    SException(CL_INVALID_VALUE, "Memory objects size mismatch")
{}

InvalidKernelArgs::InvalidKernelArgs():
    SException(CL_INVALID_VALUE, "Invalid kernel arguments list")
{}

}
