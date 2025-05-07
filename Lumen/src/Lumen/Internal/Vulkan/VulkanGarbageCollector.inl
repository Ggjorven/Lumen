#include "VulkanGarbageCollector.hpp"
namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Methods
    ////////////////////////////////////////////////////////////////////////////////////
    hintinline void VulkanGarbageCollector::Collect(CommandBufferGarbageEntry commandBuffer)
    {
        std::scoped_lock<std::mutex> lock(m_ThreadSafety);
        m_CommandBuffers.push_back(commandBuffer.CommandBuffer);
    }

    hintinline void VulkanGarbageCollector::Collect(std::initializer_list<CommandBufferGarbageEntry> commandBuffers)
    {
        std::scoped_lock<std::mutex> lock(m_ThreadSafety);

        m_CommandBuffers.reserve(m_CommandBuffers.size() + commandBuffers.size());

        // Note: This is currently allows since a CommandBufferGarbageEntry is just a wrapper around VkCommandBuffer
        const VkCommandBuffer* ptr = reinterpret_cast<const VkCommandBuffer*>(commandBuffers.begin());
        m_CommandBuffers.insert(m_CommandBuffers.end(), ptr, ptr + commandBuffers.size());
    }

    hintinline void VulkanGarbageCollector::Collect(ImageGarbageEntry image)
    {
        std::scoped_lock<std::mutex> lock(m_ThreadSafety);
        m_Images.push_back(image);
    }

    hintinline void VulkanGarbageCollector::Collect(std::initializer_list<ImageGarbageEntry> images)
    {
        std::scoped_lock<std::mutex> lock(m_ThreadSafety);

        m_Images.reserve(m_Images.size() + images.size());
        m_Images.insert(m_Images.end(), images.begin(), images.end());
    }

}