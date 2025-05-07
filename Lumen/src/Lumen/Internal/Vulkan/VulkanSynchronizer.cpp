#include "lupch.h"
#include "VulkanSynchronizer.hpp"

#include "Lumen/Internal/IO/Print.hpp"
#include "Lumen/Internal/Utils/Profiler.hpp"

#include "Lumen/Internal/Vulkan/VulkanContext.hpp"
#include "Lumen/Internal/Vulkan/VulkanCommandBuffer.hpp"

namespace Lumen::Internal
{

	////////////////////////////////////////////////////////////////////////////////////
	// Frame
	////////////////////////////////////////////////////////////////////////////////////
	void VulkanSynchronizer::BakeFrameGraph(const FrameGraph& frame, uint8_t frameIndex)
	{

	}

	void VulkanSynchronizer::BakeCurrentFrameGraph(const FrameGraph& frame)
	{
		//BakeFrameGraph(frame, );
		//BakeFrameGraph(frame, ); // TODO: Use SwapChain frame
	}

}