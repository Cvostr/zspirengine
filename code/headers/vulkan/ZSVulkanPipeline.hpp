#pragma once

#include <vulkan/vulkan.hpp>
#include "ZSVulkan.hpp"
#include "vk_data.h"

namespace Engine {
    typedef struct ZsVkPipelineConf {
        VkPrimitiveTopology iaTopology;
        VkViewport viewport;
        VkRect2D scissor;
        VkPolygonMode polygonMode;

        unsigned int cullFace;
        bool hasDepth;

        std::vector<VkAttachmentDescription> mAttachmentDescriptions;
        std::vector<VkAttachmentReference> mAttachmentReferences;
        VkAttachmentReference DepthDescriptionRef;

        ZsVkPipelineConf() {
            iaTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
            cullFace = VK_CULL_MODE_NONE;
            //Set Viewport base params
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = 640;
            viewport.height = 480;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            //Set swap extend base params
            VkExtent2D swap_extend;
            swap_extend.width = 640;
            swap_extend.height = 480;
            scissor.offset = { 0, 0 };
            scissor.extent = swap_extend;

            hasDepth = false;
        }
        ZsVkPipelineConf(float w, float h, float maxDepth) {
            iaTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
            cullFace = VK_CULL_MODE_NONE;
            //Set Viewport base params
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = w;
            viewport.height = h;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = maxDepth;
            //Set swap extend base params
            VkExtent2D swap_extend;
            swap_extend.width = static_cast<uint32_t>(w);
            swap_extend.height = static_cast<uint32_t>(h);
            scissor.offset = { 0, 0 };
            scissor.extent = swap_extend;

            hasDepth = false;
        }
    }ZsVkPipelineConf;

	class ZSVulkanPipeline {
	private:
		VkPipeline pipeline; //Vulkan pipeline object
        VkRenderPass mRenderPass; //Render pass of this layout
        VkPipelineLayout mPipelineLayout;
	public:

        VkRenderPass GetRenderPass();
        VkPipelineLayout GetPipelineLayout();
        VkPipeline GetPipeline();

		bool Create(ZSVulkanDevice* device, _vk_Shader* Shader, ZsVkPipelineConf Conf);
	};
}