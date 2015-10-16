#include "Event.hpp"
#include "SException.hpp"

namespace rustling
{
Event::Event():
    event(NULL)
{}

Event::~Event()
{}

void Event::Wait()
{
    if (!isUsed())
        throw(InvalidEvent());

    auto ret = clWaitForEvents(1, &event);
    if (ret != CL_SUCCESS)
        throw(SException(ret));
}

bool Event::isUsed()
{
    return (event != NULL);
}

timer Event::getTime()
{
    cl_ulong start = 0, end = 0;

    if (!event)
        throw(NoGeneratedEvent());

    auto ret = clWaitForEvents(1, &event);
    if (ret != CL_SUCCESS)
        throw(SException(ret));

    ret = clGetEventProfilingInfo(
              event,
              CL_PROFILING_COMMAND_START,
              sizeof(cl_ulong),
              &start,
              NULL);
    if (ret != CL_SUCCESS)
        throw(SException(ret));

    ret = clGetEventProfilingInfo(
              event,
              CL_PROFILING_COMMAND_END,
              sizeof(cl_ulong),
              &end,
              NULL);
    if (ret != CL_SUCCESS)
        throw(SException(ret));

    return (end - start) * 1e-3;
}
}
