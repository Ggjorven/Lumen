#include "lupch.h"
#include "VulkanSwapChain.hpp"

#include "Lumen/Internal/IO/Print.hpp"
#include "Lumen/Internal/Utils/Profiler.hpp"

#include "Lumen/Internal/Core/Window.hpp"

#include "Lumen/Internal/Vulkan/VulkanContext.hpp"
#include "Lumen/Internal/Vulkan/VulkanCommandBuffer.hpp"

#if defined(LU_PLATFORM_DESKTOP)
	#define GLFW_INCLUDE_VULKAN
	#include <GLFW/glfw3.h>
#endif

namespace Lumen::Internal
{

	////////////////////////////////////////////////////////////////////////////////////
	// Constructor & Destructor
	////////////////////////////////////////////////////////////////////////////////////
	VulkanSwapChain::VulkanSwapChain(Window* window, VkSurfaceKHR surface, ImageFormat requestedFormat) // Note: The swapchain takes ownership of the surface
		: m_Surface(surface), m_Window(window)
	{
		FindImageFormatAndColorSpace(requestedFormat);

		// Synchronization objects
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		for (size_t i = 0; i < RendererSpecification::FramesInFlight; i++)
		{
			VK_VERIFY(vkCreateSemaphore(VulkanContext::GetVulkanDevice().GetVkDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]));
		}
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		VulkanDevice& device = VulkanContext::GetVulkanDevice();
		device.Wait();

		if (m_SwapChain)
			vkDestroySwapchainKHR(device.GetVkDevice(), m_SwapChain, nullptr);

		for (auto& image : m_Images)
			image.Destroy();

		for (size_t i = 0; i < RendererSpecification::FramesInFlight; i++)
			vkDestroySemaphore(device.GetVkDevice(), m_ImageAvailableSemaphores[i], nullptr);

		vkDestroySurfaceKHR(VulkanContext::GetVkInstance(), m_Surface, nullptr);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Methods
	////////////////////////////////////////////////////////////////////////////////////
	void VulkanSwapChain::Resize(uint32_t width, uint32_t height, bool vsync)
	{
		if (width == 0 || height == 0)
			return;

		///////////////////////////////////////////////////////////
		// SwapChain
		///////////////////////////////////////////////////////////
		SwapChainSupportDetails details = SwapChainSupportDetails::Query(m_Surface, VulkanContext::GetVulkanPhysicalDevice().GetVkPhysicalDevice());

		VkExtent2D swapchainExtent = {};
		// If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
		if (details.Capabilities.currentExtent.width == 0xFFFFFFFF)
		{
			// If the surface size is undefined, the size is set to
			// the size of the images requested.
			swapchainExtent.width = width;
			swapchainExtent.height = height;
		}
		else
		{
			// If the surface size is defined, the swap chain size must match
			swapchainExtent = details.Capabilities.currentExtent;
		}

		// The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
		// This mode waits for the vertical blank ("v-sync")
		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

		// If v-sync is not requested, try to find a mailbox mode
		// It's the lowest latency non-tearing present mode available
		if (!vsync)
		{
			for (size_t i = 0; i < details.PresentModes.size(); i++)
			{
				if (details.PresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
					break;
				}
				if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (details.PresentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
				{
					swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				}
			}
		}

		LU_ASSERT((details.Capabilities.minImageCount <= RendererSpecification::FramesInFlight), "[VkSwapChain] FramesInFlight is less than the minimum amount of SwapChain images.");
		LU_ASSERT((RendererSpecification::FramesInFlight < details.Capabilities.maxImageCount), "[VkSwapChain] FramesInFlight is more than the maximum amount of SwapChain images.");

		// Find the transformation of the surface
		VkSurfaceTransformFlagsKHR preTransform;
		if (details.Capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		else
			preTransform = details.Capabilities.currentTransform;

		// Find a supported composite alpha format (not all devices support alpha opaque)
		VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		// Simply select the first composite alpha format available
		std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};
		for (auto& compositeAlphaFlag : compositeAlphaFlags)
		{
			if (details.Capabilities.supportedCompositeAlpha & compositeAlphaFlag)
			{
				compositeAlpha = compositeAlphaFlag;
				break;
			}
		}

		VkSwapchainCreateInfoKHR swapchainCI = {};
		swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCI.pNext = nullptr;
		swapchainCI.surface = m_Surface;
		swapchainCI.minImageCount = static_cast<uint32_t>(RendererSpecification::FramesInFlight);
		swapchainCI.imageFormat = m_ColourFormat;
		swapchainCI.imageColorSpace = m_ColourSpace;
		swapchainCI.imageExtent = { swapchainExtent.width, swapchainExtent.height };

		// Note: I don't know why we would use this, but here's an example implementation.
		//swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		//if (details.Capabilities.supportedTransforms & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
		//	swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		//if (details.Capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		//	swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		swapchainCI.preTransform = static_cast<VkSurfaceTransformFlagBitsKHR>(preTransform);
		swapchainCI.imageArrayLayers = 1;
		swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCI.queueFamilyIndexCount = 0;
		swapchainCI.pQueueFamilyIndices = nullptr;
		swapchainCI.presentMode = swapchainPresentMode;
		swapchainCI.oldSwapchain = m_SwapChain;
		// Setting clipped to VK_TRUE allows the implementation to discard rendering outside of the surface area
		swapchainCI.clipped = VK_TRUE;
		swapchainCI.compositeAlpha = compositeAlpha;

		auto device = VulkanContext::GetVulkanDevice().GetVkDevice();
		auto oldSwapchain = m_SwapChain;
		VK_VERIFY(vkCreateSwapchainKHR(device, &swapchainCI, nullptr, &m_SwapChain));

		if (oldSwapchain)
			vkDestroySwapchainKHR(device, oldSwapchain, nullptr); // Destroys swapchain images

		// Get the swap chain images
		uint32_t imageCount = 0;
		Array<VkImage, RendererSpecification::FramesInFlight> tempImages = { };

		VK_VERIFY(vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, nullptr));
		LU_ASSERT((imageCount == RendererSpecification::FramesInFlight), "[VkSwapChain] Created image count doesn't match requested FramesInFlight");
		VK_VERIFY(vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, tempImages.data()));

