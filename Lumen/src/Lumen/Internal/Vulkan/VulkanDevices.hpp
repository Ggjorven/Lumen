#pragma once

#include "Lumen/Internal/Vulkan/Vulkan.hpp"

#include "Lumen/Internal/Enum/Bitwise.hpp"

#include "Lumen/Core/Core.hpp"

#include <cstdint>
#include <optional>

namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Internal structs
    ////////////////////////////////////////////////////////////////////////////////////
    enum class QueueFamilyFlags : uint16_t
    {
        None = 0, 
        Graphics = 1 << 0,
        Compute = 1 << 1,
        Transfer = 1 << 2,
        SparseBinding = 1 << 3,
        Protected = 1 << 4,
        VideoDecodeKHR = 1 << 5,
        VideoEncodeKHR = 1 << 6,
        OpticalFlowNV = 1 << 7,

        Present = 1 << 8
    };

    struct QueueFamilyInfo
    {
    public:
        uint32_t Index = 0;
        uint32_t Count = 0;

        QueueFamilyFlags Flags = QueueFamilyFlags::None;

    public:
        // Methods
        bool SupportsRequired() const; // Note: Checks for Graphics, Compute & Present
        bool EnoughQueues() const; // Note: Just checks if Count >= 3 (Graphics + Compute + Present)
    };

    struct QueueFamilyIndices
    {
    public:
        uint32_t QueueFamily = 0;

        uint32_t GraphicsQueue = 0;
        uint32_t ComputeQueue = 0;
        uint32_t PresentQueue = 0;

        std::vector<QueueFamilyInfo> Queues = {};

        bool CompletedQueues = false;

    public:
        // Methods
        forceinline bool IsComplete() const { return CompletedQueues; }
        bool SameQueue() const;

    public:
        static QueueFamilyIndices Find(VkSurfaceKHR surface, VkPhysicalDevice device); 
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
        forceinline VkPhysicalDevice GetVkPhysicalDevice() const { return m_PhysicalDevice; }
        
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
        forceinline VkDevice GetVkDevice() const { return m_LogicalDevice; }

        forceinline VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
        forceinline VkQueue GetComputeQueue() const { return m_ComputeQueue; }
        forceinline VkQueue GetPresentQueue() const { return m_PresentQueue; }

        forceinline VulkanPhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }

    private:
        VulkanPhysicalDevice& m_PhysicalDevice;
        VkDevice m_LogicalDevice = VK_NULL_HANDLE;

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkQueue m_ComputeQueue = VK_NULL_HANDLE;
        VkQueue m_PresentQueue = VK_NULL_HANDLE;
    };

}

template<>
struct Lumen::Enum::Customize<Lumen::Internal::QueueFamilyFlags>
{
public:
    inline static constexpr bool Bitwise = true;
};

#include "Lumen/Internal/Vulkan/VulkanDevices.inl"