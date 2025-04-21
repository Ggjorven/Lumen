#include "lupch.h"
#include "Renderer.hpp"

#include "Lumen/Internal/IO/Print.hpp"
#include "Lumen/Internal/Utils/Profiler.hpp"

#include <unordered_map>

namespace Lumen
{

	namespace
	{
		static std::array<Renderer*, std::numeric_limits<RendererID>::max()> s_Renderers = { };
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Main methods
	////////////////////////////////////////////////////////////////////////////////////
	void Renderer::BeginFrame()
	{
		m_Renderer->BeginFrame();
	}

	void Renderer::EndFrame()
	{
		m_Renderer->EndFrame();
		m_Renderer->Present();
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Constructor & Destructor
	////////////////////////////////////////////////////////////////////////////////////
	void Renderer::Init(const Internal::RendererID renderer)
	{
		s_Renderers[renderer] = this;
		m_Renderer = &Internal::Renderer::GetRenderer(renderer);
	}

	void Renderer::Destroy()
	{
		s_Renderers[m_Renderer->GetID()] = nullptr;
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Private methods
	////////////////////////////////////////////////////////////////////////////////////
	void Renderer::Resize(uint32_t width, uint32_t height)
	{
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Static methods
	////////////////////////////////////////////////////////////////////////////////////
	Renderer& Renderer::GetRenderer(const RendererID renderer)
	{
		LU_PROFILE("Renderer::GetRenderer()");
		LU_ASSERT(s_Renderers[renderer], std::format("[Renderer] Requested renderer by ID: {0}, but this renderer is not initialized.", static_cast<size_t>(renderer)));
		return *s_Renderers[renderer];
	}
	
}