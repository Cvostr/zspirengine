#include "../../headers/vulkan/ZSVulkanPipeline.hpp"
#include "../../headers/math/Vertex.hpp"

#define VERTEX_BUFFER_BIND 0

VkRenderPass Engine::ZSVulkanPipeline::GetRenderPass() {
    return mRenderPass;
}
VkPipelineLayout Engine::ZSVulkanPipeline::GetPipelineLayout() {
    return mPipelineLayout;
}
VkPipeline Engine::ZSVulkanPipeline::GetPipeline() {
    return pipeline;
}

VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = VERTEX_BUFFER_BIND;
    bindingDescription.stride = sizeof(ZSVERTEX);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

bool Engine::ZSVulkanPipeline::Create(ZSVulkanDevice* device, _vk_Shader* Shader, ZsVkPipelineConf Conf) {
    VkPipelineShaderStageCreateInfo vertexStageCreateInfo = {}, fragmentStageCreateInfo = {};

    vertexStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexStageCreateInfo.flags = 0;
    vertexStageCreateInfo.pNext = nullptr;
    vertexStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStageCreateInfo.module = Shader->vertexShader;
    vertexStageCreateInfo.pName = "main";

    fragmentStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentStageCreateInfo.flags = 0;
    fragmentStageCreateInfo.pNext = nullptr;
    fragmentStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStageCreateInfo.module = Shader->fragmentShader;
    fragmentStageCreateInfo.pName = "main";

    //IA state
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = Conf.iaTopology;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    //Viewport & Scissors congiration
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &Conf.viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &Conf.scissor;
    //Rasterizer conf
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = Conf.polygonMode;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = Conf.cullFace;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
    //MSAA configuration
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional


    //Create SubPass
    //This collects attachment references
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = Conf.mAttachmentReferences.size();
    subpass.pColorAttachments = Conf.mAttachmentReferences.data();
    subpass.pDepthStencilAttachment = &Conf.DepthDescriptionRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    //RENDER PASS
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = Conf.mAttachmentDescriptions.size();
    renderPassInfo.pAttachments = Conf.mAttachmentDescriptions.data();
    //Set subpass
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    //Set dependencies
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device->getVkDevice(), &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS) {
        return false;
    }



    //Get
    VkVertexInputBindingDescription bindingDescription = getBindingDescription();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = 5;

    VkVertexInputAttributeDescription attrDescr[5];
    attrDescr[0].binding = VERTEX_BUFFER_BIND;
    attrDescr[0].location = 0;
    attrDescr[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrDescr[0].offset = offsetof(ZSVERTEX, pos);
    attrDescr[1].binding = VERTEX_BUFFER_BIND;
    attrDescr[1].location = 1;
    attrDescr[1].format = VK_FORMAT_R32G32_SFLOAT;
    attrDescr[1].offset = offsetof(ZSVERTEX, uv);
    attrDescr[2].binding = VERTEX_BUFFER_BIND;
    attrDescr[2].location = 2;
    attrDescr[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrDescr[2].offset = offsetof(ZSVERTEX, normal);
    attrDescr[3].binding = VERTEX_BUFFER_BIND;
    attrDescr[3].location = 3;
    attrDescr[3].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrDescr[3].offset = offsetof(ZSVERTEX, tangent);
    attrDescr[4].binding = VERTEX_BUFFER_BIND;
    attrDescr[4].location = 4;
    attrDescr[4].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrDescr[4].offset = offsetof(ZSVERTEX, bitangent);

    vertexInputInfo.pVertexAttributeDescriptions = attrDescr; // Optional



    //Color blend
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    //depthStencil.front{}; // Optional
   //depthStencil.back{}; // Optional

    VkGraphicsPipelineCreateInfo pipeline_create_info = {};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.pNext = nullptr;
    pipeline_create_info.stageCount = 2; //Usually, we have two shader stages (vertex and fragment)
    VkPipelineShaderStageCreateInfo stages[2] = { vertexStageCreateInfo, fragmentStageCreateInfo };
    pipeline_create_info.pStages = stages; //Store shader stages
    //Now bind all fucking structures, that created above
    pipeline_create_info.pVertexInputState = &vertexInputInfo;
    pipeline_create_info.pInputAssemblyState = &inputAssembly;
    pipeline_create_info.pViewportState = &viewportState;
    pipeline_create_info.pRasterizationState = &rasterizer;
    pipeline_create_info.pMultisampleState = &multisampling;
    if (Conf.hasDepth)
        pipeline_create_info.pDepthStencilState = &depthStencil;
    else
        pipeline_create_info.pDepthStencilState = nullptr;
    pipeline_create_info.pColorBlendState = &colorBlending;
    pipeline_create_info.pDynamicState = nullptr;
    pipeline_create_info.layout = mPipelineLayout;
    //Bind render pass
    pipeline_create_info.renderPass = mRenderPass;
    pipeline_create_info.subpass = 0;
    //For future recreation
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipeline_create_info.basePipelineIndex = -1; // Optional
    //Try to create pipeline
    if (vkCreateGraphicsPipelines(device->getVkDevice(), VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &this->pipeline) != VK_SUCCESS)
        return false;

    return true;
}