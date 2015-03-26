#include <string>
#include "Device.hpp"
#include "DeviceMgr.hpp"
#include "SException.hpp"

using namespace std;

namespace rustling
{
static size_t getDevFeatureSize(
    cl_device_id device,
    cl_device_info info)
{
    size_t size;
    ret_code ret = clGetDeviceInfo(device, info, 0, NULL, &size);
    if (ret != CL_SUCCESS)
        throw(InvalidDeviceParamInfo());

    return size;
}

static void getDeviceFeature(
    cl_device_id device,
    cl_device_info info,
    size_t param_size,
    void* dest)
{
    ret_code ret = clGetDeviceInfo(device, info, param_size, dest, NULL);
    if (ret != CL_SUCCESS)
        throw(InvalidDeviceParamInfo());
}

Device::Device(cl_device_id& device):
    device_id(device)
{}

Device::~Device()
{}

cl_device_id const & Device::getID() const
{
    return device_id;
}

std::string Device::getName() const
{
    size_t name_len;
    char *name_str = 0;

    try {
        name_len = getDevFeatureSize(device_id, CL_DEVICE_NAME);
        name_str = (char*)calloc(name_len + 1, sizeof(*name_str));
        getDeviceFeature(device_id, CL_DEVICE_NAME, name_len, (void*)name_str);
    } catch (SException &e) {
        if (name_str)
            free(name_str);
        throw e;
    }

    string name(name_str);
    free(name_str);
    return name;
}

GPU::GPU(cl_device_id& device):
    Device(device)
{
    if (CL_DEVICE_TYPE_GPU != DeviceMgr::checkType(device_id)) {
        throw (InvalidDeviceType());
    }
}

CPU::CPU(cl_device_id& device) :
    Device(device)
{
    if (CL_DEVICE_TYPE_CPU != DeviceMgr::checkType(device_id)) {
        throw (InvalidDeviceType());
    }
}
}
