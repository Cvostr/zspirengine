#ifndef ZSUNIFORMBUFFER_H
#define ZSUNIFORMBUFFER_H
namespace Engine {

class UniformBuffer{
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
#endif // ZSUNIFORMBUFFER_H
