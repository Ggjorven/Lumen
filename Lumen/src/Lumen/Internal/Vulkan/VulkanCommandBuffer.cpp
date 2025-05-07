#include "lupch.h"
#include "VulkanCommandBuffer.hpp"

#include "Lumen/Internal/IO/Print.hpp"

#include "Lumen/Internal/Renderer/Renderer.hpp"

#include "Lumen/Internal/Vulkan/VulkanContext.hpp"
#include "Lumen/Internal/Vulkan/VulkanRenderer.hpp"

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
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        VulkanRenderer::GetRenderer().GetGarbageCollector().Collect(m_CommandBuffer);
    }

    ////////////////////////////////////////////////////////////////////////////////////
	// Constructor & Destructor
    ////////////////////////////////////////////////////////////////////////////////////
    VulkanRenderCommandBuffer::VulkanRenderCommandBuffer()
    {
        VkDevice device = VulkanContext::GetVulkanDevice().GetVkDevice();

        Array<VkCommandBuffer, RendererSpecification::FramesInFlight> temp = {};

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = VulkanRenderer::GetRenderer().GetVkCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

        VK_VERIFY(vkAllocateCommandBuffers(device, &allocInfo, temp.data()));

        // Pass through to CommandBuffer wrappers
        for (size_t i = 0; i < temp.size(); i++)
            m_CommandBuffers[i].Construct(temp[i]);
    }

    VulkanRenderCommandBuffer::~VulkanRenderCommandBuffer()
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////
	// Getters
    ////////////////////////////////////////////////////////////////////////////////////
    VkCommandBuffer VulkanRenderCommandBuffer::GetVkCommandBuffer() const // TODO: ...
    {
        //return GetVkCommandBuffer(VulkanRenderer::GetRenderer().GetVulkanSwapChain().GetCurrentFrame());
        return VK_NULL_HANDLE;
    }

    VulkanCommandBuffer& VulkanRenderCommandBuffer::GetCommandBuffer()
    {
        //return GetCommandBuffer(VulkanRenderer::GetRenderer().GetVulkanSwapChain().GetCurrentFrame());
        return *static_cast<VulkanCommandBuffer*>(nullptr);
    }

}