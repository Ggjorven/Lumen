#pragma once

#include "Lumen/Internal/Renderer/RendererSpec.hpp"
#include "Lumen/Internal/Renderer/ImageSpec.hpp"

#include "Lumen/Internal/Vulkan/VulkanImage.hpp"

#include <filesystem>

namespace Lumen::Internal
{

    class CommandBuffer;

    ////////////////////////////////////////////////////////////////////////////////////
    // Image
    ////////////////////////////////////////////////////////////////////////////////////
    class Image
    {
    public:
        using Type = VulkanImage;
    public:
        // Constructors & Destructor
        forceinline Image(const CommandBuffer& initCmd, const ImageSpecification& imageSpec, const SamplerSpecification& samplerSpec)
            : m_Image(initCmd, imageSpec, samplerSpec) {}
        forceinline Image(const CommandBuffer& initCmd, const ImageSpecification& imageSpec, const SamplerSpecification& samplerSpec, const std::filesystem::path& imagePath)
            : m_Image(initCmd, imageSpec, samplerSpec, imagePath) {}
        ~Image() = default;

        // Methods
        forceinline void SetData(const CommandBuffer& cmd, void* data, size_t size) { m_Image.SetData(cmd, data, size); }

        forceinline void Resize(const CommandBuffer& cmd, uint32_t width, uint32_t height) { m_Image.Resize(cmd, width, height); }

        forceinline void Transition(const CommandBuffer& cmd, ImageLayout initial, ImageLayout final) { m_Image.Transition(cmd, initial, final); }

        // Getters
        forceinline const ImageSpecification& GetSpecification() const { return m_Image.GetSpecification(); }
        forceinline const SamplerSpecification& GetSamplerSpecification() const { return m_Image.GetSamplerSpecification(); }

        forceinline uint32_t GetWidth() const { return m_Image.GetWidth(); }
        forceinline uint32_t GetHeight() const { return m_Image.GetHeight(); }

        // Internal
        forceinline Type& GetInternalImage() { return m_Image; }

    private:
        Type m_Image;
    };

}