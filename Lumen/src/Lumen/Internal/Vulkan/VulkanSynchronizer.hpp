#pragma once

#include "Lumen/Internal/Memory/Array.hpp"

#include "Lumen/Internal/Vulkan/Vulkan.hpp"

#include "Lumen/Internal/Renderer/FrameGraph.hpp"
#include "Lumen/Internal/Renderer/RendererSpec.hpp"

#include "Lumen/Core/Core.hpp"

#include <cstdint>
#include <tuple>
#include <utility>
#include <unordered_map>

namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
    // VulkanFrame
    ////////////////////////////////////////////////////////////////////////////////////
    struct VulkanFrame
    {
    public:
        //Array<std::pair<VkSemaphore, uint64_t>, static_cast<size_t>(Queue::COUNT)> TimelineSemaphores;
        //std::unordered_map<CommandBuffer*, std::pair<Queue, uint64_t>> a;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    // VulkanSynchronizer
    ////////////////////////////////////////////////////////////////////////////////////
    class VulkanSynchronizer
    {
    public:
        // Constructors & Destructor
        VulkanSynchronizer() = default;
        ~VulkanSynchronizer() = default;

        // Methods

        // Frame
        void BakeFrameGraph(const FrameGraph& frame, uint8_t frameIndex);
        void BakeCurrentFrameGraph(const FrameGraph& frame);

    private:
        //Array<
    };

}

#include "Lumen/Internal/Vulkan/VulkanSynchronizer.inl"