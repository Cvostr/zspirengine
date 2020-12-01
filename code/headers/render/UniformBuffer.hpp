#pragma once

#include "GpuObject.hpp"

namespace Engine {

class UniformBuffer : public GpuObject {
public:
    virtual void init(unsigned int slot, unsigned int size) {}
    virtual void bind() {}
    virtual void writeData(unsigned int offset, unsigned int size, void* data) {}
    virtual void Destroy(){}

    UniformBuffer();
    UniformBuffer(unsigned int slot, unsigned int size);
    virtual ~UniformBuffer();
};

UniformBuffer* allocUniformBuffer();

}