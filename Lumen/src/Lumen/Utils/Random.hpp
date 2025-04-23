#pragma once

#include "Lumen/Internal/Utils/Profiler.hpp"

#include "Lumen/Core/Core.hpp"

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

}

#include "Lumen/Utils/Random.inl"