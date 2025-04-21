#pragma once

#include "Lumen/Internal/Renderer/Renderer.hpp"

#include "Lumen/Renderer/RendererSpec.hpp"
#include "Lumen/Renderer/Texture.hpp"

#include <cstdint>
#include <vector>

namespace Lumen
{

	class Window;

	////////////////////////////////////////////////////////////////////////////////////
	// Renderer
	////////////////////////////////////////////////////////////////////////////////////
	class Renderer
	{
	public:
		// Main methods
		void BeginFrame();
		void EndFrame();

		// Getters
		inline RendererID GetID() const { return static_cast<RendererID>(m_Renderer->GetID()); }

	private:
		// Constructor & Destructor
		Renderer() = default;
		~Renderer() = default;

		// Init & Destroy
		void Init(const Internal::RendererID renderer);
		void Destroy();

		// Private methods
		void Resize(uint32_t width, uint32_t height);

		// Static methods
		static Renderer& GetRenderer(const RendererID renderer);

	private:
		Internal::Renderer* m_Renderer = nullptr;

		friend class Window;
		friend class Renderpass2D;
	};

}