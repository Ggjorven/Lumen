#pragma once

#include "Lumen/Internal/Utils/Profiler.hpp"
#include "Lumen/Internal/Utils/Preprocessor.hpp"

#include "Lumen/Internal/Memory/DeferredConstruct.hpp"

#include "Lumen/Internal/Core/WindowSpec.hpp"
#include "Lumen/Internal/Renderer/Renderer.hpp"

#include "Lumen/Core/Core.hpp"

#include "Lumen/Maths/Structs.hpp"

#if defined(LU_PLATFORM_DESKTOP)
	#include <GLFW/glfw3.h>
#endif

namespace Lumen::Internal
{

	class Window;
	class DesktopWindow;

#if defined(LU_PLATFORM_DESKTOP)
	////////////////////////////////////////////////////////////////////////////////////
	// DesktopWindow
	////////////////////////////////////////////////////////////////////////////////////
	class DesktopWindow
	{
	public:
		// Constructors & Destructor
		DesktopWindow(const WindowSpecification& specs, Window* instance);
		~DesktopWindow();

		// Methods
		void PollEvents();
		void SwapBuffers();

		void Resize(uint32_t width, uint32_t height);
		inline void Close() { m_Closed = true; }

		// Getters
		inline Vec2<uint32_t> GetSize() const { return { m_Specification.Width, m_Specification.Height }; }
		Vec2<int32_t> GetPosition() const;

		// Setters
		void SetTitle(std::string_view title);
		void SetVSync(bool vsync);

		// Additional getters
		double GetTime() const;
		inline bool IsVSync() const { return m_Specification.VSync; }
		inline bool IsOpen() const { return !m_Closed; }
		inline bool IsMinimized() const { return ((m_Specification.Width == 0) || (m_Specification.Height == 0)); }

		inline void* GetNativeWindow() { return static_cast<void*>(m_Window); }
		inline WindowSpecification& GetSpecification() { return m_Specification; }
		inline Renderer& GetRenderer() { return m_Renderer.Get(); }

	private:
		WindowSpecification m_Specification;
		
		GLFWwindow* m_Window = nullptr;
		
		bool m_Closed = false;
		
		DeferredConstruct<Renderer, true> m_Renderer = {};
	};
#endif

}

#if defined(LU_PLATFORM_DESKTOP)
	#include "Lumen/Internal/Platform/Desktop/DesktopWindow.inl"
#endif