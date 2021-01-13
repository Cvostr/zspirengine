#include "../../headers/ogl/GLUniformBuffer.hpp"
#include <GL/glew.h>

void Engine::glUniformBuffer::init(unsigned int slot, unsigned int size, bool StorageBuffer){
    if (!mCreated) {
        mSlot = slot;
        mBufferSize = size;

        mCpuBuffer = new char[mBufferSize];

        target = GL_UNIFORM_BUFFER;
        if (StorageBuffer)
            target = GL_SHADER_STORAGE_BUFFER;

        glGenBuffers(1, &mBufferID);
        glBindBuffer(target, mBufferID);
        glBufferData(target, size, nullptr, GL_STATIC_DRAW);
        //Connect to point 0 (zero)
        glBindBufferBase(target, slot, mBufferID);

        mCreated = true;
    }
}
void Engine::glUniformBuffer::writeData(unsigned int offset, unsigned int size, void* data){
    if (mCreated) {
        glBufferSubData(target, offset, size, data);
        writeDataBuffered(offset, size, data);
    }
}

void Engine::glUniformBuffer::updateBufferedData() {
    if (mCreated) {
        glBufferSubData(target, 0, mBufferSize, mCpuBuffer);
    }
}

void Engine::glUniformBuffer::bind(){
    if (mCreated)
        glBindBuffer(target, mBufferID);
}

void Engine::glUniformBuffer::Destroy(){
    if (mCreated) {
        glDeleteBuffers(1, &mBufferID);
        mCreated = false;
    }
}

Engine::glUniformBuffer::glUniformBuffer() :
    mBufferID(0)
{

}
Engine::glUniformBuffer::~glUniformBuffer(){
    delete[] mCpuBuffer;
    Destroy();
}
