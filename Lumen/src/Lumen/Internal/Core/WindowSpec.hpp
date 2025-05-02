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
        // Window
        std::string_view Title = {};
        uint32_t Width = 0, Height = 0;

        EventCallbackFn EventCallback = nullptr;

        // Renderer
        bool VSync = false;
    };

}