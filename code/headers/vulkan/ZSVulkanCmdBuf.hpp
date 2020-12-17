#pragma once

#include <vulkan/vulkan.hpp>

namespace Engine {
	VkCommandPool beginCommandPool();
	VkCommandBuffer beginSingleTimeComdbuf(VkCommandPool commandPool);
	void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool commandPool);
}