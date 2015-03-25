#pragma once
#include "Common.hpp"
#include "Command.hpp"
#include "CmdQueue.hpp"

namespace rustling
{
class Event
{
    friend cl_event& EventCommand::getEventToBind();
protected:
    cl_event event;
public:
    Event();
    virtual ~Event();
    virtual void Wait();
    virtual bool isUsed();
    virtual timer getTime();
};

class UserEvent: public Event
{
public:
    UserEvent();
    virtual ~UserEvent();
};
}
