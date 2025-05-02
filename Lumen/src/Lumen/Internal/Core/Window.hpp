#pragma once

#include "Lumen/Internal/Utils/Settings.hpp"
#include "Lumen/Internal/Platform/Desktop/DesktopWindow.hpp"

#include "Lumen/Internal/Core/WindowSpec.hpp"

#include "Lumen/Core/Core.hpp"

#include "Lumen/Maths/Structs.hpp"

namespace Lumen::Internal
{

	////////////////////////////////////////////////////////////////////////////////////
	// Selection
	////////////////////////////////////////////////////////////////////////////////////
	template<Info::Platform Platform>
	struct WindowSelect;

	template<> struct WindowSelect<Info::Platform::Windows>		{ using Type = DesktopWindow; };
	template<> struct WindowSelect<Info::Platform::Linux>		{ using Type = DesktopWindow; };
	template<> struct WindowSelect<Info::Platform::MacOS>		{ using Type = DesktopWindow; };

	using WindowType = typename WindowSelect<Info::g_Platform>::Type;

	////////////////////////////////////////////////////////////////////////////////////
	// Window
	////////////////////////////////////////////////////////////////////////////////////
	class Window // Note: This class acts as an interface between different platform implementations
	{
	public:
		// Constructors & Destructor
		Window() = default;
		forceinline Window(const WindowSpecification& specs) 
			: m_Window(specs, this) {}
		~Window() = default;

		// Methods
		forceinline void PollEvents() { m_Window.PollEvents(); }
		forceinline void SwapBuffers() { m_Window.SwapBuffers(); }

		// Note: This is not resizing the window, it's resizing the drawing area (on the internal renderer).
		forceinline void Resize(uint32_t width, uint32_t height) { m_Window.Resize(width, height); }
		forceinline void Close() { m_Window.Close(); }

		// Getters
		forceinline double GetTime() const { return m_Window.GetTime(); }
		forceinline Vec2<uint32_t> GetSize() const { return m_Window.GetSize(); }
		forceinline Vec2<int32_t> GetPosition() const { return m_Window.GetPosition(); }

		// Setters
		forceinline void SetTitle(std::string_view title) { m_Window.SetTitle(title); }
		forceinline void SetVSync(bool vsync) { m_Window.SetVSync(vsync); }

		// Additional getters
		forceinline bool IsVSync() const { return m_Window.IsVSync(); }
		forceinline bool IsOpen() const { return m_Window.IsOpen(); }
		forceinline bool IsMinimized() const { return m_Window.IsMinimized(); }

		forceinline void* GetNativeWindow() { return m_Window.GetNativeWindow(); }
		forceinline WindowSpecification& GetSpecification() { return m_Window.GetSpecification(); }
		forceinline Renderer& GetRenderer() { return m_Window.GetRenderer(); }

	private:
		WindowType m_Window;
	};

}