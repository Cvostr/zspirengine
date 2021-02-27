#include <vulkan/ZSVulkanPipeline.hpp>
#include <math/Vertex.hpp>
#include <game.h>

#define VERTEX_BUFFER_BIND 0
extern ZSGAME_DATA* game_data;

void Engine::ZsVkPipelineConf::SetDefaultViewport() {
    Viewport.x = 0.0f;
    Viewport.y = 0.0f;
    Viewport.width = static_cast<float>(game_data->window->GetWindowWidth());
    Viewport.height = static_cast<float>(game_data->window->GetWindowHeight());
    Viewport.minDepth = 0.0f;
    Viewport.maxDepth = 1.0f;
}

VkPipelineLayout Engine::ZSVulkanPipeline::_GetPipelineLayout() {
    return mLayout->GetPipelineLayout();
}
VkPipeline Engine::ZSVulkanPipeline::GetPipeline() {
    return pipeline;
}

Engine::ZSVulkanPipelineLayout* Engine::ZSVulkanPipeline::GetPipelineLayout() {
    return this->mLayout;
}

void Engine::ZSVulkanPipeline::CmdPushConstants(VkCommandBuffer cmdbuf, VkShaderStageFlagBits stage, unsigned int offset, unsigned int size, void* data) {
    vkCmdPushConstants(cmdbuf, _GetPipelineLayout(), stage, offset, size, data);
}

void Engine::ZSVulkanPipeline::CmdBindDescriptorSets(VkCommandBuffer cmdbuf) {
    mLayout->CmdBindDescriptorSets(cmdbuf);
}

void Engine::ZSVulkanPipeline::CmdBindPipeline(VkCommandBuffer cmdbuf) {
    vkCmdBindPipeline(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipeline());
}

void Engine::ZSVulkanPipeline::Destroy() {
    if (mCreated) {
        VkDevice device = game_data->vk_main->mDevice->getVkDevice();
        vkDestroyPipeline(device, pipeline, nullptr);
        mCreated = false;
    }
}

VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = VERTEX_BUFFER_BIND;
    bindingDescription.stride = sizeof(ZSVERTEX);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

