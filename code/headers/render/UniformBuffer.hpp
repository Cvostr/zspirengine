#pragma once

#include "GpuObject.hpp"

namespace Engine {

class UniformBuffer : public GpuObject {
protected:
    unsigned int mSlot;
    unsigned int mBufferSize;
public:
    virtual void init(unsigned int slot, unsigned int size) {}
    virtual void bind() {}
    virtual void writeData(unsigned int offset, unsigned int size, void* data) {}
    virtual void Destroy(){}
    unsigned int GetSlotId() { return mSlot; }
    unsigned int GetBufferSize() { return mBufferSize; }

    UniformBuffer();
    UniformBuffer(unsigned int slot, unsigned int size);
    virtual ~UniformBuffer();
};

UniformBuffer* allocUniformBuffer();

}