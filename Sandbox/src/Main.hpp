#include <iostream>

#include "Lumen/Internal/Core/Window.hpp"

#include "Lumen/Internal/IO/Print.hpp"
#include "Lumen/Internal/Utils/Settings.hpp"

#include "Lumen/Utils/DeferredConstruct.hpp"

using namespace Lumen;

int Main(int argc, char* argv[])
{
	DeferredConstruct<Internal::Window> window;
	window.Construct(Internal::WindowSpecification({
		.Title = "Lumen", 

		.Width = 1280,
		.Height = 720,

		.EventCallback = [&](Event event) -> void 
		{ 
			EventHandler handler(event);
			handler.Handle<WindowCloseEvent>([&](WindowCloseEvent&) -> void { window->Close(); });
			handler.Handle<WindowResizeEvent>([&](WindowResizeEvent& e) -> void { window->Resize(e.GetWidth(), e.GetHeight()); });
		},

		.VSync = true,
	}));


	while (window->IsOpen())
	{
		window->PollEvents();

		window->SwapBuffers();
	}

	return 0;
}