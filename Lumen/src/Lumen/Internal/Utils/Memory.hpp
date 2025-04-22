#pragma once

#include <cstring>

#include "Lumen/Core/Core.hpp"
#include "Lumen/Internal/Utils/Profiler.hpp"

namespace Lumen::Internal::Memory
{

	////////////////////////////////////////////////////////////////////////////////////
	// Memory
	////////////////////////////////////////////////////////////////////////////////////
	// General functions
	forceinline void Copy(void* dst, const void* src, size_t size)
	{
		LU_PROFILE("Memory::Copy()");
		std::memcpy(dst, src, size);
	}

}