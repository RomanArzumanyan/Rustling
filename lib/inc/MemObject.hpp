#pragma once

#include <vector>
#include <stddef.h>
#include <initializer_list>
#include "Common.hpp"
#include "BufferCommands.hpp"
#include "Command.hpp"
#include "Context.hpp"
#include "SException.hpp"

using namespace std;

namespace rustling
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

    MemObject<T>& operator=(const MemObject& other)
    {
        mapped_to = other.mapped_to;
        mem_object = other.mem_object;

        auto ret = clRetainMemObject(mem_object);
        if(ret!=CL_SUCCESS)
            throw(SException(ret));

        return *this;
    }

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
        if(mem_object != static_cast<cl_mem>(NULL))
            clReleaseMemObject(mem_object);
    }

    cl_mem& getMemObject()
    {
        return mem_object;
    }

    MemObject(const MemObject<T>& other):
        mapped_to(other.mapped_to),
        mem_object(other.mem_object)
    {
        auto ret = clRetainMemObject(mem_object);
        if(ret!=CL_SUCCESS)
            throw(SException(ret));
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
    bool is_child;
    size_t elems_num;
    size_t size;

    Buffer(bool is_sub_buffer):
        MemObject<T>(),
        is_child(is_sub_buffer),
        elems_num(static_cast<size_t>(0)),
        size(static_cast<size_t>(0))
    {}

public:
    Buffer(
        Context& ctx,
        std::size_t num_elems,
        cl_mem_flags flags = CL_MEM_READ_WRITE,
        T* host_ptr = static_cast<T*>(NULL)):

        MemObject<T>(),
        is_child(false),
        elems_num(num_elems),
        size(elems_num*sizeof(T))
    {
        ret_code ret;
        this->mem_object = clCreateBuffer(
                    ctx.getContext(),
                    flags,
                    size,
                    static_cast<void*>(host_ptr), &ret);

        if (ret != CL_SUCCESS)
            throw(SException(ret));
    }

    Buffer():
        MemObject<T>()
    {}

    bool isChild() const
    {
        return is_child;
    }

    Buffer(const Buffer<T>& other):
        MemObject<T>(other)
    {
        is_child = other.is_child;
        elems_num = other.elems_num;
        size = other.size;
    }

    Buffer<T>& operator=(const Buffer<T>& other)
    {
        MemObject<T>::operator=(other);
        is_child = other.is_child;
        elems_num = other.elems_num;
        size = other.size;
        return *this;
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
            auto map_size = size ? size : getSize();
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
            auto read_size = size ? size : getSize();
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
            auto write_size = size ? size : getSize();
            return WriteBuffer<T>(*this, src, offset, write_size,
                                  num_events_in_wait_list, event_wait_list);
        } catch (SException &e) {
            throw (e);
        }
    }

    CopyMemObject<T> copy(
        Buffer<T> &dst,
        size_t src_offset=0,
        size_t dst_offset=0,
        size_t num_bytes=0,
        cl_uint num_events_in_wait_list = 0,
        const cl_event* event_wait_list = NULL)
    {
        try{
            if(num_bytes==0)
                num_bytes = this->getSize();

            if(num_bytes > dst.getSize())
                throw MemObjectsSizeMismatch();

            return CopyMemObject<T>(*this, dst, src_offset, dst_offset, num_bytes,
                                    num_events_in_wait_list, event_wait_list);
        }catch(SException &e){
            throw(e);
        }
    }
};

template<class T>
class SubBuffer: public Buffer<T>
{
public:
    virtual ~SubBuffer()
    {}

    SubBuffer(
            Buffer<T>& parent,
            size_t sub_origin,
            size_t sub_size,
            cl_mem_flags flags=0):

        Buffer<T>(true)
    {
        if(parent.isChild())
            throw(SException(CL_INVALID_MEM_OBJECT,
                             "Can't make SubBuffer from SubBuffer"));

        this->elems_num = sub_size/sizeof(T);
        this->size = sub_size;

        cl_buffer_region buffer_region;
        buffer_region.origin = sub_origin;
        buffer_region.size = sub_size;

        ret_code ret;
        this->mem_object = clCreateSubBuffer(
                    parent.getMemObject(),
                    flags,
                    CL_BUFFER_CREATE_TYPE_REGION,
                    &buffer_region,
                    &ret);
        if(ret!=CL_SUCCESS)
            throw(SException(ret, "Failed to create sub-buffer"));

    }

    SubBuffer():
        Buffer<T>()
    {}

    SubBuffer(const SubBuffer& other):
        Buffer<T>(other)
    {}

    SubBuffer<T>& operator=(const SubBuffer& other)
    {
        Buffer<T>::operator =(other);
        return *this;
    }
};

template<class T>
class Image: public MemObject<T>
{};
}

