#pragma once

#include "../render/UniformBuffer.hpp"
#include "../vulkan/ZSVulkanInstance.hpp"
#include "ZSVMA.hpp"

namespace Engine{

class vkUniformBuffer : public UniformBuffer{
private:
    VmaVkBuffer mVkBuffer;
public:
    void init(unsigned int slot, unsigned int size, bool CreateCpuBuffer = false);
    void writeData(unsigned int offset, unsigned int size, void* data);
    void bind();
    void updateBufferedData();
    void Destroy();
    VkBuffer GetBuffer() { return mVkBuffer.Buffer; }

    vkUniformBuffer();
    ~vkUniformBuffer();
};
}
