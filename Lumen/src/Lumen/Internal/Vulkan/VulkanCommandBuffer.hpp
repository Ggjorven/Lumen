#pragma once

#include "Lumen/Internal/Memory/Array.hpp"

#include "Lumen/Internal/Renderer/RendererSpec.hpp"

#include "Lumen/Internal/Vulkan/Vulkan.hpp"

#include "Lumen/Core/Core.hpp"

namespace Lumen::Internal
{

    class VulkanCommandBuffer;
    class VulkanRenderCommandBuffer;

    ////////////////////////////////////////////////////////////////////////////////////
    // VulkanCommandView
    ////////////////////////////////////////////////////////////////////////////////////
    class VulkanCommandView
    {
	public:
		// Constructor & Destructor
		VulkanCommandView(const VulkanCommandBuffer& cmd);
		VulkanCommandView(const VulkanRenderCommandBuffer& cmd);
		~VulkanCommandView() = default;

		// Getters
        VkCommandBuffer GetVkCommandBuffer() const;
        VkFence GetVkInFlightFence() const;
        VkSemaphore GetVkRenderFinishedSemaphore() const;
    
    private:
		enum class Type : uint8_t { None = 0, CommandBuffer, RenderCommandBuffer };
    private:
        Type m_Type = Type::None;
		const void* m_CommandBuffer = nullptr;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    // VulkanCommandBuffer
    ////////////////////////////////////////////////////////////////////////////////////
    class VulkanCommandBuffer
    {
    public:
        // Constructor & Destructor
        VulkanCommandBuffer();
        ~VulkanCommandBuffer();

        // The Begin, End & Submit methods are in the Renderer class.

        // Getters
        forceinline VkCommandBuffer GetVkCommandBuffer() const { return m_CommandBuffer; }
        forceinline VkFence GetVkInFlightFence() const { return m_InFlightFence; }
        forceinline VkSemaphore GetVkRenderFinishedSemaphore() const { return m_RenderFinishedSemaphore; }

    private:
        VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;

        // Synchronization objects
        VkFence m_InFlightFence = VK_NULL_HANDLE;
        VkSemaphore m_RenderFinishedSemaphore = VK_NULL_HANDLE;
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
        forceinline VkCommandBuffer GetVkCommandBuffer(uint32_t index) const { return m_CommandBuffers[index]; }
        VkFence GetVkInFlightFence() const;
        forceinline VkFence GetVkInFlightFence(uint32_t index) const { return m_InFlightFences[index]; }
        VkSemaphore GetVkRenderFinishedSemaphore() const;
        forceinline VkSemaphore GetVkRenderFinishedSemaphore(uint32_t index) const { return m_RenderFinishedSemaphores[index]; }

    private:
        Array<VkCommandBuffer, RendererSpecification::FramesInFlight> m_CommandBuffers = {};

        // Synchronization objects
		Array<VkFence, RendererSpecification::FramesInFlight> m_InFlightFences = {};
		Array<VkSemaphore, RendererSpecification::FramesInFlight> m_RenderFinishedSemaphores = {};
    };

}

#include "Lumen/Internal/Vulkan/VulkanCommandBuffer.inl"