#include <map>
#include "Platform.hpp"
#include "PlatformMgr.hpp"
#include "SException.hpp"
using namespace std;

namespace rustling
{
static string getParamString(
    cl_platform_id platform,
    cl_platform_info info_wanted)
{
    size_t size;
    cl_int ret = clGetPlatformInfo(platform, info_wanted, 0, NULL, &size);
    if (ret != CL_SUCCESS)
        throw(SException(ret));

    char *str = (char*)calloc(1 + size, sizeof(*str));

    ret = clGetPlatformInfo(platform, info_wanted, size, str, NULL);
    if (ret != CL_SUCCESS) {
        free(str);
        throw (SException(ret));
    }

    string result(str);
    free(str);
    return result;
}

Platform::Platform(cl_platform_id& platform):
    platform_id(platform),
    devices(0)
{}

cl_platform_id const& Platform::getID() const
{
    return platform_id;
}

bool Platform::operator ==(const Platform& rhs)
{
    return (getID() == rhs.getID());
}

string Platform::getProperty(cl_platform_info info) const
{
    string name;
    try {
        name = getParamString(platform_id, info);
    } catch (SException &e) {
        throw e;
    }

    return name;
}

string Platform::getProfile() const
{
    return getProperty(CL_PLATFORM_PROFILE);
}

string Platform::getVersion() const
{
    return getProperty(CL_PLATFORM_VERSION);
}

string Platform::getName() const
{
    return getProperty(CL_PLATFORM_NAME);
}

string Platform::getVendor() const
{
    return getProperty(CL_PLATFORM_VENDOR);
}

string Platform::getExtensions() const
{
    return getProperty(CL_PLATFORM_EXTENSIONS);
}

std::vector<cl_device_id>& Platform::getDevices()
{
    if (devices.begin() != devices.end())
        return devices;

    try {
        auto devmap = PlatformMgr::checkDevices(platform_id);

        for (auto& dev : devmap) {
            vector<cl_device_id> temp(dev.second);
            ret_code ret = clGetDeviceIDs(platform_id,
                                          dev.first,
                                          dev.second,
                                          (cl_device_id*)&temp[0],
                                          NULL);
            if (ret != CL_SUCCESS)
                throw(InvalidDeviceType());

            devices.insert(devices.end(), temp.begin(), temp.end());
        }

        return devices;
    } catch (SException &e) {
        throw (e);
    }

    throw (InvalidPlatform());
}
}
