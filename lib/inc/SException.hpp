#pragma once

#define __CL_ENABLE_EXCEPTIONS

#include <string>
#include <exception>
#include <CL/cl.h>

namespace rustling
{

class SException : public std::exception
{
    cl_int err_code;
    std::string message;
public:
    SException(cl_int err, const char * errStr = NULL);
    virtual const char* what();
};

class NoPlatformFound : public SException
{
public:
    NoPlatformFound();
};

class NoDeviceFound : public SException
{
public:
    NoDeviceFound();
};

class InvalidDeviceType : public SException
{
public:
    InvalidDeviceType();
};

class InvalidPlatform: public SException
{
public:
    InvalidPlatform();
};

class InvalidDeviceParamInfo: public SException
{
public:
    InvalidDeviceParamInfo();
};

class InvalidBlockingOpStatus: public SException
{
public:
    InvalidBlockingOpStatus();
};

class CantBindEvent: public SException
{
public:
    CantBindEvent();
};

class NoGeneratedEvent: public SException
{
public:
    NoGeneratedEvent();
};

class InvalidEvent: public SException
{
public:
    InvalidEvent();
};

class MemObjectNotMapped: public SException
{
public:
    MemObjectNotMapped();
};

class InvalidViewSize: public SException
{
public:
    InvalidViewSize();
};

class KernelFileNotFound: public SException
{
public:
    KernelFileNotFound();
};

class InvalidLocalGroupSize: public SException
{
public:
    InvalidLocalGroupSize();
};

class InvalidGlobalGroupSize: public SException
{
public:
    InvalidGlobalGroupSize();
};

class MemObjectsSizeMismatch: public SException
{
public:
    MemObjectsSizeMismatch();
};

class InvalidKernelArgs: public SException
{
public:
    InvalidKernelArgs();
};

}
