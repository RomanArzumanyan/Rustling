#pragma once
#include "Command.hpp"
#include "CmdQueue.hpp"
#include "SException.hpp"

namespace rustling
{
template<class T>
class Buffer;

template<class T>
class MemObject;

template<class T>
class ReadBuffer: public BlockingCommand, public EventCommand
{
protected:
    Buffer<T>& buffer;
    T* dest;
    size_t offset;
    size_t size;
    cl_uint num_events_in_wait_list;
    const cl_event *event_wait_list;
public:
    ReadBuffer(
            Buffer<T>& read_buffer,
            T* read_dest,
            size_t read_offset,
            size_t read_size,
            cl_uint read_num_events_in_wait_list,
            const cl_event* read_event_wait_list):

        buffer(read_buffer),
        dest(read_dest),
        offset(read_offset),
        size(read_size),
        num_events_in_wait_list(read_num_events_in_wait_list),
        event_wait_list(read_event_wait_list)
    {}

    ~ReadBuffer()
    {}

    void execute(CmdQueue& queue)
    {
        try {
            cl_event* p_evt = static_cast<cl_event*>(NULL);
            if (p_event)
                p_evt = &getEventToBind();

            ret_code ret = clEnqueueReadBuffer(
                        queue.getQueue(),
                        buffer.getMemObject(),
                        isBlocking(),
                        offset,
                        size,
                        static_cast<void*>(dest),
                        num_events_in_wait_list,
                        event_wait_list,
                        p_evt);

            if (ret != CL_SUCCESS)
                throw(SException(ret));
        } catch (SException& e) {
            throw (e);
        }
    }

    ReadBuffer& operator()(bool is_blocking)
    {
        blocking = is_blocking;
        return *this;
    }

    ReadBuffer& operator >>(Event &event)
    {
        p_event = &event;
        return *this;
    }

};

template<class T>
class WriteBuffer: public BlockingCommand, public EventCommand
{
protected:
    Buffer<T>& buffer;
    T const* src;
    size_t offset;
    size_t size;
    cl_uint num_events_in_wait_list;
    const cl_event *event_wait_list;
public:
    WriteBuffer(
            Buffer<T>& write_buffer,
            T const* write_src,
            size_t write_offset,
            size_t write_size,
            cl_uint write_num_events_in_wait_list,
            const cl_event* write_event_wait_list):

        buffer(write_buffer),
        src(write_src),
        offset(write_offset),
        size(write_size),
        num_events_in_wait_list(write_num_events_in_wait_list),
        event_wait_list(write_event_wait_list)
    {}

    ~WriteBuffer()
    {}

    void execute(CmdQueue& queue)
    {
        try {
            cl_event* p_evt = static_cast<cl_event*>(NULL);
            if (p_event)
                p_evt = &getEventToBind();

            ret_code ret = clEnqueueWriteBuffer(
                        queue.getQueue(),
                        buffer.getMemObject(),
                        isBlocking(),
                        offset,
                        size,
                        static_cast<void const*>(src),
                        num_events_in_wait_list,
                        event_wait_list,
                        p_evt);

            if (ret != CL_SUCCESS)
                throw(SException(ret));
        } catch (SException& e) {
            throw (e);
        }
    }

    WriteBuffer& operator()(bool is_blocking)
    {
        blocking = is_blocking;
        return *this;
    }

    WriteBuffer& operator >>(Event &event)
    {
        p_event = &event;
        return *this;
    }

};

template<class T>
class MapBuffer: public BlockingCommand, public EventCommand
{
protected:
    Buffer<T>& buffer;
    cl_map_flags flags;
    size_t offset;
    size_t size;
    cl_uint num_events_in_wait_list;
    const cl_event *event_wait_list;
public:
    MapBuffer(
            Buffer<T>& map_buffer,
            cl_map_flags map_flags,
            size_t map_offset,
            size_t map_size,
            cl_uint map_num_events_in_wait_list,
            const cl_event* map_event_wait_list):

