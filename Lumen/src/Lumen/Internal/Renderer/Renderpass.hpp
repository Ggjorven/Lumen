#pragma once

#include "Lumen/Internal/Utils/Settings.hpp"

#include "Lumen/Internal/Renderer/RendererSpec.hpp"
#include "Lumen/Internal/Renderer/RenderpassSpec.hpp"
#include "Lumen/Internal/Renderer/CommandBuffer.hpp"

#include "Lumen/Internal/API/Vulkan/VulkanRenderpass.hpp"

#include "Lumen/Maths/Structs.hpp"

namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Selection
    ////////////////////////////////////////////////////////////////////////////////////
    template<Info::RenderingAPI API>
    struct RenderpassSelect;

    template<> struct RenderpassSelect<Info::RenderingAPI::Vulkan> { using Type = VulkanRenderpass; };

    using RenderpassType = typename RenderpassSelect<Info::g_RenderingAPI>::Type;

    ////////////////////////////////////////////////////////////////////////////////////
    // Renderpass
    ////////////////////////////////////////////////////////////////////////////////////
    class Renderpass
    {
    public:
        // Constructors & Destructor
        inline Renderpass() = default;
        inline Renderpass(const RendererID renderer, const RenderpassSpecification& specs, CommandBuffer* cmdBuf) { Init(renderer, specs, cmdBuf); }
        inline ~Renderpass() = default;

        // Init & Destroy
		inline void Init(const RendererID renderer, const RenderpassSpecification& specs, CommandBuffer* cmdBuf) { m_Renderpass.Init(renderer, specs, cmdBuf); }
		inline void Destroy(const RendererID renderer) { m_Renderpass.Destroy(renderer); }

        // The Begin, End & Submit function are in the Renderer

        // Methods
        inline void Resize(const RendererID renderer, uint32_t width, uint32_t height) { return m_Renderpass.Resize(renderer, width, height); }

        // Getters
        inline Vec2<uint32_t> GetSize() const { return m_Renderpass.GetSize(); }

        inline const RenderpassSpecification& GetSpecification() const { return m_Renderpass.GetSpecification(); }
		inline CommandBuffer& GetCommandBuffer() { return m_Renderpass.GetCommandBuffer(); }

        // Internal
        inline RenderpassType& GetInternalRenderpass() { return m_Renderpass; }

    private:
		RenderpassType m_Renderpass = {};
    };

}