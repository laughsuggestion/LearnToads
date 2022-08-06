#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

struct LTVKSwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct LTVKQueueFamilyIndices {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    bool graphicsFamilyHasValue = false;
    bool presentFamilyHasValue = false;
    bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
};

#ifdef NDEBUG
#define LTVK_ENABLE_VALIDATION_LAYERS 0
#else
#define LTVK_ENABLE_VALIDATION_LAYERS 1
#endif

class LTVKDevice
{
private:
    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    class LTGameWindow& m_Window;
    VkCommandPool m_CommandPool;

    VkDevice m_Device;
    VkSurfaceKHR m_Surface;
    VkQueue m_GraphicsQueue;
    VkQueue m_PresentQueue;
    VkPhysicalDeviceProperties m_Properties;

    const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
    const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

private:
    // non-copyable
    LTVKDevice(const LTVKDevice&) = delete;
    void operator=(const LTVKDevice&) = delete;
    // non-movable
    LTVKDevice(LTVKDevice&&) = delete;
    LTVKDevice& operator=(LTVKDevice&&) = delete;

public:
    LTVKDevice(class LTGameWindow& gameWindow) :
        m_Window(gameWindow) {}

public:
    bool Initialize();
    void Destroy();
    VkCommandPool GetCommandPool() { return m_CommandPool; }
    VkDevice GetDevice() { return m_Device; }
    VkSurfaceKHR GetSurface() { return m_Surface; }
    VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }
    VkQueue GetPresentQueue() { return m_PresentQueue; }

    LTVKSwapChainSupportDetails GetSwapChainSupport()
    {
        return QuerySwapChainSupport(m_PhysicalDevice);
    }

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    LTVKQueueFamilyIndices FindPhysicalQueueFamilies()
    {
        return FindQueueFamilies(m_PhysicalDevice);
    }

    VkFormat FindSupportedFormat(
        const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlags features);

    void CreateBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& bufferMemory);

    VkCommandBuffer BeginSingleTimeCommands();

    void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void CopyBufferToImage(
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height,
        uint32_t layerCount);

    void CreateImageWithInfo(
        const VkImageCreateInfo& imageInfo,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory& imageMemory);

private:
    bool Initialize_CreateInstance();
    bool Initialize_SetupDebugMessenger();
    bool Initialize_CreateSurface();
    bool Initialize_PickPhysicalDevice();
    bool Initialize_CreateLogicalDevice();
    bool Initialize_CreateCommandPool();

    // helper functions
    bool IsDeviceSuitable(VkPhysicalDevice device);
    std::vector<const char*> GetRequiredExtensions();
    bool CheckValidationLayerSupport();
    LTVKQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void HasGflwRequiredInstanceExtensions();
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    LTVKSwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
};