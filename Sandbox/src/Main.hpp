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

	double lastTime = window->GetTime();
	double deltaTime = 0.0f;
	double timer = 0.0f;
	uint32_t frameCount = 0;

	while (window->IsOpen())
	{
		window->PollEvents();

		window->SwapBuffers();

		deltaTime = window->GetTime() - lastTime;
		lastTime = window->GetTime();
		timer += deltaTime;
		frameCount++;

		if (timer >= 1.0f)
		{
			LU_LOG_INFO("FPS: {0}", frameCount);
			window->SetTitle(std::format("Lumen - FPS: {0}", frameCount));
			timer = 0.0f;
			frameCount = 0;
		}
	}

	return 0;
}