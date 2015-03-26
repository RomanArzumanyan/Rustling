#pragma once
#include <vector>
#include <map>
#include "Common.hpp"
#include "Device.hpp"
#include "Platform.hpp"

namespace rustling
{
class abstractDeviceCreator
{
public:
    abstractDeviceCreator() {}
    virtual Device * create(cl_device_id & id) const = 0;
};

template <class C>
class DeviceCreator : public abstractDeviceCreator
{
public:
    virtual Device* create(cl_device_id& id) const
    {
        return new C(id);
    }
};

class DeviceMgr
{
protected:
    static std::vector<cl_device_type> const supported_devices;
    typedef std::map<cl_device_type, abstractDeviceCreator*> devmgr_map;
    devmgr_map factory_map;
    Device* create(cl_device_type type, cl_device_id& id);
    template<class C>
    void add(cl_device_type type)
    {
        typename devmgr_map::iterator it = factory_map.find(type);
        if (it == factory_map.end())
            factory_map[type] = new DeviceCreator<C>();
    }

public:
    DeviceMgr();
    virtual ~DeviceMgr();
    static cl_device_type checkType(cl_device_id& device_id);
    static std::vector<cl_device_type> const& getSupportedTypes();
    Device makeDevice(cl_device_type type);
    Device makeDevice(Platform &paltform, cl_device_type type);
    std::vector<Device> makeDevices(cl_device_type type);
    std::vector<Device> makeDevices(Platform &paltform, cl_device_type type);
};
}