        buffer(map_buffer),
        flags(map_flags),
        offset(map_offset),
        size(map_size),
        num_events_in_wait_list(map_num_events_in_wait_list),
        event_wait_list(map_event_wait_list)
    {}

    ~MapBuffer()
    {}

    void execute(CmdQueue& queue)
    {
        try {
            cl_event* p_evt = static_cast<cl_event*>(NULL);
            if (p_event)
                p_evt = &getEventToBind();

            ret_code ret = CL_SUCCESS;
            buffer.mapped_to = static_cast<T*>(clEnqueueMapBuffer(
                                                   queue.getQueue(),
                                                   buffer.getMemObject(),
                                                   isBlocking(),
                                                   flags,
                                                   offset,
                                                   size,
                                                   num_events_in_wait_list,
                                                   event_wait_list,
                                                   p_evt,
                                                   &ret));

            if (ret != CL_SUCCESS)
                throw(SException(ret));
        } catch (SException& e) {
            throw (e);
        }
    }

    MapBuffer& operator()(bool is_blocking)
    {
        blocking = is_blocking;
        return *this;
    }

    MapBuffer& operator >>(Event &event)
    {
        p_event = &event;
        return *this;
    }
};

template<class T>
class UnmapMemObject: public EventCommand
{
protected:
    MemObject<T>& mem_object;
    cl_uint num_events_in_wait_list;
    const cl_event *event_wait_list;

public:
    UnmapMemObject(
            Buffer<T>& map_mem_object,
            cl_uint map_num_events_in_wait_list,
            const cl_event* map_event_wait_list):

        mem_object(map_mem_object),
        num_events_in_wait_list(map_num_events_in_wait_list),
        event_wait_list(map_event_wait_list)
    {}

    ~UnmapMemObject()
    {}

    void execute(CmdQueue &queue)
    {
        try {
            cl_event* p_evt = static_cast<cl_event*>(NULL);
            if (p_event)
                p_evt = &getEventToBind();

            ret_code ret = clEnqueueUnmapMemObject(
                        queue.getQueue(),
                        mem_object.getMemObject(),
                        static_cast<void*>(mem_object.getMappedTo()),
                        num_events_in_wait_list,
                        event_wait_list,
                        p_evt);

            if (ret != CL_SUCCESS)
                throw(SException(ret));
        } catch (SException& e) {
            throw (e);
        }
    }

    UnmapMemObject& operator >>(Event &event)
    {
        p_event = &event;
        return *this;
    }

};

template<class T>
class CopyMemObject: public EventCommand
{
protected:
    size_t src_offset, dst_offset, num_bytes;
    MemObject<T>& src, dst;
    cl_uint num_events_in_wait_list;
    const cl_event *event_wait_list;

public:
    CopyMemObject(
            Buffer<T>& copy_src,
            Buffer<T>& copy_dst,
            size_t copy_src_offset,
            size_t copy_dst_offset,
            size_t copy_num_bytes,
            cl_uint copy_num_events_in_wait_list,
            const cl_event* copy_event_wait_list):

        src(copy_src),
        dst(copy_dst),
        src_offset(copy_src_offset),
        dst_offset(copy_dst_offset),
        num_bytes(copy_num_bytes),
        num_events_in_wait_list(copy_num_events_in_wait_list),
        event_wait_list(copy_event_wait_list)
    {}

    ~CopyMemObject()
    {}

    void execute(CmdQueue &queue)
    {
        try {
            cl_event* p_evt = static_cast<cl_event*>(NULL);
            if (p_event)
                p_evt = &getEventToBind();

            ret_code ret = clEnqueueCopyBuffer(
                        queue.getQueue(),
                        src.getMemObject(),
                        dst.getMemObject(),
                        src_offset,
                        dst_offset,
                        num_bytes,
                        num_events_in_wait_list,
                        event_wait_list,
                        p_evt);

            if (ret != CL_SUCCESS)
                throw(SException(ret));
        } catch (SException& e) {
            throw (e);
        }
    }

    CopyMemObject& operator >>(Event &event)
    {
        p_event = &event;
        return *this;
    }
};
}
