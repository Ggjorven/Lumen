#pragma once

#include "Lumen/Internal/Renderer/RendererSpec.hpp"
//#include "Lumen/Internal/Renderer/Buffers.hpp"
//#include "Lumen/Internal/Renderer/Pipeline.hpp"
//#include "Lumen/Internal/Renderer/Renderpass.hpp"
//#include "Lumen/Internal/Renderer/CommandBuffer.hpp"

#include "Lumen/Internal/Vulkan/Vulkan.hpp"

#include "Lumen/Internal/Vulkan/VulkanBuffers.hpp"

//#include "Lumen/Internal/Vulkan/VulkanSwapChain.hpp"
//#include "Lumen/Internal/Vulkan/VulkanTaskManager.hpp"

#include <cstdint>
#include <queue>
#include <mutex>

namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
    // VulkanRenderer
    ////////////////////////////////////////////////////////////////////////////////////
    class VulkanRenderer
    {
    public:
        // Constructor & Destructor
        VulkanRenderer(const RendererSpecification& specs);
        ~VulkanRenderer();

        // Methods
        //void BeginFrame();
        //void EndFrame();
        //void Present();

        // Object methods
        //void Begin(CommandBuffer& cmdBuf);
        //void Begin(Renderpass& renderpass);
        //void End(CommandBuffer& cmdBuf);
        //void End(Renderpass& renderpass);
        //void Submit(CommandBuffer& cmdBuf, ExecutionPolicy policy, Queue queue, PipelineStage waitStage, const std::vector<CommandBuffer*>& waitOn);
        //void Submit(Renderpass& renderpass, ExecutionPolicy policy, Queue queue, PipelineStage waitStage, const std::vector<CommandBuffer*>& waitOn);

        //void NextSubpass(CommandBuffer& cmdBuf);
        //void NextSubpass(Renderpass& renderpass);

        //void Draw(CommandBuffer& cmdBuf, uint32_t vertexCount, uint32_t instanceCount);
        //void DrawIndexed(CommandBuffer& cmdBuf, uint32_t indexCount, uint32_t instanceCount);
        //void DrawIndexed(CommandBuffer& cmdBuf, IndexBuffer& indexBuffer, uint32_t instanceCount);

        // Internal
        void Free(const FreeFn& fn);
        void FreeQueue();

        void Recreate(uint32_t width, uint32_t height, bool vsync);

        // Getters
        forceinline const RendererSpecification& GetSpecification() const { return m_Specification; }

        //ImageFormat GetColourFormat() const;
        //ImageFormat GetDepthFormat() const;
        //std::vector<Image*> GetSwapChainImages();

        // Internal getters
        //inline VulkanTaskManager& GetTaskManager() { return m_TaskManager; }
        //inline VulkanSwapChain& GetVulkanSwapChain() { return m_SwapChain; }
        forceinline VulkanStagingBufferRegistry& GetStagingBuffers() { return m_StagingBuffers; }
        forceinline VkCommandPool GetVkCommandPool() const { return m_CommandPool; }

        // Static methods
		forceinline static VulkanRenderer& GetRenderer() { return *s_Renderer; }

    private:
        // Private methods
        //void SetViewportAndScissor(CommandBuffer& cmdBuf, uint32_t width, uint32_t height);

        // Init methods
        void InitCommandPool(VkSurfaceKHR surface);

    private:
        std::mutex m_FreeMutex = {};
        std::queue<FreeFn> m_FreeQueue = {};

        //VulkanSwapChain m_SwapChain = {};
        //VulkanTaskManager m_TaskManager = {};

        RendererSpecification m_Specification;
        
        VkCommandPool m_CommandPool = VK_NULL_HANDLE;
        VulkanStagingBufferRegistry m_StagingBuffers = {};

        inline static VulkanRenderer* s_Renderer = nullptr;
    };

}