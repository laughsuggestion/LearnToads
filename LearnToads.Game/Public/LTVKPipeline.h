#pragma once

#include <vulkan/vulkan.h>

class LTShader;
class LTVKDevice;

/**
 * The configuration for describing a graphics pipeline in Vulkan.
 */
struct LTVKPipelineConfig
{
    /**
     * The input assembler, for purposes of generating geometry, describes how to interpret the list of vertices
     * that are given as input to the assembler. The assembler sees the input as a list of numbers
     * broken up into chunks by vertex. The vertex can be anything [position, color, etc.] but the assembler describes
     * how to pull out the position information to create topology.
     */
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;

    /**
     * The viewport describes the transformation between the pipeline output and the target image.
     */
    VkViewport viewport;

    /**
     * The scissor describes which part of the viewport is valid to render into; anything outside the scissor is
     * not rendered.
     */
    VkRect2D scissor;

    /**
     * This describes how to create the viewport state.
     * This ties together the 'viewport' and 'scissor' members of this struct.
     */
    VkPipelineViewportStateCreateInfo viewportInfo;

    /**
     * This describes how to configure rasterization: depth, culling, triangle winding order, etc.
     */
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;

    /**
     * Describes how the rasterizer handles the edges of geometry (e.g. anti-aliasing)
     */
    VkPipelineMultisampleStateCreateInfo multisampleInfo;

    /**
     * Describes how we control two colors blending in our frame buffer (e.g. when two triangles overlap)
     */
    VkPipelineColorBlendAttachmentState colorBlendAttachment;

    /**
     * This describes how to create the blend state and references the 'colorBlendAttachment' in this struct.
     */
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;

    /**
     * Describes if and how depth and stencil testing is performed.
     */
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;


    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;
    uint32_t subpass;

    LTVKPipelineConfig() :
        inputAssemblyInfo({}),
        viewport({}),
        scissor({}),
        viewportInfo({}),
        rasterizationInfo({}),
        multisampleInfo({}),
        colorBlendAttachment({}),
        colorBlendInfo({}),
        depthStencilInfo({}),
        pipelineLayout(nullptr),
        renderPass(nullptr),
        subpass(0) {}

    // non-copyable
    LTVKPipelineConfig(const LTVKPipelineConfig&) = delete;
    void operator=(const LTVKPipelineConfig&) = delete;
    // non-movable
    LTVKPipelineConfig(LTVKPipelineConfig&&) = delete;
    LTVKPipelineConfig& operator=(LTVKPipelineConfig&&) = delete;
};

/**
 * Describes a graphics pipeline in Vulkan.
 */
class LTVKPipeline
{
    /**
     * Fields
     */
private:
    /**
     * The wrapper around the vulkan graphics device.
     */
    LTVKDevice* m_Device;

    /**
     * The vertex shader that is used in this pipeline.
     */
    LTShader* m_VertexShader;

    /**
     * The fragment shader that is used in this pipeline.
     */
    LTShader* m_FragmentShader;

    /**
     * The vulkan pipeline that this class wraps.
     */
    VkPipeline m_VkPipeline;

    /**
     * Constructors
     */
public:
    LTVKPipeline(
        LTVKDevice* device,
        LTShader* vertexShader,
        LTShader* fragmentShader);

    /**
     * Methods
     */
private:
public:
    bool Initialize(const LTVKPipelineConfig& config);
    void Destroy();

    /**
     * Gets the default pipeline configuration.
     */
    void GetDefaultPipelineConfig(LTVKPipelineConfig& outConfig, uint32_t width, uint32_t height);
};


