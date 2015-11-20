#include "Command.hpp"
#include "CmdQueue.hpp"
#include "SException.hpp"

namespace rustling
{
CmdQueue::CmdQueue():
    queue(0x0)
{}

CmdQueue::CmdQueue(
    Context &ctx,
    Device &dev,
    cl_command_queue_properties properties)
{
    ret_code ret;

    auto context_ = ctx.getContext();
    auto device_ = dev.getID();
    queue = clCreateCommandQueue(ctx.getContext(), dev.getID(), properties, &ret);
    if (ret != CL_SUCCESS)
        throw(SException(ret));
}

CmdQueue::CmdQueue(const CmdQueue &other):
    queue(other.queue)
{
    auto ret = clRetainCommandQueue(queue);
    if(ret!=CL_SUCCESS)
        throw(SException(ret));
}

CmdQueue &CmdQueue::operator=(const CmdQueue &other)
{
    this->queue=other.queue;
    auto ret = clRetainCommandQueue(queue);
    if(ret!=CL_SUCCESS)
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
