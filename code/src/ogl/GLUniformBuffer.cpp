#include "../../headers/ogl/GLUniformBuffer.hpp"
#include <GL/glew.h>

void Engine::glUniformBuffer::init(unsigned int slot, unsigned int size, bool CreateCpuBuffer){
    if (!mCreated) {
        mSlot = slot;
        mBufferSize = size;

        if (CreateCpuBuffer) {
            mCpuBuffer = new char[mBufferSize];
            mCpuBufferCreated = true;
        }

        glGenBuffers(1, &mBufferID);
        glBindBuffer(GL_UNIFORM_BUFFER, mBufferID);
        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
        //Connect to point 0 (zero)
        glBindBufferBase(GL_UNIFORM_BUFFER, slot, mBufferID);

        mCreated = true;
    }
}
void Engine::glUniformBuffer::writeData(unsigned int offset, unsigned int size, void* data){
    if(mCreated && !mCpuBufferCreated)
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

void Engine::glUniformBuffer::updateBufferedData() {
    if (mCreated && mCpuBufferCreated) {
        glBufferSubData(GL_UNIFORM_BUFFER, 0, mBufferSize, mCpuBuffer);
    }
}

void Engine::glUniformBuffer::bind(){
    if (mCreated)
        glBindBuffer(GL_UNIFORM_BUFFER, mBufferID);
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
    if (mCpuBufferCreated)
        delete[] mCpuBuffer;
    Destroy();
}
