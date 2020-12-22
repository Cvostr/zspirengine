#pragma once

#include "ZSVulkanInstance.hpp"
#include "ZSVulkanDevice.hpp"

namespace Engine {

	struct VmaVkBuffer {
		VkBuffer Buffer;
		void* _allocation;	
	};

	class ZSVMA {
	private:
		void* allocator;

		VkCommandPool MemoryCommandPool;
		VkCommandBuffer MemoryCommandBuffer;

	public:
		
		ZSVMA(ZSVulkanInstance* inst, ZSVulkanDevice* device);
		void allocate(const VkBufferCreateInfo createInfo, VkBuffer* buffer);
		void allocate(VkBufferUsageFlags flags, VkBuffer* buffer, unsigned int size);
		void allocateCpu(VkBufferUsageFlags flags, VmaVkBuffer* buffer, unsigned int size, void** mapped);
		void allocate(VkBufferUsageFlags flags, VkBuffer* buffer, void* data, unsigned int size);
		void copy(VkBuffer buffer, unsigned int offset, void* data, unsigned int size);
	
		void map(VmaVkBuffer* buf, void** mem);
		void unmap(VmaVkBuffer* buf);
		void destroyBuffer(VmaVkBuffer* buf);
	};
}