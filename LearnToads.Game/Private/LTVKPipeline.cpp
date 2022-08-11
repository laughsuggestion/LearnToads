#include "PrecompiledHeader.h"
#include "LTVKPipeline.h"
#include "LTAsset.h"
#include "LTVKDevice.h"

LTVKPipeline::LTVKPipeline(
    LTVKDevice* device,
    LTShader* vertexShader,
    LTShader* fragmentShader) :
    m_Device(device),
    m_VertexShader(vertexShader),
    m_FragmentShader(fragmentShader),
    m_VkPipeline(VK_NULL_HANDLE)
{
}

bool LTVKPipeline::Initialize(const LTVKPipelineConfig& config)
{
    VkShaderModule& vertexShader = m_VertexShader->GetShaderModule();
    VkShaderModule& fragmentShader = m_FragmentShader->GetShaderModule();

    VkPipelineShaderStageCreateInfo stages[2];
    VkPipelineShaderStageCreateInfo& vertexShaderCreateInfo = (stages[0] = {});
    VkPipelineShaderStageCreateInfo& fragmentShaderCreateInfo = (stages[1] = {});

    vertexShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderCreateInfo.module = vertexShader;
    vertexShaderCreateInfo.pName = "main";

    fragmentShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderCreateInfo.module = fragmentShader;
    fragmentShaderCreateInfo.pName = "main";

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pVertexAttributeDescriptions = 0;
    vertexInputInfo.pVertexBindingDescriptions = 0;

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = stages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &config.inputAssemblyInfo;
    pipelineInfo.pViewportState = &config.viewportInfo;
    pipelineInfo.pRasterizationState = &config.rasterizationInfo;
    pipelineInfo.pMultisampleState = &config.multisampleInfo;
    pipelineInfo.pColorBlendState = &config.colorBlendInfo;
    pipelineInfo.pDepthStencilState = &config.depthStencilInfo;
    pipelineInfo.layout = config.pipelineLayout;
    pipelineInfo.renderPass = config.renderPass;
    pipelineInfo.subpass = config.subpass;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(
        m_Device->GetDevice(),
        VK_NULL_HANDLE,
        1,
        &pipelineInfo,
        nullptr,
        &m_VkPipeline) != VK_SUCCESS)
    {
        return false;
    }

    return true;
}

void LTVKPipeline::Destroy()
{
    if (m_VkPipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(
            m_Device->GetDevice(),
            m_VkPipeline,
            nullptr);
    }
}

void LTVKPipeline::GetDefaultPipelineConfig(LTVKPipelineConfig& outConfig, uint32_t width, uint32_t height)
{
    // configure the input assembly information
    outConfig.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    outConfig.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    outConfig.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    // configure the viewport
    outConfig.viewport.x = 0.0f;
    outConfig.viewport.y = 0.0f;
    outConfig.viewport.width = (float)width;
    outConfig.viewport.height = (float)height;
    outConfig.viewport.minDepth = 0.0f;
    outConfig.viewport.maxDepth = 1.0f;

    // configure the scissor rect
    outConfig.scissor.offset.x = 0;
    outConfig.scissor.offset.y = 0;
    outConfig.scissor.extent.width = width;
    outConfig.scissor.extent.height = height;

    // configure viewport creation info
    outConfig.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    outConfig.viewportInfo.viewportCount = 1;
    outConfig.viewportInfo.pViewports = &outConfig.viewport;
    outConfig.viewportInfo.scissorCount = 1;
    outConfig.viewportInfo.pScissors = &outConfig.scissor;

    // configure rasterization state creation info
    outConfig.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    outConfig.rasterizationInfo.depthClampEnable = VK_FALSE;
    outConfig.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    outConfig.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    outConfig.rasterizationInfo.lineWidth = 1.0f;
    outConfig.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    outConfig.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    outConfig.rasterizationInfo.depthBiasEnable = VK_FALSE;
    outConfig.rasterizationInfo.depthBiasConstantFactor = 0.0f; // optional
    outConfig.rasterizationInfo.depthBiasClamp = 0.0f;          // optional
    outConfig.rasterizationInfo.depthBiasSlopeFactor = 0.0f;    // optional

    // configure multi-sampling
    outConfig.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    outConfig.multisampleInfo.sampleShadingEnable = VK_FALSE;
    outConfig.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    outConfig.multisampleInfo.minSampleShading = 1.0f;            // optional
    outConfig.multisampleInfo.pSampleMask = nullptr;              // optional
    outConfig.multisampleInfo.alphaToCoverageEnable = VK_FALSE;   // optional
    outConfig.multisampleInfo.alphaToOneEnable = VK_FALSE;        // optional

    // configure color blending
    // mixing colors works something like:
    // color.rgb = (srcColorBlendFactor * newColor.rgb) <color_op> (dstColorBlendFactor * oldColor.rgb)
    // color.a   = (srcAlphaBlendFactor * newColor.a) <alpha_op> (dstAlphaBlendFactor * oldColor.a)
    // color = color & colorWriteMask

    // configure color blend attachment
    outConfig.colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT
        | VK_COLOR_COMPONENT_G_BIT
        | VK_COLOR_COMPONENT_B_BIT
        | VK_COLOR_COMPONENT_A_BIT;

    outConfig.colorBlendAttachment.blendEnable = VK_FALSE;
    outConfig.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    outConfig.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    outConfig.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    outConfig.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    outConfig.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    outConfig.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    // configure color blend state info
    outConfig.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    outConfig.colorBlendInfo.logicOpEnable = VK_FALSE;
    outConfig.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    outConfig.colorBlendInfo.attachmentCount = 1;
    outConfig.colorBlendInfo.pAttachments = &outConfig.colorBlendAttachment;
    outConfig.colorBlendInfo.blendConstants[0] = 0.0f; // optional
    outConfig.colorBlendInfo.blendConstants[1] = 0.0f; // optional
    outConfig.colorBlendInfo.blendConstants[2] = 0.0f; // optional
    outConfig.colorBlendInfo.blendConstants[3] = 0.0f; // optional

    // configure depth testing
    outConfig.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    outConfig.depthStencilInfo.depthTestEnable = VK_TRUE;
    outConfig.depthStencilInfo.depthWriteEnable = VK_TRUE;
    outConfig.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    outConfig.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    outConfig.depthStencilInfo.minDepthBounds = 0.0f; // optional
    outConfig.depthStencilInfo.maxDepthBounds = 1.0f; // optional
    outConfig.depthStencilInfo.stencilTestEnable = VK_FALSE;
    outConfig.depthStencilInfo.front = {};            // optional
    outConfig.depthStencilInfo.back = {};             // optional
}