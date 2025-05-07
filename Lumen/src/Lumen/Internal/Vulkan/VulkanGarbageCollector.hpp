#pragma once

#include "Lumen/Internal/Vulkan/Vulkan.hpp"

#include "Lumen/Core/Core.hpp"

#include <mutex>
#include <initializer_list>

namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
    // GarbageEntries
    ////////////////////////////////////////////////////////////////////////////////////
    struct CommandBufferGarbageEntry
    {
    public:
        VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

    public:
        // Constructors & Destructor
        CommandBufferGarbageEntry() = default;
        forceinline CommandBufferGarbageEntry(VkCommandBuffer cmdBuf)
            : CommandBuffer(cmdBuf) {}
        ~CommandBufferGarbageEntry() = default;
    };

    struct ImageGarbageEntry
    {
    public:
        VkImage Image = VK_NULL_HANDLE;
        VmaAllocation Allocation = VK_NULL_HANDLE;
        VkImageView ImageView = VK_NULL_HANDLE;
        VkSampler Sampler = VK_NULL_HANDLE;

    public:
        // Constructors & Destructor
        ImageGarbageEntry() = default;
        forceinline ImageGarbageEntry(VkImage image, VmaAllocation allocation, VkImageView imageView, VkSampler sampler)
            : Image(image), Allocation(allocation), ImageView(imageView), Sampler(sampler) {}
        ~ImageGarbageEntry() = default;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    // VulkanGarbageCollector
    ////////////////////////////////////////////////////////////////////////////////////
    class VulkanGarbageCollector
    {
    public:
        // Constructor & Destructor
        VulkanGarbageCollector() = default;
        ~VulkanGarbageCollector() = default;

        // Methods
        void Collect(CommandBufferGarbageEntry commandBuffer);
        void Collect(std::initializer_list<CommandBufferGarbageEntry> commandBuffers);

        void Collect(ImageGarbageEntry image);
        void Collect(std::initializer_list<ImageGarbageEntry> images);

        void Dispose();

    private:
        std::mutex m_ThreadSafety = {};

        std::vector<VkCommandBuffer> m_CommandBuffers = { };
        std::vector<ImageGarbageEntry> m_Images = { };
    };

}

#include "Lumen/Internal/Vulkan/VulkanGarbageCollector.inl"