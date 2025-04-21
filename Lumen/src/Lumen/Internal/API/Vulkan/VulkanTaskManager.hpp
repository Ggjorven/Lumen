#pragma once

#include "Lumen/Internal/Renderer/RendererSpec.hpp"

#include "Lumen/Internal/API/Vulkan/Vulkan.hpp"

#include "Lumen/Internal/API/Vulkan/VulkanCommandBuffer.hpp"

#include <cstdint>
#include <vector>
#include <unordered_map>

namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
    // VulkanTaskManager
    ////////////////////////////////////////////////////////////////////////////////////
    class VulkanTaskManager
    {
    public:
        // Constructor & Destructor
        VulkanTaskManager() = default;
        ~VulkanTaskManager() = default;

		// Init & Destroy
		void Init(const RendererID rendererID, uint32_t frameCount);
		void Destroy();

        // Add methods
        void Add(VulkanCommandBuffer& cmdBuf, ExecutionPolicy policy);
        void Add(VkSemaphore semaphore); // Internal function for swapchain image available semaphore

        // Remove & Reset methods
        void Remove(VkFence fence); // It removes the fence from current frame if it exists
        void Remove(VkFence fence, uint32_t frame); // It removes the fence from the frame vector

        void Remove(VkSemaphore semaphore); // It removes the semaphore from current frame if it exists (it checks both vectors)
        void Remove(VkSemaphore semaphore, uint32_t frame); // It removes the semaphore from the frame vector
        
        void ResetFences(); // Resets current frame semaphores
        void ResetSemaphores(); // Resets current frame semaphores
        
        void RemoveFromAll(VkFence fence); // It removes the fence from all vectors if it exists
        void RemoveFromAll(VkSemaphore semaphore); // It removes the semaphore from all vectors if it exists

        // Getters
        VkSemaphore GetNext(); // Returns the next semaphore in the queue

        std::vector<VkFence>& GetFences();
        std::vector<VkSemaphore>& GetSemaphores();

    private:
        RendererID m_RendererID = 0;
        std::mutex m_ThreadSafety = {};

        // The first vector index is the frame, the pair is first = semaphores to wait on by other cmdBufs, second = semaphores to only wait on by frame
        std::vector<std::pair<std::vector<VkSemaphore>, std::vector<VkSemaphore>>> m_Semaphores = { };
        std::vector<std::vector<VkFence>> m_Fences = { }; // Waited on by Renderer::EndFrame
    };

}