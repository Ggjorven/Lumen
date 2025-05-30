#pragma once

#include <cstdint>

#include "Lumen/Internal/Utils/Preprocessor.hpp"

namespace Lumen::Internal::Info
{

	////////////////////////////////////////////////////////////////////////////////////
	// Structs
	////////////////////////////////////////////////////////////////////////////////////
	enum class Compiler : uint8_t
	{
		MSVC = 0, 
		GCC, 
		Clang
	};

	enum class Platform : uint8_t
	{
		Windows = 0,
		Linux,
		MacOS,

		// Android,
		// iOS,
	};

	enum class Configuration : uint8_t
	{
		Debug = 0,
		Release,
		Dist
	};

	enum class CppStd : uint8_t
	{
		Cpp11 = LU_CPPSTD_11,
		Cpp14 = LU_CPPSTD_14,
		Cpp17 = LU_CPPSTD_17,
		Cpp20 = LU_CPPSTD_20,
		Cpp23 = LU_CPPSTD_23,
		Cpp26 = LU_CPPSTD_26,
	};

	////////////////////////////////////////////////////////////////////////////////////
	// Values
	////////////////////////////////////////////////////////////////////////////////////
	#if defined(LU_COMPILER_MSVC)
		inline constexpr const Compiler g_Compiler = Compiler::MSVC;
	#elif defined(LU_COMPILER_GCC)
		inline constexpr const Compiler g_Compiler = Compiler::GCC;
	#elif defined(LU_COMPILER_CLANG)
		inline constexpr const Compiler g_Compiler = Compiler::Clang;
	#else
		#error Lumen Settings: Unsupported compiler.
	#endif

	#if defined(LU_PLATFORM_WINDOWS)
		inline constexpr const Platform g_Platform = Platform::Windows;
	#elif defined(LU_PLATFORM_LINUX) 
		inline constexpr const Platform g_Platform = Platform::Linux;
	#elif defined(LU_PLATFORM_MACOS) 
		inline constexpr const Platform g_Platform = Platform::MacOS;
	#else 
		#error Lumen Settings: Unsupported platform.
	#endif

	#if defined(LU_CONFIG_DEBUG)
		inline constexpr const Configuration g_Configuration = Configuration::Debug;
	#elif defined(LU_CONFIG_RELEASE)
		inline constexpr const Configuration g_Configuration = Configuration::Release;
	#elif defined(LU_CONFIG_DIST)
		inline constexpr const Configuration g_Configuration = Configuration::Dist;
	#else
		#error Lumen Settings: Unsupported configuration.
	#endif

	inline constexpr const CppStd g_CppStd = static_cast<CppStd>(LU_CPPSTD);

}