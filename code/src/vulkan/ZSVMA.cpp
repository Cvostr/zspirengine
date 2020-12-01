#include "../../headers/vulkan/ZSVMA.hpp"

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
