#include "lupch.h"
#include "VulkanRenderer.hpp"

#include "Lumen/Internal/IO/Print.hpp"

#include "Lumen/Internal/Core/Window.hpp"

#include "Lumen/Internal/Vulkan/VulkanContext.hpp"

#if defined (LU_PLATFORM_DESKTOP)
	#include <GLFW/glfw3.h>
#endif

namespace Lumen::Internal
{

	////////////////////////////////////////////////////////////////////////////////////
	// Constructor & Destructor
	////////////////////////////////////////////////////////////////////////////////////
	VulkanRenderer::VulkanRenderer(const RendererSpecification& specs)
		: m_Specification(specs)
	{
		s_Renderer = this;

		//m_TaskManager.Init();

		VkSurfaceKHR surface = VK_NULL_HANDLE;
		#if defined(LU_PLATFORM_DESKTOP)
		VK_VERIFY(glfwCreateWindowSurface(VulkanContext::GetVkInstance(), static_cast<GLFWwindow*>(specs.WindowRef->GetNativeWindow()), nullptr, &surface));
		#endif

		InitCommandPool(surface);

		// Note: We pass ownership of the surface to the swapchain
		//m_SwapChain.Init(specs.WindowRef);
	}

	VulkanRenderer::~VulkanRenderer()
	{
		// Wait for the device (GPU) to finish
		VulkanDevice& device = VulkanContext::GetVulkanDevice();
		device.Wait();

		vkDestroyCommandPool(device.GetVkDevice(), m_CommandPool, nullptr);

		FreeQueue();
		//m_SwapChain.Destroy();
		//m_TaskManager.Destroy();
		FreeQueue();

		s_Renderer = nullptr;
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Internal
	////////////////////////////////////////////////////////////////////////////////////
	void VulkanRenderer::Free(const FreeFn& fn)
	{
		LU_PROFILE("VkRenderer::Free()");

		std::scoped_lock<std::mutex> lock(m_FreeMutex);
		m_FreeQueue.push(fn);
	}

	void VulkanRenderer::FreeQueue()
	{
		LU_PROFILE("VkRenderer::FreeQueue()");

		std::scoped_lock<std::mutex> lock(m_FreeMutex);
		while (!m_FreeQueue.empty())
		{
			m_FreeQueue.front()();
			m_FreeQueue.pop();
		}
	}

	void VulkanRenderer::Recreate(uint32_t width, uint32_t height, bool vsync)
	{
		// TODO: ...
		//m_SwapChain->Resize();
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Private methods
	////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////
	// Init methods
	////////////////////////////////////////////////////////////////////////////////////
	void VulkanRenderer::InitCommandPool(VkSurfaceKHR surface)
	{
		QueueFamilyIndices queueFamilyIndices = QueueFamilyIndices::Find(surface, VulkanContext::GetVulkanPhysicalDevice().GetVkPhysicalDevice());

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Allows us to reset the command buffer and reuse it.
		poolInfo.queueFamilyIndex = queueFamilyIndices.QueueFamily;

		VK_VERIFY(vkCreateCommandPool(VulkanContext::GetVulkanDevice().GetVkDevice(), &poolInfo, nullptr, &m_CommandPool));
	}

}