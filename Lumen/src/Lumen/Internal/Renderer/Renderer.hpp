#pragma once

#include "Lumen/Internal/Renderer/ImageSpec.hpp"
#include "Lumen/Internal/Renderer/RendererSpec.hpp"

#include "Lumen/Internal/Vulkan/VulkanRenderer.hpp"

#include "Lumen/Core/Core.hpp"

namespace Lumen::Internal
{

	////////////////////////////////////////////////////////////////////////////////////
	// Renderer
	////////////////////////////////////////////////////////////////////////////////////
	class Renderer
	{
	public:
		using Type = VulkanRenderer;
	public:
        // Constructor & Destructor
        Renderer(const RendererSpecification& specs);
        ~Renderer();

        // Methods
        //inline void BeginFrame() { m_Renderer.BeginFrame(); }
        //inline void EndFrame() { m_Renderer.EndFrame(); }
        //inline void Present() { m_Renderer.Present(); }

        // Object methods
        //inline void Begin(CommandBuffer& cmdBuf) { m_Renderer.Begin(cmdBuf); }
        //inline void Begin(Renderpass& renderpass) { m_Renderer.Begin(renderpass); }
        //inline void End(CommandBuffer& cmdBuf) { m_Renderer.End(cmdBuf); }
        //inline void End(Renderpass& renderpass) { m_Renderer.End(renderpass); }
        //inline void Submit(CommandBuffer& cmdBuf, ExecutionPolicy policy, Queue queue = Queue::Graphics, PipelineStage waitStage = PipelineStage::ColourAttachmentOutput, const //std::vector<CommandBuffer*>& waitOn = {}) { m_Renderer.Submit(cmdBuf, policy, queue, waitStage, waitOn); }
        //inline void Submit(Renderpass& renderpass, ExecutionPolicy policy, Queue queue = Queue::Graphics, PipelineStage waitStage = PipelineStage::ColourAttachmentOutput, const std::vector<CommandBuffer*>& waitOn = {}) { m_Renderer.Submit(renderpass, policy, queue, waitStage, waitOn); }

        //inline void NextSubpass(CommandBuffer& cmdBuf) { m_Renderer.NextSubpass(cmdBuf); }
        //inline void NextSubpass(Renderpass& renderpass) { m_Renderer.NextSubpass(renderpass); }

        //inline void Draw(CommandBuffer& cmdBuf, uint32_t vertexCount = 3, uint32_t instanceCount = 1) { m_Renderer.Draw(cmdBuf, vertexCount, instanceCount); }
        //inline void DrawIndexed(CommandBuffer& cmdBuf, uint32_t indexCount, uint32_t instanceCount = 1) { m_Renderer.DrawIndexed(cmdBuf, indexCount, instanceCount); }
        //inline void DrawIndexed(CommandBuffer& cmdBuf, IndexBuffer& indexBuffer, uint32_t instanceCount = 1) { m_Renderer.DrawIndexed(cmdBuf, indexBuffer, instanceCount); }

        // Internal
        forceinline void Free(const FreeFn& fn) { m_Renderer.Free(fn); }
        forceinline void FreeQueue() { m_Renderer.FreeQueue(); }

        forceinline void Recreate(uint32_t width, uint32_t height, bool vsync) { m_Renderer.Recreate(width, height, vsync); }

        // Getters
        forceinline const RendererSpecification& GetSpecification() const { return m_Renderer.GetSpecification(); }
        
        //inline ImageFormat GetColourFormat() const { return m_Renderer.GetColourFormat(); }
        //inline ImageFormat GetDepthFormat() const { return m_Renderer.GetDepthFormat(); }
        //inline std::vector<Image*> GetSwapChainImages() { return m_Renderer.GetSwapChainImages(); }

        // Internal
        forceinline Type& GetInternalRenderer() { return m_Renderer; }

        // Static methods
		forceinline static Renderer& GetRenderer() { return *s_Renderer; }

    private:
        Type m_Renderer;
		
        inline static Renderer* s_Renderer = nullptr;
	};

}

#include "Lumen/Internal/Renderer/Renderer.inl"