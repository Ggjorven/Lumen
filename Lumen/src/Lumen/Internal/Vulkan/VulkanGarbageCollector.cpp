#include "lupch.h"
#include "VulkanGarbageCollector.hpp"

#include "Lumen/Internal/IO/Print.hpp"

#include "Lumen/Internal/Vulkan/VulkanContext.hpp"
#include "Lumen/Internal/Vulkan/VulkanRenderer.hpp"

namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Methods
    ////////////////////////////////////////////////////////////////////////////////////
    void VulkanGarbageCollector::Dispose()
    {
        std::scoped_lock<std::mutex> lock(m_ThreadSafety);

        VkDevice device = VulkanContext::GetVulkanDevice().GetVkDevice();
        VulkanRenderer& renderer = VulkanRenderer::GetRenderer();

        // CommandBuffers
        if (!m_CommandBuffers.empty()) // [[unlikely]]
        {
            vkFreeCommandBuffers(device, renderer.GetVkCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
            m_CommandBuffers.clear();
        }

        // Images
        if (!m_Images.empty()) // [[unlikely]]
        {
            for (auto& image : m_Images)
            {
                if (image.Sampler)
                    vkDestroySampler(device, image.Sampler, nullptr);
                if (image.ImageView)
                    vkDestroyImageView(device, image.ImageView, nullptr);

                if (image.Image != VK_NULL_HANDLE && image.Allocation != VK_NULL_HANDLE)
                    VulkanAllocator::DestroyImage(image.Image, image.Allocation);
            }
            m_Images.clear();
        }
    }

}