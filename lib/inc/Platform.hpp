#pragma once
#include <string>
#include <vector>
#include "Common.hpp"

namespace rustling
{
class Platform
{
protected:
    cl_platform_id platform_id;
    std::vector<cl_device_id> devices;
    std::string getProperty(cl_platform_info info) const;

public:
    Platform(cl_platform_id& platform);
    cl_platform_id const& getID() const;
    bool operator ==(const Platform& rhs);
    std::string getProfile() const;
    std::string getVersion() const;
    std::string getName() const;
    std::string getVendor() const;
    std::string getExtensions() const;
    std::vector<cl_device_id>& getDevices();
};
}
