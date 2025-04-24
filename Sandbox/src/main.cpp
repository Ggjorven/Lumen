#include "Main.hpp"

#if !defined(LU_CONFIG_DIST)
int main(int argc, char* argv[])
{
	return Main(argc, argv);
}
#elif defined(LU_PLATFORM_WINDOWS) // WindowedApp on windows
#include <Windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
	return Main(__argc, __argv);
}
#else
int main(int argc, char* argv[])
{
	return Main(argc, argv);
}
#endif