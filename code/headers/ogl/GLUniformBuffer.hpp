#pragma once

#include "../render/UniformBuffer.hpp"

namespace Engine{

class glUniformBuffer : public UniformBuffer{
private:
    //openGL integer ID
    unsigned int mBufferID;
public:
    void init(unsigned int slot, unsigned int size, bool CreateCpuBuffer = false);
    void writeData(unsigned int offset, unsigned int size, void* data);
    void updateBufferedData();
    void bind();
    void Destroy();

    glUniformBuffer();
    ~glUniformBuffer();
};

}
