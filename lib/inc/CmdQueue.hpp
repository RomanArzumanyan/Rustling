#pragma once
#include "Common.hpp"
#include "Context.hpp"
#include "Device.hpp"

namespace rustling
{
class BasicCommand;
class EventCommand;

class CmdQueue
{
protected:
    cl_command_queue queue;
public:
    CmdQueue();
    CmdQueue(
        Context &ctx,
        Device &dev,
        cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE);
    CmdQueue(const CmdQueue& other);
    CmdQueue& operator=(const CmdQueue& other);
    ~CmdQueue();
    cl_command_queue getQueue();
    CmdQueue& operator <<(BasicCommand& cmd);
    CmdQueue& operator <<(EventCommand& cmd);
};
}
