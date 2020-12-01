#include "../../headers/ogl/ogl.h"

void Engine::_ogl_UniformBuffer::init(unsigned int slot, unsigned int size){
    if (!mCreated) {
        glGenBuffers(1, &buffer_id);
        glBindBuffer(GL_UNIFORM_BUFFER, buffer_id);
        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
        //Connect to point 0 (zero)
        glBindBufferBase(GL_UNIFORM_BUFFER, slot, buffer_id);

        mCreated = true;
    }
}
void Engine::_ogl_UniformBuffer::writeData(unsigned int offset, unsigned int size, void* data){
    if(mCreated)
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

void Engine::_ogl_UniformBuffer::bind(){
    if (mCreated)
        glBindBuffer(GL_UNIFORM_BUFFER, buffer_id);
}

void Engine::_ogl_UniformBuffer::Destroy(){
    if (mCreated) {
        glDeleteBuffers(1, &buffer_id);
        mCreated = false;
    }
}

Engine::_ogl_UniformBuffer::_ogl_UniformBuffer(){

}
Engine::_ogl_UniformBuffer::~_ogl_UniformBuffer(){

}
