#pragma once
#include <stddef.h>
#include <initializer_list>
#include "Common.hpp"
#include "BufferCommands.hpp"
#include "Command.hpp"
#include "Context.hpp"
#include "SException.hpp"

using namespace std;

namespace scow
{
template<class T>
class MemObject
{
    friend class UnmapMemObject<T>;
protected:
    T* mapped_to;
    cl_mem mem_object;
    MemObject():
        mapped_to(static_cast<T*>(NULL)),
        mem_object(static_cast<cl_mem>(NULL))
    {}

    T* getMappedTo()
    {
        return mapped_to;
    }

    T* const getMappedTo() const
    {
        return mapped_to;
    }

public:
    virtual ~MemObject()
    {
        clReleaseMemObject(mem_object);
    }

    cl_mem& getMemObject()
    {
        return mem_object;
    }
};

template<size_t m_dim, class T>
class MemObjectView
{
protected:
    size_t const dim;
    size_t const max_dim;
    vector<size_t> sizes;

    MemObjectView(initializer_list<size_t> view_sizes):
        dim(view_sizes.size()),
        max_dim(m_dim),
        sizes(dim, 0)
    {
        if (view_sizes.size() > max_dim)
            throw(InvalidViewSize());

        for (size_t i = 0; i < view_sizes.size(); i++)
            sizes[i] = *(view_sizes.begin() + i);
    }
public:
    virtual ~MemObjectView();
};

template<class T>
class Buffer: public MemObject<T>
{
    friend class ReadBuffer<T>;
    friend class WriteBuffer<T>;
    friend class MapBuffer<T>;
    friend class UnmapMemObject<T>;
protected:
    size_t elems_num;
    size_t size;
public:
    Buffer(
        Context& ctx,
        std::size_t num_elems,
        cl_mem_flags flags = CL_MEM_READ_WRITE,
        T* host_ptr = static_cast<T*>(NULL)):

        MemObject<T>(),
        elems_num(num_elems),
        size(elems_num*sizeof(T))
    {
        ret_code ret;
        this->mem_object = clCreateBuffer(ctx.getContext(), flags, size,
                                          static_cast<void*>(host_ptr), &ret);

        if (ret != CL_SUCCESS)
            throw(SException(ret));
    }

    ~Buffer()
    {}



    T& operator[](size_t idx)
    {
        if (!this->mapped_to)
            throw(MemObjectNotMapped());
        else
            return this->mapped_to[idx];
    }

    T const& operator[](size_t idx) const
    {
        if (!this->mapped_to)
            throw(MemObjectNotMapped());
        else
            return this->mapped_to[idx];
    }

    size_t getNumElems() const
    {
        return elems_num;
    }

    size_t getSize() const
    {
        return getNumElems() * sizeof(T);
    }

    MapBuffer<T> map(
        cl_map_flags flags = CL_MAP_READ | CL_MAP_WRITE,
        size_t offset = 0,
        size_t size = 0,
        cl_uint num_events_in_wait_list = 0,
        const cl_event* event_wait_list = NULL)
    {
        try {
            size_t map_size = size ? size : getSize();
            return MapBuffer<T>(*this, flags, offset, map_size,
                                num_events_in_wait_list, event_wait_list);
        } catch (SException &e) {
            throw (e);
        }
    }

    UnmapMemObject<T> unmap(
        cl_uint num_events_in_wait_list = 0,
        const cl_event* event_wait_list = NULL)
    {
        try {
            return UnmapMemObject<T>(*this, num_events_in_wait_list,
                                     event_wait_list);
        } catch (SException &e) {
            throw (e);
        }
    }

    ReadBuffer<T> read(
        T* dest,
        size_t offset = 0,
        size_t size = 0,
        cl_uint num_events_in_wait_list = 0,
        const cl_event* event_wait_list = NULL)
    {
        try {
            size_t read_size = size ? size : getSize();
            return ReadBuffer<T>(*this, dest, offset, read_size,
                                 num_events_in_wait_list, event_wait_list);
        } catch (SException &e) {
            throw (e);
        }
    }

    WriteBuffer<T> write(
        T const* src,
        size_t offset = 0,
        size_t size = 0,
        cl_uint num_events_in_wait_list = 0,
        const cl_event* event_wait_list = NULL)
    {
        try {
            size_t write_size = size ? size : getSize();
            return WriteBuffer<T>(*this, src, offset, write_size,
                                  num_events_in_wait_list, event_wait_list);
        } catch (SException &e) {
            throw (e);
        }
    }
};

template<class T>
class Image: public MemObject<T>
{};
}

