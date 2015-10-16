#include "Command.hpp"
#include "CmdQueue.hpp"
#include "SException.hpp"
#include "Event.hpp"

namespace rustling
{
BasicCommand::BasicCommand()
{}

BasicCommand::~BasicCommand()
{}

void BasicCommand::operator()(CmdQueue& queue)
{
    return execute(queue);
}

BlockingCommand::BlockingCommand(bool is_blocking):
    blocking(is_blocking)
{}

BlockingCommand::~BlockingCommand()
{}

bool BlockingCommand::isBlocking() const
{
    return blocking;
}

Flush::Flush():
    BasicCommand()
{}

EventCommand::EventCommand():
    p_event(static_cast<Event*>(NULL))
{}

EventCommand::~EventCommand()
{}

void EventCommand::unsetEvent()
{
    p_event = 0x0;
}

cl_event& EventCommand::getEventToBind()
{
    if (p_event && !p_event->event)
        return p_event->event;

    throw (CantBindEvent());
}

void Flush::execute(CmdQueue &queue)
{
    auto ret = clFlush(queue.getQueue());
    if (ret != CL_SUCCESS)
        throw (SException(ret));
}
Finish::Finish():
    BasicCommand()
{}

void Finish::execute(CmdQueue &queue)
{
    auto ret = clFinish(queue.getQueue());
    if (ret != CL_SUCCESS)
        throw (SException(ret));
}

Flush flush;
Finish finish;
}
