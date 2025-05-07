#pragma once

#include "Lumen/Internal/Vulkan/VulkanCommandBuffer.hpp"

#include "Lumen/Core/Core.hpp"

namespace Lumen::Internal
{

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

        // Internal
        forceinline Type& GetInternalCommandBuffer() { return m_CommandBuffer; }
        forceinline const Type& GetInternalCommandBuffer() const { return m_CommandBuffer; }

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
        forceinline CommandBuffer& GetCommandBuffer() { return *reinterpret_cast<CommandBuffer*>(&m_CommandBuffer.GetCommandBuffer()); }
        forceinline CommandBuffer& GetCommandBuffer(uint32_t index) { return *reinterpret_cast<CommandBuffer*>(&m_CommandBuffer.GetCommandBuffer(index)); }

        // Internal
        forceinline Type& GetInternalRenderCommandBuffer() { return m_CommandBuffer; }

    private:
        Type m_CommandBuffer = {};
    }; 

}