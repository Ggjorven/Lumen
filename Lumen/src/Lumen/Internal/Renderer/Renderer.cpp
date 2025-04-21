#include "lupch.h"
#include "Renderer.hpp"

#include "Lumen/Internal/IO/Print.hpp"
#include "Lumen/Internal/Utils/Profiler.hpp"

#include <array>
#include <limits>

namespace Lumen::Internal
{

    namespace
    {
        static RendererID s_CurrentID = 0;
        static std::array<Renderer*, std::numeric_limits<RendererID>::max()> s_Renderers = { };
    }

    ////////////////////////////////////////////////////////////////////////////////////
	// Constructor & Destructor
    ////////////////////////////////////////////////////////////////////////////////////
	Renderer::Renderer()
		: m_RendererID(s_CurrentID++)
    {
		s_Renderers[m_RendererID] = this;
    }

    Renderer::~Renderer()
    {
        s_Renderers[m_RendererID] = nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Static methods
    ////////////////////////////////////////////////////////////////////////////////////
    Renderer& Renderer::GetRenderer(const RendererID renderer)
    {
        LU_PROFILE("Renderer::GetRenderer(Internal)");
        LU_ASSERT(s_Renderers[renderer], std::format("[Renderer] Requested renderer by ID: {0}, but this renderer is not initialized.", static_cast<size_t>(renderer)));
        return *s_Renderers[renderer];
    }

}