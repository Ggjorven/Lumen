namespace Lumen::Internal
{

	////////////////////////////////////////////////////////////////////////////////////
	// Constructors & Destructors
	////////////////////////////////////////////////////////////////////////////////////
	forceinline VulkanCommandView::VulkanCommandView(const VulkanCommandBuffer& cmd)
		: m_Type(Type::CommandBuffer), m_CommandBuffer(static_cast<const void*>(&cmd))
	{
	}

	forceinline VulkanCommandView::VulkanCommandView(const VulkanRenderCommandBuffer& cmd)
		: m_Type(Type::RenderCommandBuffer), m_CommandBuffer(static_cast<const void*>(&cmd))
	{
	}
	
	////////////////////////////////////////////////////////////////////////////////////
    // Getters
    ////////////////////////////////////////////////////////////////////////////////////
	hintinline VkCommandBuffer VulkanCommandView::GetVkCommandBuffer() const
	{
		LU_ASSERT((m_Type != Type::None), "[VkCommandView] CommandBuffer not properly initialized!");
		LU_ASSERT((m_CommandBuffer != nullptr), "[VkCommandView] CommandBuffer is not initialized!");

		switch (m_Type)
		{
		case Type::CommandBuffer: 
			return static_cast<const VulkanCommandBuffer*>(m_CommandBuffer)->GetVkCommandBuffer();
		case Type::RenderCommandBuffer: 
			return static_cast<const VulkanRenderCommandBuffer*>(m_CommandBuffer)->GetVkCommandBuffer();
		}

		return VK_NULL_HANDLE;
	}

	hintinline VkFence VulkanCommandView::GetVkInFlightFence() const
	{
		LU_ASSERT((m_Type != Type::None), "[VkCommandView] CommandBuffer not properly initialized!");
		LU_ASSERT((m_CommandBuffer != nullptr), "[VkCommandView] CommandBuffer is not initialized!");

		switch (m_Type)
		{
		case Type::CommandBuffer:
			return static_cast<const VulkanCommandBuffer*>(m_CommandBuffer)->GetVkInFlightFence();
		case Type::RenderCommandBuffer:
			return static_cast<const VulkanRenderCommandBuffer*>(m_CommandBuffer)->GetVkInFlightFence();
		}

		return VK_NULL_HANDLE;
	}

	hintinline VkSemaphore VulkanCommandView::GetVkRenderFinishedSemaphore() const
	{
		LU_ASSERT((m_Type != Type::None), "[VkCommandView] CommandBuffer not properly initialized!");
		LU_ASSERT((m_CommandBuffer != nullptr), "[VkCommandView] CommandBuffer is not initialized!");

		switch (m_Type)
		{
		case Type::CommandBuffer:
			return static_cast<const VulkanCommandBuffer*>(m_CommandBuffer)->GetVkRenderFinishedSemaphore();
		case Type::RenderCommandBuffer:
			return static_cast<const VulkanRenderCommandBuffer*>(m_CommandBuffer)->GetVkRenderFinishedSemaphore();
		}

		return VK_NULL_HANDLE;
	}

}