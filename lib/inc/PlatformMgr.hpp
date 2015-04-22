#pragma once
#include <map>
#include <vector>
#include <string>
#include "Common.hpp"
#include "Platform.hpp"

namespace rustling
{
class PlatformMgr
{
protected:
    static std::vector<cl_platform_id> platforms;

public:
    static std::size_t getNumPlatforms();
    static Platform makePlatform(cl_device_type type);
    static std::vector<Platform> makePlatforms(cl_device_type type);
    static std::map<cl_device_type, cl_uint> checkDevices(cl_platform_id platform);
};
}
