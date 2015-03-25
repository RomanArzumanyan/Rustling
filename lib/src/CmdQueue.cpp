#include "Command.hpp"
#include "CmdQueue.hpp"
#include "SException.hpp"

namespace scow
{
CmdQueue::CmdQueue(
    Context &ctx,
    Device &dev,
    cl_command_queue_properties properties)
{
    ret_code ret;
    queue = clCreateCommandQueue(ctx.getContext(), dev.getID(), properties, &ret);
    if (ret != CL_SUCCESS)
        throw(SException(ret));
}

CmdQueue::~CmdQueue()
{
    clReleaseCommandQueue(queue);
}

cl_command_queue CmdQueue::getQueue()
{
    return queue;
}

CmdQueue& CmdQueue::operator <<(BasicCommand& cmd)
{
    try {
        cmd.execute(*this);
        return *this;
    } catch (SException &e) {
        throw (e);
    }
}

CmdQueue& CmdQueue::operator <<(EventCommand& cmd)
{
    try {
        cmd.execute(*this);
        cmd.unsetEvent();
        return *this;
    } catch (SException &e) {
        throw (e);
    }
}

}