bool Engine::ZSVulkanPipeline::Create(vkShader* Shader, ZSVulkanRenderPass* renderPass, ZsVkPipelineConf Conf) {
    VkPipelineShaderStageCreateInfo vertexStageCreateInfo = {},
        fragmentStageCreateInfo = {},
        geometryStageCreateInfo = {},
        tessControlCreateInfo = {},
        tessEvalCreateInfo = {};

    std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_info;

    if (Shader->mStages & HAS_VERT_SHADER) {
        vertexStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexStageCreateInfo.flags = 0;
        vertexStageCreateInfo.pNext = nullptr;
        vertexStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexStageCreateInfo.module = Shader->vertexShader;
        vertexStageCreateInfo.pName = "main";

        shader_stage_create_info.push_back(vertexStageCreateInfo);
    }

    if (Shader->mStages & HAS_FRAG_SHADER) {
        fragmentStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentStageCreateInfo.flags = 0;
        fragmentStageCreateInfo.pNext = nullptr;
        fragmentStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentStageCreateInfo.module = Shader->fragmentShader;
        fragmentStageCreateInfo.pName = "main";

        shader_stage_create_info.push_back(fragmentStageCreateInfo);
    }

    if (Shader->mStages & HAS_GEOM_SHADER) {
        geometryStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        geometryStageCreateInfo.flags = 0;
        geometryStageCreateInfo.pNext = nullptr;
        geometryStageCreateInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        geometryStageCreateInfo.module = Shader->geometryShader;
        geometryStageCreateInfo.pName = "main";
        //push geometry stage
        shader_stage_create_info.push_back(geometryStageCreateInfo);
    }
    if (Shader->mStages & HAS_TESSCTRL_SHADER) {
        tessControlCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        tessControlCreateInfo.flags = 0;
        tessControlCreateInfo.pNext = nullptr;
        tessControlCreateInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        tessControlCreateInfo.module = Shader->tessControlShader;
        tessControlCreateInfo.pName = "main";
        //push geometry stage
        shader_stage_create_info.push_back(tessControlCreateInfo);
    }
    if (Shader->mStages & HAS_TESS_SHADER) {
        tessEvalCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        tessEvalCreateInfo.flags = 0;
        tessEvalCreateInfo.pNext = nullptr;
        tessEvalCreateInfo.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        tessEvalCreateInfo.module = Shader->tessEvalShader;
        tessEvalCreateInfo.pName = "main";
        //push geometry stage
        shader_stage_create_info.push_back(tessEvalCreateInfo);
    }

    VkRect2D scissor;
    //Set swap extend base params
    VkExtent2D swap_extend;
    swap_extend.width = static_cast<uint32_t>(Conf.Viewport.width);
    swap_extend.height = static_cast<uint32_t>(Conf.Viewport.height);
    scissor.offset = { 0, 0 };
    scissor.extent = swap_extend;

    //IA state
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = Conf.iaTopology;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    //Viewport & Scissors congiration
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &Conf.Viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
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

    //Create Layout
    mLayout->Create(Conf.LayoutInfo);
    //Get
    VkVertexInputBindingDescription bindingDescription = getBindingDescription();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = 12;

    VkVertexInputAttributeDescription attrDescr[12];
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
    //Skinning
    for (unsigned int i = 0; i < MAX_BONE_PER_VERTEX / 4; i++) {
        attrDescr[5 + i].binding = VERTEX_BUFFER_BIND;
        attrDescr[5 + i].location = 5 + i;
        attrDescr[5 + i].format = VK_FORMAT_R32G32B32A32_SINT;
        attrDescr[5 + i].offset = sizeof(float) * (14 + i * 4);

        attrDescr[8 + i].binding = VERTEX_BUFFER_BIND;
        attrDescr[8 + i].location = 8 + i;
        attrDescr[8 + i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attrDescr[8 + i].offset = sizeof(float) * (26 + i * 4);
    }

    attrDescr[11].binding = VERTEX_BUFFER_BIND;
    attrDescr[11].location = 11;
    attrDescr[11].format = VK_FORMAT_R32_UINT;
    attrDescr[11].offset = sizeof(float) * (38);

    vertexInputInfo.pVertexAttributeDescriptions = attrDescr; // Optional

    //Color blend
    VkPipelineColorBlendAttachmentState colorBlendAttachments[6];
    colorBlendAttachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachments[0].blendEnable = VK_FALSE;
    for (int i = 1; i < 6; i++) {
        colorBlendAttachments[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachments[i].blendEnable = VK_FALSE;
    }

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 6;
    colorBlending.pAttachments = colorBlendAttachments;

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_LINE_WIDTH,
        VK_DYNAMIC_STATE_FRONT_FACE_EXT,
        VK_DYNAMIC_STATE_CULL_MODE_EXT
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = Conf.hasDepth;
    depthStencil.depthWriteEnable = Conf.hasDepth;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional

    VkGraphicsPipelineCreateInfo pipeline_create_info = {};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.pNext = nullptr;
    pipeline_create_info.stageCount = static_cast<uint32_t>(shader_stage_create_info.size()); //Usually, we have two shader stages (vertex and fragment)
    pipeline_create_info.pStages = shader_stage_create_info.data(); //Store shader stages
    //Now bind all fucking structures, that created above
    pipeline_create_info.pVertexInputState = &vertexInputInfo;
    pipeline_create_info.pInputAssemblyState = &inputAssembly;
    pipeline_create_info.pViewportState = &viewportState;
    pipeline_create_info.pRasterizationState = &rasterizer;
    pipeline_create_info.pMultisampleState = &multisampling;
    pipeline_create_info.pDepthStencilState = &depthStencil;

    pipeline_create_info.pColorBlendState = &colorBlending;
    pipeline_create_info.pDynamicState = nullptr;
    pipeline_create_info.layout = _GetPipelineLayout();
    //Bind render pass
    pipeline_create_info.renderPass = renderPass->GetRenderPass();
    pipeline_create_info.subpass = 0;
    //For future recreation
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipeline_create_info.basePipelineIndex = -1; // Optional
    //Try to create pipeline
    if (vkCreateGraphicsPipelines(game_data->vk_main->mDevice->getVkDevice(), VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &this->pipeline) != VK_SUCCESS)
        return false;

    return true;
}