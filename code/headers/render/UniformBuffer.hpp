#pragma once

#include "GpuObject.hpp"

namespace Engine {


class UniformBuffer : public GpuObject {
protected:
    bool mStorageBuffer;
    unsigned int mSlot;
    unsigned int mBufferSize;
    char* mCpuBuffer;
public:
    virtual void init(unsigned int slot, unsigned int size, bool StorageBuffer = false) {}
    virtual void bind() {}
    virtual void writeData(unsigned int offset, unsigned int size, void* data) {}
    void writeDataBuffered(unsigned int offset, unsigned int size, void* data);
    char* GetCpuBuffer() { return mCpuBuffer; }
    virtual void updateBufferedData(){}
    virtual void Destroy(){}
    unsigned int GetSlotId() { return mSlot; }
    unsigned int GetBufferSize() { return mBufferSize; }
    bool isStorageBuffer() { return mStorageBuffer; }

    UniformBuffer();
    UniformBuffer(unsigned int slot, unsigned int size, bool StorageBuffer = false);
    virtual ~UniformBuffer();
};

UniformBuffer* allocUniformBuffer();

}