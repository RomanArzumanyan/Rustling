#include "DeviceMgr.hpp"
#include "PlatformMgr.hpp"
#include "SException.hpp"
using namespace std;

namespace rustling
{

std::vector<cl_device_type> const DeviceMgr::supported_devices(
{
    // At present time, only CPU & GPU are supported
    CL_DEVICE_TYPE_CPU,
    CL_DEVICE_TYPE_GPU
});

DeviceMgr::DeviceMgr()
{
    // Check that all entries from DeviceMgr::supported_devices are added
    add<CPU>(CL_DEVICE_TYPE_CPU);
    add<GPU>(CL_DEVICE_TYPE_GPU);
}

DeviceMgr::~DeviceMgr()
{
    for (auto &factory : factory_map)
        delete factory.second;
}

cl_device_type DeviceMgr::checkType(cl_device_id& device_id)
{
    cl_device_type type;

    auto ret = clGetDeviceInfo(
                       device_id,
                       CL_DEVICE_TYPE,
                       sizeof(cl_device_type),
                       &type,
                       NULL);

    if (ret != CL_SUCCESS)
        throw(SException(ret));

    return type;
}

vector<cl_device_type> const& DeviceMgr::getSupportedTypes()
{
    return supported_devices;
}

Device* DeviceMgr::create(cl_device_type type, cl_device_id& id)
{
    auto it = factory_map.find(type);
    if (it != factory_map.end())
        return it->second->create(id);

    throw (InvalidDeviceType());
}

Device DeviceMgr::makeDevice(cl_device_type type)
{
    try {
        Platform plat = PlatformMgr::makePlatform(type);
        return makeDevice(plat, type);
    } catch (SException &e) {
        throw e;
    }
}

Device DeviceMgr::makeDevice(Platform &platform, cl_device_type type)
{
    try {
        auto devices = platform.getDevices();
        for (auto device = devices.begin(); device != devices.end(); device++)
            if (DeviceMgr::checkType(*device) == type)
                return *create(type, *device);
    } catch (SException &e) {
        throw (e);
    }
}

std::vector<Device> DeviceMgr::makeDevices(cl_device_type type)
{
    std::vector<Device> result, temp;
    try {
        for (auto plat : PlatformMgr::makePlatforms(type)) {
            temp = makeDevices(plat, type);
            result.insert(result.end(), temp.begin(), temp.end());
        }
    } catch (SException &e) {
        throw (e);
    }

    return result;
}

std::vector<Device> DeviceMgr::makeDevices(Platform &platform, cl_device_type type)
{
    std::vector<Device> result;
    try {
        for (auto &device: platform.getDevices())
            if (DeviceMgr::checkType(device) == type)
                    result.push_back(*create(type, device));

        return result;
    } catch (SException &e) {
        throw (e);
    }
}

}
