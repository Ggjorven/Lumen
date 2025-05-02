#include "lupch.h"
#include "VulkanImage.hpp"

#include "Lumen/Internal/IO/Print.hpp"
#include "Lumen/Internal/Utils/Profiler.hpp"

#include "Lumen/Internal/Enum/Fuse.hpp"

#include "Lumen/Internal/Renderer/Renderer.hpp"
#include "Lumen/Internal/Renderer/CommandBuffer.hpp"

#include "Lumen/Internal/Vulkan/VulkanContext.hpp"
#include "Lumen/Internal/Vulkan/VulkanCommandBuffer.hpp"

//#define STBI_ASSERT(x) LU_ASSERT(x, std::format("[VkImage:stb_image] '{0}'", #x))

#if defined(LU_COMPILER_GCC)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"

    #define STB_IMAGE_IMPLEMENTATION
    #include <stb/stb_image.h>

    #pragma GCC diagnostic pop
#elif !defined(LU_PLATFORM_APPLE)
    #pragma warning(push, 0)

    #define STB_IMAGE_IMPLEMENTATION
    #include <stb/stb_image.h>

    #pragma warning(pop)
#else
    #define STB_IMAGE_IMPLEMENTATION
    #include <stb/stb_image.h>
#endif

namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Methods
    ////////////////////////////////////////////////////////////////////////////////////
    void VulkanImage::Resize(const CommandView& cmd, uint32_t width, uint32_t height)
    {
        DestroyImage();

        m_ImageSpecification.Width = width;
        m_ImageSpecification.Height = height;

        CreateImage(cmd, width, height);
    }

    void VulkanImage::Transition(const CommandView& cmd, ImageLayout initial, ImageLayout final)
    {
        if (initial == final)
            return;

        auto [srcStage, dstStage, imageBarrier] = GetImageBarrier(ImageLayoutToVkImageLayout(initial), ImageLayoutToVkImageLayout(final), m_Miplevels);
        vkCmdPipelineBarrier(cmd.GetInternalCommandView().GetVkCommandBuffer(), srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier);

        m_ImageSpecification.Layout = final;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Create & Destroy
    ////////////////////////////////////////////////////////////////////////////////////
    void VulkanImage::CreateImage(const CommandView& cmd, uint32_t width, uint32_t height)
    {
        ImageLayout desiredLayout = m_ImageSpecification.Layout;

        if (m_ImageSpecification.MipMaps)
            m_Miplevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

        m_Allocation = VulkanAllocator::AllocateImage(VMA_MEMORY_USAGE_GPU_ONLY, m_Image, width, height, m_Miplevels, ImageFormatToVkFormat(m_ImageSpecification.Format), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | ImageUsageToVkImageUsage(m_ImageSpecification.Usage));
        m_ImageView = VulkanAllocator::CreateImageView(m_Image, ImageFormatToVkFormat(m_ImageSpecification.Format), VkFormatToVkImageAspectFlags(ImageFormatToVkFormat(m_ImageSpecification.Format)), m_Miplevels);
        m_Sampler = VulkanAllocator::CreateSampler(FilterModeToVkFilter(m_SamplerSpecification.MagFilter), FilterModeToVkFilter(m_SamplerSpecification.MinFilter), AddressModeToVkSamplerAddressMode(m_SamplerSpecification.Address), MipmapModeToVkSamplerMipmapMode(m_SamplerSpecification.Mipmaps), m_Miplevels);

        Transition(cmd, ImageLayout::Undefined, desiredLayout);
    }

    void VulkanImage::CreateImage(const CommandView& cmd, const std::filesystem::path& imagePath)
    {
        ImageLayout desiredLayout = m_ImageSpecification.Layout;

        int width, height, texChannels;

        stbi_set_flip_vertically_on_load(1);
        stbi_uc* pixels = stbi_load(imagePath.string().c_str(), &width, &height, &texChannels, STBI_rgb_alpha); // STBI_default, STBI_rgb_alpha

        LU_ASSERT((pixels != nullptr), std::format("[VkImage] Failed to load image from '{0}'", imagePath.string()));

        m_ImageSpecification.Width = static_cast<uint32_t>(width);
        m_ImageSpecification.Height = static_cast<uint32_t>(height);
        if (m_ImageSpecification.MipMaps)
            m_Miplevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

        m_ImageSpecification.Format = ImageFormat::RGBA;
        size_t imageSize = static_cast<size_t>(m_ImageSpecification.Width) * m_ImageSpecification.Height * 4ull;

        m_Allocation = VulkanAllocator::AllocateImage(VMA_MEMORY_USAGE_GPU_ONLY, m_Image, m_ImageSpecification.Width, m_ImageSpecification.Height, m_Miplevels, ImageFormatToVkFormat(m_ImageSpecification.Format), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | ImageUsageToVkImageUsage(m_ImageSpecification.Usage));
        m_ImageView = VulkanAllocator::CreateImageView(m_Image, ImageFormatToVkFormat(m_ImageSpecification.Format), VK_IMAGE_ASPECT_COLOR_BIT, m_Miplevels);
        m_Sampler = VulkanAllocator::CreateSampler(FilterModeToVkFilter(m_SamplerSpecification.MagFilter), FilterModeToVkFilter(m_SamplerSpecification.MinFilter), AddressModeToVkSamplerAddressMode(m_SamplerSpecification.Address), MipmapModeToVkSamplerMipmapMode(m_SamplerSpecification.Mipmaps), m_Miplevels);

        Transition(cmd, ImageLayout::Undefined, ImageLayout::TransferDst);

        SetData(cmd, static_cast<void*>(pixels), imageSize, desiredLayout);
        stbi_image_free(static_cast<void*>(pixels));
    }

    void VulkanImage::GenerateMipmaps(const CommandView& cmd, VkImage& image, VkFormat imageFormat, VkImageLayout desiredLayout, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
    {
        LU_ASSERT((mipLevels != 0 && mipLevels != 1), "[VkImage] Trying to generate mipmaps with no miplevels.");

        VkCommandBuffer commandBuffer = cmd.GetInternalCommandView().GetVkCommandBuffer();

        // Check if image format supports linear blitting
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(VulkanContext::GetVulkanPhysicalDevice().GetVkPhysicalDevice(), imageFormat, &formatProperties);

        LU_VERIFY((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT), "[VulkanImage] Texture image format does not support linear blitting!");

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        for (uint32_t i = 1; i < mipLevels; i++)
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

            VkImageBlit blit = {};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        auto [srcStage, dstStage, newBarrier] = GetImageBarrier(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, desiredLayout, mipLevels);

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = desiredLayout;
        barrier.srcAccessMask = newBarrier.srcAccessMask;
        barrier.dstAccessMask = newBarrier.dstAccessMask;;

        vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    void VulkanImage::DestroyImage()
    {
        VulkanRenderer::GetRenderer().Free([sampler = m_Sampler, imageView = m_ImageView, image = m_Image, allocation = m_Allocation]()
        {
            auto device = VulkanContext::GetVulkanDevice().GetVkDevice();

            if (sampler)
                vkDestroySampler(device, sampler, nullptr);
            if (imageView)
                vkDestroyImageView(device, imageView, nullptr);

            if (image != VK_NULL_HANDLE && allocation != VK_NULL_HANDLE)
                VulkanAllocator::DestroyImage(image, allocation);
        });
    }

    void VulkanImage::SetData(const CommandView& cmd, void* data, size_t size, ImageLayout desiredLayout)
    {
        VulkanStagingBuffer& stagingBuffer = VulkanRenderer::GetRenderer().GetStagingBuffers().GetBuffer(size);
        stagingBuffer.SetData(data, size);

        Transition(cmd, m_ImageSpecification.Layout, ImageLayout::TransferDst);
        VulkanAllocator::CopyBufferToImage(cmd.GetInternalCommandView().GetVkCommandBuffer(), stagingBuffer.Buffer, m_Image, m_ImageSpecification.Width, m_ImageSpecification.Height);

        if (m_ImageSpecification.MipMaps)
        {
            GenerateMipmaps(cmd, m_Image, ImageFormatToVkFormat(m_ImageSpecification.Format), ImageLayoutToVkImageLayout(desiredLayout), m_ImageSpecification.Width, m_ImageSpecification.Height, m_Miplevels);
        }
        else
        {
            Transition(cmd, ImageLayout::TransferDst, desiredLayout);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Private methods
    ////////////////////////////////////////////////////////////////////////////////////
    std::tuple<VkPipelineStageFlags, VkPipelineStageFlags, VkImageMemoryBarrier> VulkanImage::GetImageBarrier(VkImageLayout src, VkImageLayout dst, uint32_t mipLevels) const
    {
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = src;
        barrier.newLayout = dst;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrier.image = m_Image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage = 0;
        VkPipelineStageFlags destinationStage = 0;

        // Aspect checks
        if (VkFormatIsDepth(ImageFormatToVkFormat(m_ImageSpecification.Format)))
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            // Check if it has stencil component
            if (VkFormatHasStencil(ImageFormatToVkFormat(m_ImageSpecification.Format)))
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }

        LU_ASSERT(false, "[VkImage] TODO: Implement GetImageBarrier()");
        switch (Enum::Fuse(src, dst))
        {
        // Undefined // TODO: Set this
        //case Enum::Fuse(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL):
        //{
        //    barrier.srcAccessMask = 0;
        //    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        //    
        //    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        //    destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        //    break;
        }

        return { sourceStage, destinationStage, barrier };
    }

}