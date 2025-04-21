#pragma once

#include <cstring>

#include "Lumen/Internal/Utils/Profiler.hpp"

namespace Lumen::Internal
{

	////////////////////////////////////////////////////////////////////////////////////
	// Memory
	////////////////////////////////////////////////////////////////////////////////////
	class Memory
	{
	public:
		// General functions
		inline static void Copy(void* dst, const void* src, size_t size)
		{
			LU_PROFILE("Memory::Copy()");
			std::memcpy(dst, src, size);
		}
	};

}