#pragma once

#include "Lumen/Internal/Memory/Array.hpp"
#include "Lumen/Internal/Memory/DeferredConstruct.hpp"

#include "Lumen/Internal/Renderer/RendererSpec.hpp"

#include "Lumen/Internal/Vulkan/Vulkan.hpp"

#include "Lumen/Core/Core.hpp"

namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
    // VulkanCommandBuffer
    ////////////////////////////////////////////////////////////////////////////////////
    class VulkanCommandBuffer
    {
    public:
        // Constructor & Destructor
        VulkanCommandBuffer();
        VulkanCommandBuffer(VkCommandBuffer commandBuffer); // Note: This makes this object own the commandBuffer and will get rid of it
        ~VulkanCommandBuffer();

        // The Begin, End & Submit methods are in the Renderer class.

        // Getters
        forceinline VkCommandBuffer GetVkCommandBuffer() const { return m_CommandBuffer; }

    private:
        VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    // VulkanRenderCommandBuffer
    ////////////////////////////////////////////////////////////////////////////////////
    class VulkanRenderCommandBuffer
    {
    public:
        // Constructor & Destructor
        VulkanRenderCommandBuffer();
        ~VulkanRenderCommandBuffer();

        // The Begin, End & Submit methods are in the Renderer class.

        // Getters
        VkCommandBuffer GetVkCommandBuffer() const;
        forceinline VkCommandBuffer GetVkCommandBuffer(uint32_t index) const { return m_CommandBuffers[index]->GetVkCommandBuffer(); }
        VulkanCommandBuffer& GetCommandBuffer();
        forceinline VulkanCommandBuffer& GetCommandBuffer(uint32_t index) { return m_CommandBuffers[index]; }

    private:
        Array<DeferredConstruct<VulkanCommandBuffer>, RendererSpecification::FramesInFlight> m_CommandBuffers = {};
    };

}

#include "Lumen/Internal/Vulkan/VulkanCommandBuffer.inl"