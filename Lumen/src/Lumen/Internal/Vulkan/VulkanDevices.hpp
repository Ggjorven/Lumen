#pragma once

#include "Lumen/Internal/Vulkan/Vulkan.hpp"

#include <cstdint>
#include <optional>

namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Internal structs
    ////////////////////////////////////////////////////////////////////////////////////
    struct QueueFamilyIndices
    {
    public:
        std::optional<uint32_t> GraphicsFamily;
        std::optional<uint32_t> ComputeFamily;
        std::optional<uint32_t> PresentFamily;

    public:
        static QueueFamilyIndices Find(VkSurfaceKHR surface, VkPhysicalDevice device);
        inline bool IsComplete() const { return GraphicsFamily.has_value() && ComputeFamily.has_value() && PresentFamily.has_value(); }
    };

    struct SwapChainSupportDetails
    {
    public:
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;

    public:
        static SwapChainSupportDetails Query(VkSurfaceKHR surface, VkPhysicalDevice device);
    };

    ////////////////////////////////////////////////////////////////////////////////////
    // Vulkan Physical Device
    ////////////////////////////////////////////////////////////////////////////////////
    class VulkanPhysicalDevice
    {
    public:
        // Constructor & Destructor
        VulkanPhysicalDevice(VkSurfaceKHR surface);
        ~VulkanPhysicalDevice() = default;

        // Methods
        VkFormat FindDepthFormat() const;
        VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

        // Getters
        inline VkPhysicalDevice GetVkPhysicalDevice() const { return m_PhysicalDevice; }
        
    private:
        // Private methods
        bool PhysicalDeviceSuitable(VkSurfaceKHR surface, VkPhysicalDevice device);
        bool ExtensionsSupported(VkPhysicalDevice device);

        bool FeaturesSupported(const VkPhysicalDeviceFeatures& requested, const VkPhysicalDeviceFeatures& found);
        bool FeaturesSupported(const VkPhysicalDeviceDescriptorIndexingFeatures& requested, const VkPhysicalDeviceDescriptorIndexingFeatures& found);

    private:
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    // Vulkan Logical Device
    ////////////////////////////////////////////////////////////////////////////////////
    class VulkanDevice
    {
    public:
        // Constructor & Destructor
        VulkanDevice(VkSurfaceKHR surface, VulkanPhysicalDevice& physicalDevice);
        ~VulkanDevice();

        // Methods
        void Wait() const;

        // Getters
        inline VkDevice GetVkDevice() const { return m_LogicalDevice; }

        inline VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
        inline VkQueue GetComputeQueue() const { return m_ComputeQueue; }
        inline VkQueue GetPresentQueue() const { return m_PresentQueue; }

        inline VulkanPhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }

    private:
        VulkanPhysicalDevice& m_PhysicalDevice;
        VkDevice m_LogicalDevice = VK_NULL_HANDLE;

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkQueue m_ComputeQueue = VK_NULL_HANDLE;
        VkQueue m_PresentQueue = VK_NULL_HANDLE;
    };

}