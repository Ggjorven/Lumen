#include "VulkanBuffers.hpp"
namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Constructor
    ////////////////////////////////////////////////////////////////////////////////////
    forceinline VulkanStagingBuffer::VulkanStagingBuffer(VkBuffer buffer, VmaAllocation allocation, void* mapped, size_t size)
        : Buffer(buffer), Allocation(allocation), Mapped(mapped), Size(size)
    {
    }

    forceinline VulkanStagingBuffer::VulkanStagingBuffer(const VulkanStagingBuffer& other)
        : Buffer(other.Buffer), Allocation(other.Allocation), Mapped(other.Mapped), Size(other.Size)
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Methods
    ////////////////////////////////////////////////////////////////////////////////////
    hintinline void VulkanStagingBuffer::SetData(void* data, size_t size)
    {
        LU_PROFILE("VkStagingBuffer::SetData()");
        memcpy(Mapped, data, size);
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Destructor
    ////////////////////////////////////////////////////////////////////////////////////
    hintinline VulkanStagingBufferRegistry::~VulkanStagingBufferRegistry()
    {
        for (auto& frameBuffers : m_Buffers)
        {
            for (auto& buffers : frameBuffers)
                DestroyBuffers(buffers);
        }

        for (size_t i = 0; i < RendererSpecification::FramesInFlight; i++)
            DestroyBuffers(m_InUse[i]);
    }

}