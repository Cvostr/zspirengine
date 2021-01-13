#pragma once

#include "../render/UniformBuffer.hpp"

namespace Engine{

class glUniformBuffer : public UniformBuffer{
private:
    unsigned int target;
    //openGL integer ID
    unsigned int mBufferID;
public:
    void init(unsigned int slot, unsigned int size, bool StorageBuffer = false);
    void writeData(unsigned int offset, unsigned int size, void* data);
    void updateBufferedData();
    void bind();
    void Destroy();

    glUniformBuffer();
    ~glUniformBuffer();
};

}
