#include "lupch.h"
#include "VulkanCommandBuffer.hpp"

#include "Lumen/Internal/IO/Print.hpp"

#include "Lumen/Internal/Renderer/Renderer.hpp"

#include "Lumen/Internal/Vulkan/VulkanContext.hpp"

namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
	// Constructor & Destructor
    ////////////////////////////////////////////////////////////////////////////////////
    VulkanCommandBuffer::VulkanCommandBuffer()
    {
        VkDevice device = VulkanContext::GetVulkanDevice().GetVkDevice();

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = VulkanRenderer::GetRenderer().GetVkCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VK_VERIFY(vkAllocateCommandBuffers(device, &allocInfo, &m_CommandBuffer));

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VK_VERIFY(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore));
        
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VK_VERIFY(vkCreateFence(device, &fenceInfo, nullptr, &m_InFlightFence));
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        Renderer::GetRenderer().Free([commandBuffer = m_CommandBuffer, inFlightFence = m_InFlightFence, renderFinishedSemaphore = m_RenderFinishedSemaphore]()
        {
            VkDevice device = VulkanContext::GetVulkanDevice().GetVkDevice();

            auto& renderer = VulkanRenderer::GetRenderer();
            vkFreeCommandBuffers(device, renderer.GetVkCommandPool(), 1, &commandBuffer);

            //renderer.GetTaskManager().RemoveFromAll(renderFinishedSemaphores);
            //renderer.GetTaskManager().RemoveFromAll(inFlightFences);

            vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
            vkDestroyFence(device, inFlightFence, nullptr);
        });
    }

    ////////////////////////////////////////////////////////////////////////////////////
	// Constructor & Destructor
    ////////////////////////////////////////////////////////////////////////////////////
    VulkanRenderCommandBuffer::VulkanRenderCommandBuffer()
    {
        VkDevice device = VulkanContext::GetVulkanDevice().GetVkDevice();

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = VulkanRenderer::GetRenderer().GetVkCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

        VK_VERIFY(vkAllocateCommandBuffers(device, &allocInfo, m_CommandBuffers.data()));

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < RendererSpecification::FramesInFlight; i++)
        {
            VK_VERIFY(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]));
            VK_VERIFY(vkCreateFence(device, &fenceInfo, nullptr, &m_InFlightFences[i]));
        }
    }

    VulkanRenderCommandBuffer::~VulkanRenderCommandBuffer()
    {
        Renderer::GetRenderer().Free([commandBuffers = m_CommandBuffers, inFlightFences = m_InFlightFences, renderFinishedSemaphores = m_RenderFinishedSemaphores]()
        {
            VkDevice device = VulkanContext::GetVulkanDevice().GetVkDevice();

            auto& renderer = VulkanRenderer::GetRenderer();
            vkFreeCommandBuffers(device, renderer.GetVkCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

            for (size_t i = 0; i < renderFinishedSemaphores.size(); i++)
            {
                //renderer.GetTaskManager().RemoveFromAll(renderFinishedSemaphores[i]);
                //renderer.GetTaskManager().RemoveFromAll(inFlightFences[i]);

                vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
                vkDestroyFence(device, inFlightFences[i], nullptr);
            }
        });
    }

    ////////////////////////////////////////////////////////////////////////////////////
	// Getters
    ////////////////////////////////////////////////////////////////////////////////////
    VkCommandBuffer VulkanRenderCommandBuffer::GetVkCommandBuffer() const
    {
        //return GetVkCommandBuffer(VulkanRenderer::GetRenderer().GetVulkanSwapChain().GetCurrentFrame());
        return VK_NULL_HANDLE;
    }

    VkFence VulkanRenderCommandBuffer::GetVkInFlightFence() const
    {
        //return GetVkInFlightFence(VulkanRenderer::GetRenderer().GetVulkanSwapChain().GetCurrentFrame());
        return VK_NULL_HANDLE;
    }

    VkSemaphore VulkanRenderCommandBuffer::GetVkRenderFinishedSemaphore() const
    {
        //return GetVkRenderFinishedSemaphore(VulkanRenderer::GetRenderer().GetVulkanSwapChain().GetCurrentFrame());
        return VK_NULL_HANDLE;
    }

}