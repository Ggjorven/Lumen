#include <iostream>

#include "Lumen/Internal/Core/Window.hpp"

#include "Lumen/Internal/IO/Print.hpp"
#include "Lumen/Internal/Utils/Settings.hpp"

#include "Lumen/Utils/DeferredConstruct.hpp"

using namespace Lumen;

struct MyStruct
{
public:
	int a;
	float b;
	double c;

public:
	MyStruct(int a, float b, double c)
		: a(a), b(b), c(c)
	{
		LU_LOG_INFO("MyStruct: a = {}, b = {}, c = {}", a, b, c);
	}
};

int Main(int argc, char* argv[])
{
	LU_LOG_TRACE("Trace message");
	LU_LOG_INFO("Info message");
	LU_LOG_WARN("Warn message");
	LU_LOG_ERROR("Error message");
	LU_LOG_FATAL("Fatal message");

	DeferredConstruct<MyStruct> myStruct;
	myStruct.Construct(1, 2.0f, 3.0);

	DeferredConstruct<Internal::Window> window;
	window.Construct(Internal::WindowSpecification({
		.Title = "Lumen", 

		.Width = 1280,
		.Height = 720,

		.VSync = true,
	}));

	return 0;
}