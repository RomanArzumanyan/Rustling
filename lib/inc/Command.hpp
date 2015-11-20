#pragma once
#include "Common.hpp"

namespace rustling
{
class CmdQueue;
class Event;

class BasicCommand
{
protected:
    BasicCommand();
public:
    virtual ~BasicCommand();
    virtual void execute(CmdQueue &queue) = 0;
    virtual void operator()(CmdQueue &queue);
};

class BlockingCommand: virtual public BasicCommand
{
protected:
    bool blocking;
    BlockingCommand(bool is_blocking = false);
public:
    virtual ~BlockingCommand();
    virtual bool isBlocking() const;
    virtual BlockingCommand& operator()(bool is_blocking) final
    {
        blocking = is_blocking;
        return *this;
    }
};

class EventCommand: virtual public BasicCommand
{
    friend class CmdQueue;
    friend class Event;
protected:
    Event *p_event;
    EventCommand();
    virtual void unsetEvent();
    virtual cl_event& getEventToBind();
public:
    virtual ~EventCommand();
    virtual EventCommand& operator >>(Event &event) final
    {
        p_event = &event;
        return *this;
    }
};

class ComplexCommand : 
    virtual public EventCommand, 
    virtual public BlockingCommand
{};

class Flush: public BasicCommand
{
public:
    Flush();
    void execute(CmdQueue &queue);
};

class Finish: public BasicCommand
{
public:
    Finish();
    void execute(CmdQueue &queue);
};

extern Flush flush;
extern Finish finish;
}
