#pragma once

#include "Lumen/Internal/Utils/Preprocessor.hpp"

#include "Lumen/Core/Core.hpp"

#include <tracy/Tracy.hpp>

// Note: Internal header, may break in future versions.
#include <tracy/../client/TracyProfiler.hpp>

#include <cstdlib>
#include <string>
#include <new>

namespace Lumen::Internal
{

	////////////////////////////////////////////////////////////////////////////////////
	// Profiler
	////////////////////////////////////////////////////////////////////////////////////
	class Profiler // Note: The startup is asynchronous
	{
	public:
		// Utils
		hintinline static void Wait(uint32_t pollRateMs = 10, uint32_t initWaitMs = 400) // Note: Waits for profiler to attach and initialize
		{
			auto& profiler = tracy::GetProfiler();

			// Wait until the worker thread is running and ready
			while (!profiler.IsConnected())
				std::this_thread::sleep_for(std::chrono::milliseconds(pollRateMs));

			std::this_thread::sleep_for(std::chrono::milliseconds(initWaitMs));
		}
	};

	////////////////////////////////////////////////////////////////////////////////////
	// Macros
	////////////////////////////////////////////////////////////////////////////////////
	// Settings
	#define LU_ENABLE_PROFILING 0
	#define LU_MEM_PROFILING 0

	// Profiling macros
	#if !defined(LU_CONFIG_DIST) && LU_ENABLE_PROFILING
		#define LU_MARK_FRAME() FrameMark
		
		#define LU_PROFILE(name) ZoneScopedN(name)
		
		#define LU_PROFILER_WAIT_INIT() ::Lumen::Internal::Profiler::Wait()

		#if LU_MEM_PROFILING
			void* operator new(size_t size);
			void operator delete(void* ptr) noexcept;
			void operator delete(void* ptr, size_t size) noexcept;
		#endif
	#else
		#define LU_MARK_FRAME()

		#define LU_PROFILE(name)

		#define LU_PROFILER_WAIT_INIT()
	#endif

}