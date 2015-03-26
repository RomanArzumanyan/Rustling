#pragma once
#include <string>
#include "Common.hpp"

namespace rustling
{
class Device
{
protected:
    cl_device_id device_id;
    Device(cl_device_id & device);

public:
    virtual ~Device();
    virtual cl_device_id const & getID() const;
    virtual std::string getName() const;
};

class GPU: public Device
{
public:
    GPU(cl_device_id& device);
};

class CPU: public Device
{
public:
    CPU(cl_device_id& device);
};
}
