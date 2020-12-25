#pragma once

#include <vulkan/vulkan.hpp>
#include "ZSVulkan.hpp"
#include "vk_data.h"
#include "ZSVulkanDescriptorSet.hpp"
#include "ZSVulkanRenderPass.hpp"
#include "ZSVulkanPipelineLayout.hpp"

namespace Engine {
    typedef struct ZsVkPipelineConf {
        VkPrimitiveTopology iaTopology;
        
        VkPolygonMode polygonMode;

        unsigned int cullFace;
        bool hasDepth;

        ZsVkPipelineLayoutConf LayoutInfo;

        ZsVkPipelineConf() {
            iaTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
            cullFace = VK_CULL_MODE_NONE;
            hasDepth = false; 
        }
        ZsVkPipelineConf(float w, float h, float maxDepth) {
            iaTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
            cullFace = VK_CULL_MODE_NONE;
            hasDepth = false;
        }
    }ZsVkPipelineConf;

	class ZSVulkanPipeline {
	private:
		VkPipeline pipeline; //Vulkan pipeline object
        ZSVulkanPipelineLayout* mLayout;
        
	public:
        VkPipelineLayout _GetPipelineLayout();
        VkPipeline GetPipeline();
        ZSVulkanPipelineLayout* GetPipelineLayout();

        bool Create(_vk_Shader* Shader, ZSVulkanRenderPass* renderPass,  ZsVkPipelineConf Conf);
        void CmdBindPipeline(VkCommandBuffer cmdbuf);
        void CmdPushConstants(VkCommandBuffer cmdbuf, VkShaderStageFlagBits stage, unsigned int offset, unsigned int size, void* data);
        void CmdBindDescriptorSets(VkCommandBuffer cmdbuf);

        ZSVulkanPipeline()
        {
            mLayout = new ZSVulkanPipelineLayout;
        }
	};
}