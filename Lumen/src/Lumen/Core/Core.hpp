#pragma once

#include "Lumen/Internal/Utils/Preprocessor.hpp"

namespace Lumen
{

	// forcenoinline, forceinline and hintline macros
    #if defined(LU_COMPILER_MSVC)
        //#define forcenoinline   __declspec(noinline)
        //#define forceinline     __forceinline
        //#define hintinline      inline
    #elif defined(LU_COMPILER_GCC) || defined(LU_COMPILER_CLANG) 
        //#define forcenoinline   __attribute__((noinline))
        //#define forceinline     inline __attribute__((always_inline))
        //#define hintinline      inline
    #else
        #error Lumen: Failed to define forcenoinline, forceinline & hintinline macros
    #endif

    // Note: I don't fully trust myself using these macros, so I use default compiler specifications for now
    #define forcenoinline   
    #define forceinline     inline
    #define hintinline      inline

}