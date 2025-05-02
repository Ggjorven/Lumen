#pragma once

#include "Lumen/Internal/Vulkan/VulkanCommandBuffer.hpp"

#include "Lumen/Core/Core.hpp"

namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
    // CommandView
    ////////////////////////////////////////////////////////////////////////////////////
    class CommandView
    {
    public:
        using Type = VulkanCommandView;
    public:
        // Constructor & Destructor
        forceinline CommandView(const VulkanCommandBuffer& commandBuffer)
            : m_CommandView(commandBuffer) {}
        forceinline CommandView(const VulkanRenderCommandBuffer& commandBuffer)
            : m_CommandView(commandBuffer) {}
        ~CommandView() = default;

        // Internal
        forceinline const Type& GetInternalCommandView() const { return m_CommandView; }

    private:
        Type m_CommandView;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    // CommandBuffer
    ////////////////////////////////////////////////////////////////////////////////////
    class CommandBuffer
    {
    public:
        using Type = VulkanCommandBuffer;
    public:
        // Constructor & Destructor
        CommandBuffer() = default;
        ~CommandBuffer() = default;

        // The Begin, End & Submit methods are in the Renderer class.

        // Getters
        forceinline CommandView GetView() const { return { m_CommandBuffer }; }
        forceinline operator CommandView() const { return GetView(); }

        // Internal
        forceinline Type& GetInternalCommandBuffer() { return m_CommandBuffer; }

    private:
        Type m_CommandBuffer = {};
    };

    ////////////////////////////////////////////////////////////////////////////////////
    // RenderCommandBuffer
    ////////////////////////////////////////////////////////////////////////////////////
    class RenderCommandBuffer
    {
    public:
		using Type = VulkanRenderCommandBuffer;
    public:
        // Constructor & Destructor
        RenderCommandBuffer() = default;
        ~RenderCommandBuffer() = default;

        // The Begin, End & Submit methods are in the Renderer class.

        // Getters
        forceinline CommandView GetView() const { return { m_CommandBuffer }; }
        forceinline operator CommandView() const { return GetView(); }

        // Internal
        forceinline Type& GetInternalRenderCommandBuffer() { return m_CommandBuffer; }

    private:
        Type m_CommandBuffer = {};
    }; 

}