#pragma once

#include "Lumen/Core/Core.hpp"

#include "Lumen/Internal/Utils/Profiler.hpp"

#include <cstdint>
#include <array>
#include <chrono>
#include <random>
#include <numeric>
#include <algorithm>

namespace Lumen
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Random
    ////////////////////////////////////////////////////////////////////////////////////
    class Random
    {
    public:
        // Seeding
        static void Seed();
		static void Seed(uint32_t seed);

        // Floating point
        static float Float(float min = 0.0f, float max = 1.0f);
        static double Double(double min = 0.0, double max = 1.0);

        // Integer
        static int32_t Int(int32_t min = std::numeric_limits<int32_t>::min(), int32_t max = std::numeric_limits<int32_t>::max());
        static uint32_t UInt(uint32_t min = std::numeric_limits<uint32_t>::min(), uint32_t max = std::numeric_limits<uint32_t>::max());

        // Boolean
        static bool Bool(float trueProbability = 0.5f);

        // Utils
        static bool Chance(float percentage);

    private:
        static thread_local std::mt19937 s_RandomEngine;
        static thread_local std::uniform_real_distribution<float> s_FloatDist;
        static thread_local std::uniform_real_distribution<double> s_DoubleDist;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    // FastRandom
    ////////////////////////////////////////////////////////////////////////////////////
    class FastRandom
    {
    public:
        // Seeding
		static void Seed();
		static void Seed(uint64_t seed);

        // Floating point
        static float Float(float min = 0.0f, float max = 1.0f);
        static double Double(double min = 0.0, double max = 1.0);

        // Integer
        static int32_t Int(int32_t min = std::numeric_limits<int32_t>::min(), int32_t max = std::numeric_limits<int32_t>::max());
        static uint32_t UInt(uint32_t min = std::numeric_limits<uint32_t>::min(), uint32_t max = std::numeric_limits<uint32_t>::max());

        // Boolean
        static bool Bool(float trueProbability = 0.5f);

        // Utils
        static bool Chance(float percentage);

    private:
        // Private methods
        static uint64_t Next();
        static uint64_t NextXORShift64();   // Very fast, low quality
        static uint64_t NextSplitMix64();   // Great for seeding
        static uint64_t NextPCG32();        // Great balance, highly recommended

    private:
		static thread_local uint64_t s_State;
    };

	
	////////////////////////////////////////////////////////////////////////////////////
	// INLINE IMPLEMENTATIONS
	////////////////////////////////////////////////////////////////////////////////////
	thread_local std::mt19937								Random::s_RandomEngine(static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
	thread_local std::uniform_real_distribution<float>		Random::s_FloatDist(0.0f, 1.0f);
	thread_local std::uniform_real_distribution<double>		Random::s_DoubleDist(0.0, 1.0);

	thread_local uint64_t									FastRandom::s_State = static_cast<uint32_t>(std::chrono::steady_clock::now().time_since_epoch().count());

	////////////////////////////////////////////////////////////////////////////////////
	// Seeding
	////////////////////////////////////////////////////////////////////////////////////
	forceinline void Random::Seed()
	{
		Seed(static_cast<uint32_t>(std::chrono::steady_clock::now().time_since_epoch().count()));
	}

	forceinline void Random::Seed(uint32_t seed)
	{
		s_RandomEngine.seed(seed);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Floating point
	////////////////////////////////////////////////////////////////////////////////////
	forceinline float Random::Float(float min, float max)
	{
		LU_PROFILE("Random::Float()");
		return min + s_FloatDist(s_RandomEngine) * (max - min);
	}

	forceinline double Random::Double(double min, double max)
	{
		LU_PROFILE("Random::Double()");
		return min + s_DoubleDist(s_RandomEngine) * (max - min);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Integer
	////////////////////////////////////////////////////////////////////////////////////
	forceinline int32_t Random::Int(int32_t min, int32_t max)
	{
		LU_PROFILE("Random::Int()");
		return min + static_cast<int32_t>(s_FloatDist(s_RandomEngine) * (max - min + 1));
	}

	forceinline uint32_t Random::UInt(uint32_t min, uint32_t max)
	{
		LU_PROFILE("Random::UInt()");
		return min + static_cast<uint32_t>(s_FloatDist(s_RandomEngine) * (max - min + 1));
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Boolean
	////////////////////////////////////////////////////////////////////////////////////
	forceinline bool Random::Bool(float trueProbability)
	{
		LU_PROFILE("Random::Bool()");
		return s_FloatDist(s_RandomEngine) < trueProbability;
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Utils
	////////////////////////////////////////////////////////////////////////////////////
	forceinline bool Random::Chance(float percentage)
	{
		LU_PROFILE("Random::Chance()");
		return Float(0.0f, 100.0f) < percentage;
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Seeding
	////////////////////////////////////////////////////////////////////////////////////
	forceinline void FastRandom::Seed()
	{
		Seed(static_cast<uint32_t>(std::chrono::steady_clock::now().time_since_epoch().count()));
	}

	forceinline void FastRandom::Seed(uint64_t seed)
	{
		s_State = seed;
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Floating point
	////////////////////////////////////////////////////////////////////////////////////
	forceinline float FastRandom::Float(float min, float max)
	{
		LU_PROFILE("FastRandom::Float()");
		return min + (Next() / static_cast<float>(std::numeric_limits<uint32_t>::max())) * (max - min);
	}

	forceinline double FastRandom::Double(double min, double max)
	{
		LU_PROFILE("FastRandom::Double()");
		return min + (Next() / static_cast<double>(std::numeric_limits<uint32_t>::max())) * (max - min);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Integer
	////////////////////////////////////////////////////////////////////////////////////
	forceinline int32_t FastRandom::Int(int32_t min, int32_t max)
	{
		LU_PROFILE("FastRandom::Int()");
		return min + static_cast<int32_t>(Next() % (static_cast<uint32_t>(max - min + 1)));
	}

	forceinline uint32_t FastRandom::UInt(uint32_t min, uint32_t max)
	{
		LU_PROFILE("FastRandom::UInt()");
		return min + (Next() % (max - min + 1));
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Boolean
	////////////////////////////////////////////////////////////////////////////////////
	forceinline bool FastRandom::Bool(float trueProbability)
	{
		LU_PROFILE("FastRandom::Bool()");
		return Float(0.0f, 1.0f) < trueProbability;
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Utils
	////////////////////////////////////////////////////////////////////////////////////
	forceinline bool FastRandom::Chance(float percentage)
	{
		LU_PROFILE("FastRandom::Chance()");
		return Float(0.0f, 100.0f) < percentage;
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Private methods
	////////////////////////////////////////////////////////////////////////////////////
	forceinline uint64_t FastRandom::Next() // Note: Can be changed to a different FastRandom method
	{
		return NextXORShift64();
	}

	forceinline uint64_t FastRandom::NextXORShift64()
	{
		s_State ^= s_State << 13;
		s_State ^= s_State >> 7;
		s_State ^= s_State << 17;
		return s_State;
	}

	forceinline uint64_t FastRandom::NextSplitMix64()
	{
		s_State += 0x9E3779B97F4A7C15ULL;
		uint64_t z = s_State;
		z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
		z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
		return (z ^ (z >> 31));
	}

	forceinline uint64_t FastRandom::NextPCG32()
	{
		static constexpr uint64_t multiplier = 6364136223846793005ULL;
		static constexpr uint64_t increment = 1442695040888963407ULL;

		uint64_t oldstate = s_State;
		s_State = oldstate * multiplier + increment;
		uint32_t xorshifted = static_cast<uint32_t>(((oldstate >> 18u) ^ oldstate) >> 27u);
		uint32_t rot = static_cast<uint32_t>(oldstate >> 59u);
		return (xorshifted >> rot) | (xorshifted << ((rot) & 31));
	}

}
