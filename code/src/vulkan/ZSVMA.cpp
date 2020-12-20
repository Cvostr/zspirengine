#include "../../headers/vulkan/ZSVMA.hpp"
#include "../../headers/vulkan/ZSVulkanCmdBuf.hpp"

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
}

void Engine::ZSVMA::allocate(const VkBufferCreateInfo createInfo, VkBuffer* buffer) {

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	
	VmaAllocation allocation;
	vmaCreateBuffer(*((VmaAllocator*)allocator), &createInfo, &allocInfo, buffer, &allocation, nullptr);
}

void Engine::ZSVMA::allocate(VkBufferUsageFlags flags, VkBuffer* buffer, unsigned int size) {
    VkBufferCreateInfo vbInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    vbInfo.size = size;
    vbInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    vbInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo vbAllocCreateInfo = {};
    vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    vbAllocCreateInfo.flags = 0;

    VkBuffer stagingVertexBuffer = VK_NULL_HANDLE;
    VmaAllocation stagingVertexBufferAlloc = VK_NULL_HANDLE;
    VmaAllocationInfo stagingVertexBufferAllocInfo = {};
    vmaCreateBuffer(*((VmaAllocator*)allocator), &vbInfo, &vbAllocCreateInfo, buffer, &stagingVertexBufferAlloc, &stagingVertexBufferAllocInfo);
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


    VkCommandPool commandPool = beginCommandPool();
    VkCommandBuffer copyCmdBuf = beginSingleTimeComdbuf(commandPool);
    //Copy buffer command
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(copyCmdBuf, tempBuffer, buffer, 1, &copyRegion);
    //Run command
    endSingleTimeCommands(copyCmdBuf, commandPool);
}


void Engine::ZSVMA::allocate(VkBufferUsageFlags flags, VkBuffer* buffer, void* data, unsigned int size) {
    VkBufferCreateInfo vbInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    vbInfo.size = size;
    vbInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo vbAllocCreateInfo = {};

    VmaAllocation g_hVertexBufferAlloc;
    vbInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | flags;
    vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    vbAllocCreateInfo.flags = 0;
    //Create GPU buffer
    vmaCreateBuffer(*((VmaAllocator*)allocator), &vbInfo, &vbAllocCreateInfo, buffer, &g_hVertexBufferAlloc, nullptr);
    //Copy data to GPU buffer
    copy(*buffer, 0, data, size);
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   