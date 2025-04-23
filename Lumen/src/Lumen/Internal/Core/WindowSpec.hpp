#pragma once

#include <cstdint>
#include <functional>
#include <string_view>

#include "Lumen/Core/Events.hpp"

#include "Lumen/Internal/IO/Print.hpp"

namespace Lumen::Internal
{

    using EventCallbackFn = std::function<void(Event e)>;

    ////////////////////////////////////////////////////////////////////////////////////
    // WindowSpecification
    ////////////////////////////////////////////////////////////////////////////////////
    struct WindowSpecification
    {
    public:
        enum class BufferMode : uint8_t { Single = 1, Double = 2, Triple = 3 };
		inline static constexpr BufferMode Buffers = BufferMode::Triple;
    public:
        // Window
        std::string_view Title = {};
        uint32_t Width = 0, Height = 0;

        EventCallbackFn EventCallback = nullptr;

        // Renderer
        bool VSync = false;
    };

}