#ifndef ZSUNIFORMBUFFER_H
#define ZSUNIFORMBUFFER_H
namespace Engine {

class UniformBuffer{
public:
    virtual void init(int slot, int size);
    virtual void bind();
    virtual void writeData(unsigned int offset, unsigned int size, void* data);

    UniformBuffer();
    UniformBuffer(int slot, int size);
    virtual ~UniformBuffer();
};

}
#endif // ZSUNIFORMBUFFER_H
