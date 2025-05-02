#pragma once

#include "Lumen/Internal/Renderer/RendererSpec.hpp"

#include "Lumen/Internal/Memory/Array.hpp"
#include "Lumen/Internal/Memory/DeferredConstruct.hpp"

#include "Lumen/Internal/Vulkan/Vulkan.hpp"
#include "Lumen/Internal/Vulkan/VulkanImage.hpp"

#include <cstdint>

namespace Lumen::Internal
{

    class Window;

    ////////////////////////////////////////////////////////////////////////////////////
    // Vulkan SwapChain
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
        inline VkFormat GetColourFormat() const { return m_ColourFormat; }

        inline uint32_t GetCurrentFrame() const { return m_CurrentFrame; }
        inline uint32_t GetAquiredImage() const { return m_AcquiredImage; }

        inline Array<DeferredConstruct<VulkanImage, true>, RendererSpecification::FramesInFlight>& GetSwapChainImages() { return m_Images; }

        inline VkSurfaceKHR GetVkSurface() const { return m_Surface; }

        inline VkSemaphore GetImageAvailableSemaphore() const { return GetImageAvailableSemaphore(m_CurrentFrame); }
        inline VkSemaphore GetImageAvailableSemaphore(uint32_t index) const { return m_ImageAvailableSemaphores[index]; }

    private:
        // Private methods
        uint32_t AcquireNextImage();
        void FindImageFormatAndColorSpace(ImageFormat requestedFormat);

    private:
        VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface;

        Array<DeferredConstruct<VulkanImage, true>, RendererSpecification::FramesInFlight> m_Images = { };
        Array<VkSemaphore, RendererSpecification::FramesInFlight> m_ImageAvailableSemaphores = { };

        VkFormat m_ColourFormat = VK_FORMAT_UNDEFINED;
        VkColorSpaceKHR m_ColourSpace = VK_COLOR_SPACE_MAX_ENUM_KHR;

        Window* m_Window;

        uint32_t m_CurrentFrame = 0;
        uint32_t m_AcquiredImage = 0;
    };

}