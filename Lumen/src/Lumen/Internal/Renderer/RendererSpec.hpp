#pragma once

#include "Lumen/Internal/Renderer/ImageSpec.hpp"

#include "Lumen/Core/Core.hpp"

#include <cstdint>
#include <functional>

namespace Lumen::Internal
{

    class Window;

    ////////////////////////////////////////////////////////////////////////////////////
    // Queue
    ////////////////////////////////////////////////////////////////////////////////////
    enum class Queue : uint8_t
    {
        Graphics = 0, Compute, //Present,
        COUNT
    };

    ////////////////////////////////////////////////////////////////////////////////////
    // RendererSpecification
    ////////////////////////////////////////////////////////////////////////////////////
    struct RendererSpecification
    {
    public:
		inline static constexpr const uint8_t FramesInFlight = 3; // 1 - 3 is usual. 
    public:
        Window* WindowRef = nullptr;
        ImageFormat Format = ImageFormat::BGRA;

        uint32_t Width = 0, Height = 0;
        bool VSync = true;
    };

}