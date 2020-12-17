#pragma once

#include "ZSVulkanInstance.hpp"
#include "ZSVulkanDevice.hpp"

namespace Engine {
	class ZSVMA {
	private:
		void* allocator;
	public:
		
		ZSVMA(ZSVulkanInstance* inst, ZSVulkanDevice* device);
		void allocate(const VkBufferCreateInfo createInfo, VkBuffer* buffer);
		void allocate(VkBufferUsageFlags flags, VkBuffer* buffer, void* data, unsigned int size);
	};
}