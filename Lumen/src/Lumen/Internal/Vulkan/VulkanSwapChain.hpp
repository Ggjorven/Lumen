#pragma once

#include "Lumen/Internal/Renderer/RendererSpec.hpp"

#include "Lumen/Internal/Memory/Array.hpp"
#include "Lumen/Internal/Memory/DeferredConstruct.hpp"

#include "Lumen/Internal/Vulkan/Vulkan.hpp"
#include "Lumen/Internal/Vulkan/VulkanImage.hpp"

#include "Lumen/Core/Core.hpp"

#include <cstdint>

namespace Lumen::Internal
{

    class Window;

    ////////////////////////////////////////////////////////////////////////////////////
    // VulkanSwapChain
    ////////////////////////////////////////////////////////////////////////////////////
    class VulkanSwapChain
    {
    public:
        // Constructors & Destructor
        VulkanSwapChain(Window* window, VkSurfaceKHR surface, ImageFormat requestedFormat);
        ~VulkanSwapChain();

        // Methods
        void Resize(uint32_t width, uint32_t height, bool vsync);

        // Getters
        forceinline VkFormat GetColourFormat() const { return m_ColourFormat; }

        forceinline uint32_t GetCurrentFrame() const { return m_CurrentFrame; }
        forceinline uint32_t GetAquiredImage() const { return m_AcquiredImage; }

        forceinline Array<DeferredConstruct<VulkanImage, true>, RendererSpecification::FramesInFlight>& GetSwapChainImages() { return m_Images; }

        forceinline VkSurfaceKHR GetVkSurface() const { return m_Surface; }

        forceinline VkSemaphore GetImageAvailableSemaphore() const { return m_ImageAvailableSemaphores[m_CurrentFrame]; }
        forceinline VkSemaphore GetRenderFinishedSemaphore() const { return m_RenderFinishedSemaphores[m_CurrentFrame]; }

    private:
        // Private methods
        uint32_t AcquireNextImage();
        void FindImageFormatAndColorSpace(ImageFormat requestedFormat);

    private:
        VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface;

        Array<DeferredConstruct<VulkanImage, true>, RendererSpecification::FramesInFlight> m_Images = { };
        Array<VkSemaphore, RendererSpecification::FramesInFlight> m_ImageAvailableSemaphores = { };
        Array<VkSemaphore, RendererSpecification::FramesInFlight> m_RenderFinishedSemaphores = { };

        VkFormat m_ColourFormat = VK_FORMAT_UNDEFINED;
        VkColorSpaceKHR m_ColourSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        Window* m_Window;

        uint32_t m_CurrentFrame = 0;
        uint32_t m_AcquiredImage = 0;
    };

}