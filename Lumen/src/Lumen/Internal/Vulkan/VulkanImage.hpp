#pragma once

#include "Lumen/Internal/Renderer/RendererSpec.hpp"
#include "Lumen/Internal/Renderer/ImageSpec.hpp"

#include "Lumen/Internal/Vulkan/Vulkan.hpp"

#include "Lumen/Internal/Enum/Bitwise.hpp"

#include <tuple>
#include <filesystem>

namespace Lumen::Internal
{

    class CommandView;

    ////////////////////////////////////////////////////////////////////////////////////
    // Convert functions
    ////////////////////////////////////////////////////////////////////////////////////
    constexpr ImageUsage VkImageUsageToImageUsage(VkImageUsageFlags usage);
    constexpr VkImageUsageFlags ImageUsageToVkImageUsage(ImageUsage usage);
    constexpr ImageLayout VkImageLayoutToImageLayout(VkImageLayout layout);
    constexpr VkImageLayout ImageLayoutToVkImageLayout(ImageLayout layout);
    constexpr ImageFormat VkFormatToImageFormat(VkFormat format);
    constexpr VkFormat ImageFormatToVkFormat(ImageFormat format);

    constexpr FilterMode VkFilterToFilterMode(VkFilter filter);
    constexpr VkFilter FilterModeToVkFilter(FilterMode filter);
    constexpr AddressMode VkSamplerAddressModeToAddressMode(VkSamplerAddressMode mode);
    constexpr VkSamplerAddressMode AddressModeToVkSamplerAddressMode(AddressMode mode);
    constexpr MipmapMode VkSamplerMipmapModeToMipmapMode(VkSamplerMipmapMode mode);
    constexpr VkSamplerMipmapMode MipmapModeToVkSamplerMipmapMode(MipmapMode mode);

    constexpr VkImageAspectFlags VkFormatToVkImageAspectFlags(VkFormat format);

    constexpr bool VkFormatIsDepth(VkFormat format);
    constexpr bool VkFormatHasStencil(VkFormat format);

    ////////////////////////////////////////////////////////////////////////////////////
    // VulkanImage
    ////////////////////////////////////////////////////////////////////////////////////
    class VulkanImage
    {
    public:
        // Constructors & Destructor
        VulkanImage(const CommandView& initCmd, const ImageSpecification& imageSpecs, const SamplerSpecification& samplerSpecs);
        VulkanImage(const CommandView& initCmd, const ImageSpecification& imageSpecs, const SamplerSpecification& samplerSpecs, const std::filesystem::path& imagePath);
        VulkanImage(const ImageSpecification& imageSpecs, VkImage image, VkImageView imageView); // Note: This exists for swapchain images
        ~VulkanImage();

        // Methods
        void SetData(const CommandView& cmd, void* data, size_t size);

        void Resize(const CommandView& cmd, uint32_t width, uint32_t height);

        void Transition(const CommandView& cmd, ImageLayout initial, ImageLayout final);

        // Getters
        forceinline const ImageSpecification& GetSpecification() const { return m_ImageSpecification; }
        forceinline const SamplerSpecification& GetSamplerSpecification() const { return m_SamplerSpecification; }

        forceinline uint32_t GetWidth() const { return m_ImageSpecification.Width; }
        forceinline uint32_t GetHeight() const { return m_ImageSpecification.Height; }

        // Internal getters
        forceinline VkImage GetVkImage() const { return m_Image; }
        forceinline VmaAllocation GetVmaAllocation() const { return m_Allocation; }
        forceinline VkImageView GetVkImageView() const { return m_ImageView; }
        forceinline VkSampler GetVkSampler() const { return m_Sampler; }

    private:
        // Create & Destroy
        void CreateImage(const CommandView& cmd, uint32_t width, uint32_t height);
        void CreateImage(const CommandView& cmd, const std::filesystem::path& imagePath);
        void GenerateMipmaps(const CommandView& cmd, VkImage& image, VkFormat imageFormat, VkImageLayout desiredLayout, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
        void DestroyImage();

        // Private methods
        void SetData(const CommandView& cmd, void* data, size_t size, ImageLayout desiredLayout);
        std::tuple<VkPipelineStageFlags, VkPipelineStageFlags, VkImageMemoryBarrier> GetImageBarrier(VkImageLayout src, VkImageLayout dst, uint32_t mipLevels) const;

    private:
        VkImage m_Image = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;
        VkImageView m_ImageView = VK_NULL_HANDLE;
        VkSampler m_Sampler = VK_NULL_HANDLE;

        ImageSpecification m_ImageSpecification;
        SamplerSpecification m_SamplerSpecification;

        uint32_t m_Miplevels = 1;
    };

}

#include "Lumen/Internal/Vulkan/VulkanImage.inl"