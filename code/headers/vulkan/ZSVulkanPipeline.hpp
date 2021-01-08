#pragma once

#include <vulkan/vulkan.hpp>
#include "../render/GpuObject.hpp"
#include "ZSVulkan.hpp"
#include "VKShader.hpp"
#include "ZSVulkanDescriptorSet.hpp"
#include "ZSVulkanRenderPass.hpp"
#include "ZSVulkanPipelineLayout.hpp"

namespace Engine {
    typedef struct ZsVkPipelineConf {
        VkPrimitiveTopology iaTopology;
        
        VkPolygonMode polygonMode;
        VkViewport Viewport;
        unsigned int cullFace;
        bool hasDepth;

        void SetDefaultViewport();

        ZsVkPipelineLayoutConf LayoutInfo;

        ZsVkPipelineConf() {
            SetDefaultViewport();
            iaTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
            cullFace = VK_CULL_MODE_NONE;
            hasDepth = false; 
        }
        ZsVkPipelineConf(float w, float h, float maxDepth) {
            SetDefaultViewport();
            iaTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
            cullFace = VK_CULL_MODE_NONE;
            hasDepth = false;
        }
    }ZsVkPipelineConf;

	class ZSVulkanPipeline : public GpuObject{
	private:
		VkPipeline pipeline; //Vulkan pipeline object
        ZSVulkanPipelineLayout* mLayout;
        
	public:
        VkPipelineLayout _GetPipelineLayout();
        VkPipeline GetPipeline();
        ZSVulkanPipelineLayout* GetPipelineLayout();

        bool Create(vkShader* Shader, ZSVulkanRenderPass* renderPass,  ZsVkPipelineConf Conf);
        void CmdBindPipeline(VkCommandBuffer cmdbuf);
        void CmdPushConstants(VkCommandBuffer cmdbuf, VkShaderStageFlagBits stage, unsigned int offset, unsigned int size, void* data);
        void CmdBindDescriptorSets(VkCommandBuffer cmdbuf);
        void Destroy();

        ZSVulkanPipeline()
        {
            mLayout = new ZSVulkanPipelineLayout;
        }
	};
}