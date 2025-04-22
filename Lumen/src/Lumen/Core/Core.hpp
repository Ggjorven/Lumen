#pragma once

#include "Lumen/Internal/Utils/Preprocessor.hpp"

namespace Lumen
{

	// noinline and forceinline macros
    #if defined(LU_COMPILER_MSVC)
        #define noinline __declspec(noinline)
        #define forceinline __forceinline
        #define hintinline inline
    #elif defined(LU_COMPILER_GCC) || defined(LU_COMPILER_CLANG) 
        #define noinline __attribute__((noinline))
        #define forceinline inline __attribute__((always_inline))
        #define hintinline inline
    #else
        #error Lumen: Failed to define noinline and forceinline macros
    #endif

}