		for (uint32_t i = 0; i < imageCount; i++)
		{
			VkImageViewCreateInfo colorAttachmentView = {};
			colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorAttachmentView.pNext = nullptr;
			colorAttachmentView.format = m_ColourFormat;
			colorAttachmentView.image = tempImages[i];
			colorAttachmentView.components = {
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A
			};
			colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			colorAttachmentView.subresourceRange.baseMipLevel = 0;
			colorAttachmentView.subresourceRange.levelCount = 1;
			colorAttachmentView.subresourceRange.baseArrayLayer = 0;
			colorAttachmentView.subresourceRange.layerCount = 1;
			colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
			colorAttachmentView.flags = 0;

			VkImageView imageView = VK_NULL_HANDLE;
			VK_VERIFY(vkCreateImageView(device, &colorAttachmentView, nullptr, &imageView));

			ImageSpecification specs = {
				.Usage = ImageUsage::Colour,
				.Layout = ImageLayout::Undefined,
				.Format = VkFormatToImageFormat(m_ColourFormat),
				.Width = width,
				.Height = height,
				.MipMaps = false
			};

			/* // TODO: ... (commandBuffer)
			if (m_Images[i].GetWidth() != 0 && m_Images[i].GetHeight() != 0)
			{
				VulkanImage& src = m_Images[i];

				// Destroy old image view
				m_Window->GetRenderer().GetInternalRenderer().Free([device = device, imageView = src.m_ImageView]()
					{
						vkDestroyImageView(device, imageView, nullptr);
					});

				// Set new data
				src.m_ImageSpecification = specs;
				src.m_Image = tempImages[i];
				src.m_ImageView = imageView;
			}
			else
				m_Images[i].Init(m_Window->GetRenderer().GetID(), specs, tempImages[i], imageView);

			// We transition manually since, the layout set in the specification doesn't get used
			// since we manually set all the data.
			m_Images[i].Transition(m_RendererID, ImageLayout::Undefined, ImageLayout::PresentSrcKHR);
			*/
		}
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Private methods
	////////////////////////////////////////////////////////////////////////////////////
	uint32_t VulkanSwapChain::AcquireNextImage()
	{
		LU_PROFILE("VkSwapChain::AcquireImage()");
		uint32_t imageIndex = 0;

		VkResult result = vkAcquireNextImageKHR(VulkanContext::GetVulkanDevice().GetVkDevice(), m_SwapChain, std::numeric_limits<uint64_t>::max(), m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			Resize(m_Window->GetSize().x, m_Window->GetSize().y, m_Window->GetRenderer().GetSpecification().VSync);
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			LU_LOG_ERROR("[VulkanSwapChain] Failed to acquire SwapChain image!");
		}

		m_AcquiredImage = imageIndex;
		return imageIndex;
	}

	void VulkanSwapChain::FindImageFormatAndColorSpace(ImageFormat requestedFormat)
	{
		VkPhysicalDevice physicalDevice = VulkanContext::GetVulkanPhysicalDevice().GetVkPhysicalDevice();

		// Get list of supported surface formats
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, nullptr);

		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, surfaceFormats.data());

		// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
		// there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
		if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
		{
			m_ColourFormat = ImageFormatToVkFormat(requestedFormat);
			m_ColourSpace = surfaceFormats[0].colorSpace;
		}
		else
		{
			bool foundDesiredFormat = false;
			for (auto& surfaceFormat : surfaceFormats)
			{
				if (surfaceFormat.format == ImageFormatToVkFormat(requestedFormat))
				{
					m_ColourFormat = surfaceFormat.format;
					m_ColourSpace = surfaceFormat.colorSpace;
					foundDesiredFormat = true;
					break;
				}
			}

			// in case desired format is not available
			// select the first available color format
			if (!foundDesiredFormat)
			{
				m_ColourFormat = surfaceFormats[0].format;
				m_ColourSpace = surfaceFormats[0].colorSpace;
			}
		}
	}

}