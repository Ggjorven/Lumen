#include "lupch.h"
#include "VulkanBuffers.hpp"

#include "Lumen/Internal/IO/Print.hpp"

#include "Lumen/Internal/Vulkan/VulkanRenderer.hpp"

#include <bit>
#include <ranges>

namespace Lumen::Internal
{

	////////////////////////////////////////////////////////////////////////////////////
	// Methods
	////////////////////////////////////////////////////////////////////////////////////
	void VulkanStagingBufferRegistry::RetireUsed()
	{
		uint32_t frame = 0; //VulkanRenderer::GetRenderer().GetVulkanSwapChain().GetCurrentFrame(); // TODO: Replace all frame instances

		for (auto& buffer : m_InUse[frame])
		{
			size_t bitsNeeded = std::bit_width(buffer.Size);
			size_t index = bitsNeeded - 1;
		
			m_Buffers[frame][index].emplace(buffer);
		}

		m_InUse[frame].clear();
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Getters
	////////////////////////////////////////////////////////////////////////////////////
	VulkanStagingBuffer& VulkanStagingBufferRegistry::GetBuffer(size_t size)
	{
		LU_ASSERT((size != 0), "[VkStagingBufferRegistry] Invalid size passed in.");

		uint32_t frame = 0; //VulkanRenderer::GetRenderer().GetVulkanSwapChain().GetCurrentFrame(); // TODO: Replace all frame instances
		size_t bitsNeeded = std::bit_width(size);
		size_t index = bitsNeeded - 1;

		auto& buffers = m_Buffers[frame][index];
		
		if (!buffers.empty()) // [[likely]]
		{
			VulkanStagingBuffer& buffer = m_InUse[frame].emplace_back(buffers.front()); // Copy in to InUse array
			buffers.pop();
			return buffer;
		}

		// If no buffer usable create a new one directly in the InUse
		return CreateBuffer(bitsNeeded);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Private methods
	////////////////////////////////////////////////////////////////////////////////////
	VulkanStagingBuffer& VulkanStagingBufferRegistry::CreateBuffer(size_t bitsNeeded)
	{
		uint32_t frame = 0; //VulkanRenderer::GetRenderer().GetVulkanSwapChain().GetCurrentFrame(); // TODO: Replace all frame instances
		size_t bufferSize = std::bit_ceil(bitsNeeded);

		VkBuffer buffer = VK_NULL_HANDLE;
		VmaAllocation allocation = VulkanAllocator::AllocateBuffer(VMA_MEMORY_USAGE_CPU_ONLY, buffer, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

		void* mappedData = nullptr;
		VulkanAllocator::MapMemory(allocation, mappedData);

		return m_InUse[frame].emplace_back(buffer, allocation, mappedData, bufferSize);
	}

	void VulkanStagingBufferRegistry::DestroyBuffers(std::queue<VulkanStagingBuffer>& buffers)
	{
		while (!buffers.empty())
		{
			auto& front = buffers.front();

			VulkanAllocator::UnMapMemory(front.Allocation);
			VulkanAllocator::DestroyBuffer(front.Buffer, front.Allocation);

			buffers.pop();
		}
	}

	void VulkanStagingBufferRegistry::DestroyBuffers(std::vector<VulkanStagingBuffer>& buffers)
	{
		for (auto& buffer : buffers)
		{
			VulkanAllocator::UnMapMemory(buffer.Allocation);
			VulkanAllocator::DestroyBuffer(buffer.Buffer, buffer.Allocation);
		}

		buffers.clear();
	}

}
