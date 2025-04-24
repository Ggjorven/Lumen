#pragma once

#include "Lumen/Internal/IO/Print.hpp"
#include "Lumen/Internal/Utils/Settings.hpp"

#include "Lumen/Internal/Vulkan/VulkanContext.hpp"

#include "Lumen/Core/Core.hpp"
#include "Lumen/Utils/DeferredConstruct.hpp"

#include <cstdint>

namespace Lumen::Internal
{

	////////////////////////////////////////////////////////////////////////////////////
	// GraphicsContext
	////////////////////////////////////////////////////////////////////////////////////
	// Note: You only need one context for an entire application, even with multiple windows
	// So, only initialize it once. // Note: Currently we only support a single window.
	class GraphicsContext
	{
	public:
		using Type = VulkanContext;
	public:
		// Getters
		static bool Initialized();

		// Static methods
		static void AttachWindow(void* nativeWindow);

		static void Init();
		static void Destroy();

		// Internal
		inline static Type& GetInternalContext() { return s_GraphicsContext; }

	private:
		inline static void* s_ActiveWindow = nullptr;
		inline static bool s_Initialized = false;
		inline static GraphicsContext::Type s_GraphicsContext = {};
	};

}

#include "Lumen/Internal/Renderer/GraphicsContext.inl"