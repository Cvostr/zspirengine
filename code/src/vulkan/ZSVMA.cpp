#include <vulkan/ZSVMA.hpp>
#include <vulkan/ZSVulkanCmdBuf.hpp>
#include <game.h>

extern ZSGAME_DATA* game_data;

#define VMA_IMPLEMENTATION
#include <VMA/vk_mem_alloc.h>

Engine::ZSVMA::ZSVMA(Engine::ZSVulkanInstance* inst, Engine::ZSVulkanDevice* device) {
	this->allocator = new VmaAllocator;

	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
	allocatorInfo.physicalDevice = device->getPhysicalDevice();
	allocatorInfo.device = device->getVkDevice();
	allocatorInfo.instance = inst->GetInstance();
	
	vmaCreateAllocator(&allocatorInfo, (VmaAllocator*)allocator);

    this->MemoryCommandPool = beginCommandPool();
    this->MemoryCommandBuffer = CreateSingleTimeComdbuf(MemoryCommandPool);
}

void Engine::ZSVMA::allocate(const VkBufferCreateInfo createInfo, VkBuffer* buffer) {

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	
	VmaAllocation allocation;
	vmaCreateBuffer(*((VmaAllocator*)allocator), &createInfo, &allocInfo, buffer, &allocation, nullptr);
}

void Engine::ZSVMA::allocate(VkBufferUsageFlags flags, VmaVkBuffer* buffer, unsigned int size) {
    VkBufferCreateInfo vbInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    vbInfo.size = size;
    vbInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    vbInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo vbAllocCreateInfo = {};
    vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    vbAllocCreateInfo.flags = 0;

    VmaAllocationInfo stagingVertexBufferAllocInfo = {};
    vmaCreateBuffer(*((VmaAllocator*)allocator), &vbInfo, &vbAllocCreateInfo, &buffer->Buffer, (VmaAllocation*)&buffer->_allocation, &stagingVertexBufferAllocInfo);
}

void Engine::ZSVMA::allocateCpu(VkBufferUsageFlags flags, VmaVkBuffer* buffer, unsigned int size, void** mapped){
    VkBufferCreateInfo vbInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    vbInfo.size = size;
    vbInfo.usage = flags;
    vbInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo vbAllocCreateInfo = {};
    vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    //vbAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VmaAllocation tempBufferAlloc = VK_NULL_HANDLE;
    //VmaAllocationInfo tempBufferAllocInfo = {};
    vmaCreateBuffer(*((VmaAllocator*)allocator), &vbInfo, &vbAllocCreateInfo, &buffer->Buffer, &tempBufferAlloc, nullptr);
    //*mapped = tempBufferAllocInfo.pMappedData;
    buffer->_allocation = tempBufferAlloc;

    map(buffer, mapped);
}

void Engine::ZSVMA::copy(VkBuffer buffer, unsigned int offset, void* data, unsigned int size) {
    VkBufferCreateInfo vbInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    vbInfo.size = size;
    vbInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    vbInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo vbAllocCreateInfo = {};
    vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    vbAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkBuffer tempBuffer = VK_NULL_HANDLE;
    VmaAllocation tempBufferAlloc = VK_NULL_HANDLE;
    VmaAllocationInfo tempBufferAllocInfo = {};
    vmaCreateBuffer(*((VmaAllocator*)allocator), &vbInfo, &vbAllocCreateInfo, &tempBuffer, &tempBufferAlloc, &tempBufferAllocInfo);
    //Copy data to temporary buffer
    memcpy((char*)tempBufferAllocInfo.pMappedData + offset, data, size);

    
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(this->MemoryCommandBuffer, &beginInfo);
    //Copy buffer command
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(MemoryCommandBuffer, tempBuffer, buffer, 1, &copyRegion);
    //Run command
    vkEndCommandBuffer(MemoryCommandBuffer);
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &MemoryCommandBuffer;

    vkQueueSubmit(game_data->vk_main->mDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(game_data->vk_main->mDevice->GetGraphicsQueue());
    
    vkDestroyBuffer(game_data->vk_main->mDevice->getVkDevice(), tempBuffer, nullptr);
}

void Engine::ZSVMA::allocate(VkBufferUsageFlags flags, VmaVkBuffer* buffer, void* data, unsigned int size) {
    VkBufferCreateInfo vbInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    vbInfo.size = size;
    vbInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo vbAllocCreateInfo = {};

    vbInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | flags;
    vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    vbAllocCreateInfo.flags = 0;
    //Create GPU buffer
    vmaCreateBuffer(*((VmaAllocator*)allocator), &vbInfo, &vbAllocCreateInfo, &buffer->Buffer, (VmaAllocation*)&buffer->_allocation, nullptr);
    //Copy data to GPU buffer
    copy(buffer->Buffer, 0, data, size);
}

void Engine::ZSVMA::map(VmaVkBuffer* buf, void** mem) {
    vmaMapMemory(*(VmaAllocator*)this->allocator, (VmaAllocation)buf->_allocation, mem);
}
void Engine::ZSVMA::unmap(VmaVkBuffer* buf) {
    vmaUnmapMemory(*(VmaAllocator*)this->allocator, (VmaAllocation)buf->_allocation);
}
void Engine::ZSVMA::destroyBuffer(VmaVkBuffer* buf) {
    vmaDestroyBuffer(*((VmaAllocator*)allocator), buf->Buffer, (VmaAllocation)buf->_allocation);
}