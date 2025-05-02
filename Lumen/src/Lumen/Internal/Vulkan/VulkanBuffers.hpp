#pragma once

#include "Lumen/Internal/Utils/Profiler.hpp"
#include "Lumen/Internal/Memory/Array.hpp"

#include "Lumen/Internal/Renderer/RendererSpec.hpp"

#include "Lumen/Internal/Vulkan/Vulkan.hpp"

#include "Lumen/Core/Core.hpp"

#include <queue>
#include <vector>

namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
    // VulkanStagingBuffer
    ////////////////////////////////////////////////////////////////////////////////////
    struct VulkanStagingBuffer
    {
    public:
        VkBuffer Buffer = VK_NULL_HANDLE;
        VmaAllocation Allocation = VK_NULL_HANDLE;
        void* Mapped = nullptr;
        size_t Size = 0;

    public:
        // Constructor & Destructor
        VulkanStagingBuffer(VkBuffer buffer, VmaAllocation allocation, void* mapped, size_t size);
        VulkanStagingBuffer(const VulkanStagingBuffer& other);
        ~VulkanStagingBuffer() = default;

        // Methods
        void SetData(void* data, size_t size);
    };

    ////////////////////////////////////////////////////////////////////////////////////
    // VulkanStagingBufferRegistry
    ////////////////////////////////////////////////////////////////////////////////////
    class VulkanStagingBufferRegistry
    {
    public:
        // Constructor & Destructor
        VulkanStagingBufferRegistry() = default;
        ~VulkanStagingBufferRegistry();

        // Methods
        void RetireUsed();

        // Getters
        VulkanStagingBuffer& GetBuffer(size_t size);

    private:
        // Private methods
        VulkanStagingBuffer& CreateBuffer(size_t bitsNeeded);

        void DestroyBuffers(std::queue<VulkanStagingBuffer>& buffers);
        void DestroyBuffers(std::vector<VulkanStagingBuffer>& buffers);

    public:
        // Ascending in size and indexed by bits necessary to fit the size (std::bit_width) - 1
        Array<Array<std::queue<VulkanStagingBuffer>, 35>, RendererSpecification::FramesInFlight> m_Buffers = { };
        Array<std::vector<VulkanStagingBuffer>, RendererSpecification::FramesInFlight> m_InUse = { };
    };

}

#include "Lumen/Internal/Vulkan/VulkanBuffers.inl"