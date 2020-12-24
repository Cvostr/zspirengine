#pragma once

#include <vulkan/vulkan.hpp>
#include "ZSVulkan.hpp"
#include "vk_data.h"
#include "ZSVulkanDescriptorSet.hpp"
#include "ZSVulkanRenderPass.hpp"

namespace Engine {
    typedef struct ZsVkPipelineConf {
        VkPrimitiveTopology iaTopology;
        
        VkPolygonMode polygonMode;

        unsigned int cullFace;
        bool hasDepth;

        ZSVulkanDescriptorSet* DescrSetLayout;
        ZSVulkanDescriptorSet* DescrSetLayoutSampler;

        ZsVkPipelineConf() {
            iaTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
            cullFace = VK_CULL_MODE_NONE;
            hasDepth = false;

            DescrSetLayout = new ZSVulkanDescriptorSet(DESCR_SET_TYPE::DESCR_SET_TYPE_UBO);
            DescrSetLayoutSampler = new ZSVulkanDescriptorSet(DESCR_SET_TYPE::DESCR_SET_TYPE_TEXTURE);
        }
        ZsVkPipelineConf(float w, float h, float maxDepth) {
            iaTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
            cullFace = VK_CULL_MODE_NONE;
            hasDepth = false;

            DescrSetLayout = new ZSVulkanDescriptorSet(DESCR_SET_TYPE::DESCR_SET_TYPE_UBO);
            DescrSetLayoutSampler = new ZSVulkanDescriptorSet(DESCR_SET_TYPE::DESCR_SET_TYPE_TEXTURE);
        }
    }ZsVkPipelineConf;

	class ZSVulkanPipeline {
	private:
		VkPipeline pipeline; //Vulkan pipeline object
        VkPipelineLayout mPipelineLayout;

        
        std::vector<VkDescriptorSetLayout> mDescrSetLayouts; //Array of all layouts
        std::vector<VkDescriptorSet> mDescrSets; //Array of all descriptors

        unsigned int mPushConstantBuffersSize;
        std::vector<VkPushConstantRange> mPushConstants;

        ZSVulkanDescriptorSet* DescrSetLayout;
        
	public:
        ZSVulkanDescriptorSet* DescrSetLayoutSampler;
        VkPipelineLayout GetPipelineLayout();
        VkPipeline GetPipeline();
        VkDescriptorSet* GetDescriptorsSets();
        unsigned int GetDescriptorSetsCount();

        void AddPushConstant(unsigned int size, VkShaderStageFlagBits flag);


        bool Create(_vk_Shader* Shader, ZSVulkanRenderPass* renderPass,  ZsVkPipelineConf Conf);
        void CmdBindPipeline(VkCommandBuffer cmdbuf);
        void CmdPushConstants(VkCommandBuffer cmdbuf, VkShaderStageFlagBits stage, unsigned int offset, unsigned int size, void* data);
        void CmdBindDescriptorSets(VkCommandBuffer cmdbuf);

        ZSVulkanPipeline():
            mPushConstantBuffersSize(0)
        {

        }
	};
}