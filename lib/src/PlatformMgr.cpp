#include "PlatformMgr.hpp"
#include "SException.hpp"
#include "DeviceMgr.hpp"

using namespace std;

namespace rustling
{
typedef std::vector<Platform>::iterator platform_it;

static cl_uint GetNumPlatforms(void)
{
    cl_uint num_platforms = 0;
    ret_code ret = clGetPlatformIDs(0, (cl_platform_id *)NULL, &num_platforms);

    if (ret != CL_SUCCESS)
        throw(SException(ret));

    return num_platforms;
}

static vector<cl_platform_id> collectPlatformsList()
{
    cl_uint num_pl = 0;
    try {
        num_pl = GetNumPlatforms();
    } catch (SException &e) {
        throw e;
    }

    vector<cl_platform_id> platforms(num_pl);
    ret_code ret = clGetPlatformIDs(platforms.size(), &platforms[0], NULL);
    if (ret != CL_SUCCESS)
        throw(SException(ret));

    return platforms;
}

std::vector<cl_platform_id> PlatformMgr::platforms = collectPlatformsList();

map<cl_device_type, cl_uint> PlatformMgr::checkDevices(cl_platform_id platform)
{
    map<cl_device_type, cl_uint> devices;
    ret_code ret = CL_SUCCESS;

    for (auto dev_type : DeviceMgr::getSupportedTypes()) {
        cl_uint num_devs = 0;
        ret = clGetDeviceIDs(platform, dev_type, 0, NULL, &num_devs);

        if (ret != CL_SUCCESS)
            throw(SException(ret));

        if (num_devs)
            devices.emplace(dev_type, num_devs);
    }

    if (devices.begin() == devices.end())
        throw(InvalidPlatform());

    return devices;
}

size_t PlatformMgr::getNumPlatforms()
{
    return platforms.size();
}

Platform PlatformMgr::makePlatform(cl_device_type type)
{
    try {
        for (auto platform : platforms) {
            auto devices = checkDevices(platform);
            if (devices.find(type)->second)
                return Platform(platform);
        }
    } catch (SException &e) {
        throw e;
    }

    throw (InvalidDeviceType());
}

vector<Platform> PlatformMgr::makePlatforms(cl_device_type type)
{
    vector<Platform> plat;
    try {
        for (auto platform : platforms) {
            auto devices = checkDevices(platform);
            if (devices.find(type)->second)
                plat.push_back(Platform(platform));
        }
    } catch (SException &e) {
        throw (e);
    }

    if (plat.begin() == plat.end())
        throw(InvalidDeviceType());

    return plat;
}

}
