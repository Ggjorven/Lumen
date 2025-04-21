#pragma once

#include "Lumen/Internal/Core/Window.hpp"

#include "Lumen/Internal/Maths/Structs.hpp"

#include "Lumen/Core/Events.hpp"

#include "Lumen/Renderer/Renderer.hpp"

#include <cstdint>
#include <functional>
#include <string_view>

namespace Lumen
{

	////////////////////////////////////////////////////////////////////////////////////
	// WindowSpecification
	////////////////////////////////////////////////////////////////////////////////////
	using EventCallbackFn = std::function<void(Event& event)>;
	
	struct WindowSpecification
	{
	public:
		std::string_view Title = {};
		uint32_t Width = 0, Height = 0;

		bool VSync = false;

		EventCallbackFn EventCallback = nullptr;
	};

	////////////////////////////////////////////////////////////////////////////////////
	// Window
	////////////////////////////////////////////////////////////////////////////////////
	class Window
	{
	public:
		// Constructor & Destructor
		Window() = default;
		Window(const WindowSpecification& specs);
		~Window();

		// Init
		void Init(const WindowSpecification& specs);

		// Methods
		void PollEvents();
		void SwapBuffers();

		// Setters
		inline void SetEventCallback(const EventCallbackFn& callback) { m_EventCallback = callback; }

		// Getters
		inline double GetTime() const { return m_Window.GetTime(); }
		inline bool IsOpen() const { return m_Window.IsOpen(); }
		inline Vec2<uint32_t> GetSize() const { return m_Window.GetSize(); }

		inline Renderer& GetRenderer() { return m_Renderer; }

	private:
		// Private methods
		void OnEvent(Internal::Event& e);

	private:
		Internal::Window m_Window = {};
		EventCallbackFn m_EventCallback = {};

		Renderer m_Renderer = {};
	};

}