#pragma once

#include <cstring>

#include "Lumen/Internal/Utils/Profiler.hpp"

#include "Lumen/Core/Core.hpp"

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