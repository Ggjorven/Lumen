#pragma once

#include "Lumen/Core/Core.hpp"

#include <cstdint>
#include <vector>
#include <array>
#include <memory_resource>

namespace Lumen::Internal
{

	// Note: This file exists, because I wanted to be able to track size of Array, but didn't end up using it.

    ////////////////////////////////////////////////////////////////////////////////////
    // Array
    ////////////////////////////////////////////////////////////////////////////////////
	template<typename T, size_t N>
	using Array = std::array<T, N>;

}