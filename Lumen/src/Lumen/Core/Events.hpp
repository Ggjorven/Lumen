#pragma once

#include "Lumen/Core/Core.hpp"
#include "Lumen/Core/Input/KeyCodes.hpp"
#include "Lumen/Core/Input/MouseCodes.hpp"

#include "Lumen/Internal/Utils/Types.hpp"
#include "Lumen/Internal/Enum/Bitwise.hpp"

#include <cstdint>
#include <string>
#include <sstream>
#include <variant>
#include <concepts>
#include <functional>
#include <type_traits>

namespace Lumen
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Windows events
    ////////////////////////////////////////////////////////////////////////////////////
    class WindowResizeEvent
    {
    public:
        // Constructors & Destructor
        forceinline WindowResizeEvent(uint32_t width, uint32_t height)
            : m_Width(width), m_Height(height) {}
        ~WindowResizeEvent() = default;

        // Getters
        forceinline uint32_t GetWidth() const { return m_Width; }
        forceinline uint32_t GetHeight() const { return m_Height; }

    private:
        uint32_t m_Width, m_Height;
    };

    class WindowCloseEvent
    {
    public:
        // Constructors & Destructor
        WindowCloseEvent() = default;
        ~WindowCloseEvent() = default;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    // Key events
    ////////////////////////////////////////////////////////////////////////////////////
    class KeyPressedEvent
    {
    public:
        // Constructors & Destructor
        forceinline KeyPressedEvent(int keycode, int repeatCount)
            : m_KeyCode(static_cast<Key>(keycode)), m_RepeatCount(repeatCount) {}
        ~KeyPressedEvent() = default;

        // Getters
        forceinline Key GetKeyCode() const { return m_KeyCode; }
        forceinline int GetRepeatCount() const { return m_RepeatCount; }

    private:
        Key m_KeyCode;
        int m_RepeatCount;
    };

    class KeyReleasedEvent
    {
    public:
        // Constructors & Destructor
        forceinline KeyReleasedEvent(int keycode)
            : m_KeyCode(static_cast<Key>(keycode)) {}
        ~KeyReleasedEvent() = default;

        // Getters
        forceinline Key GetKeyCode() const { return m_KeyCode; }

    private:
        Key m_KeyCode;
    };

    class KeyTypedEvent
    {
    public:
        // Constructors & Destructor
        forceinline KeyTypedEvent(int keycode)
            : m_KeyCode(static_cast<Key>(keycode)) {}
        ~KeyTypedEvent() = default;

        // Getters
        forceinline Key GetKeyCode() const { return m_KeyCode; }

    private:
        Key m_KeyCode;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    // Mouse events
    ////////////////////////////////////////////////////////////////////////////////////
    class MouseMovedEvent
    {
    public:
        // Constructors & Destructor
        forceinline MouseMovedEvent(float x, float y)
            : m_MouseX(x), m_MouseY(y) {}
        ~MouseMovedEvent() = default;

        // Getters
        forceinline float GetX() const { return m_MouseX; }
        forceinline float GetY() const { return m_MouseY; }

    private:
        float m_MouseX, m_MouseY;
    };

    class MouseScrolledEvent
    {
    public:
        // Constructors & Destructor
        forceinline MouseScrolledEvent(float xOffset, float yOffset)
            : m_XOffset(xOffset), m_YOffset(yOffset) {}
        ~MouseScrolledEvent() = default;

        // Getters
        forceinline float GetXOffset() const { return m_XOffset; }
        forceinline float GetYOffset() const { return m_YOffset; }

    private:
        float m_XOffset, m_YOffset;
    };

    class MouseButtonPressedEvent
    {
    public:
        // Constructors & Destructor
        forceinline MouseButtonPressedEvent(int button)
            : m_Button(static_cast<MouseButton>(button)) {}
        ~MouseButtonPressedEvent() = default;

        // Getters
        forceinline MouseButton GetButtonCode() const { return m_Button; }

    private:
        MouseButton m_Button;
    };

    class MouseButtonReleasedEvent
    {
    public:
        // Constructors & Destructor
        forceinline MouseButtonReleasedEvent(int button)
            : m_Button(static_cast<MouseButton>(button)) {}
        ~MouseButtonReleasedEvent() = default;

        // Getters
        forceinline MouseButton GetButtonCode() const { return m_Button; }

    private:
        MouseButton m_Button;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    // Event
    ////////////////////////////////////////////////////////////////////////////////////
    using Event = std::variant<
        WindowResizeEvent, WindowCloseEvent,
        KeyPressedEvent, KeyReleasedEvent, KeyTypedEvent,
        MouseMovedEvent, MouseScrolledEvent, MouseButtonPressedEvent, MouseButtonReleasedEvent
    >;

    class EventHandler
    {
    public:
        // Constructors & Destructor
        forceinline EventHandler(Event& e)
            : m_Event(e) {}
        ~EventHandler() = default;

        // Methods
        template <typename TEvent, typename F>
        hintinline void Handle(const F&& func) requires (Internal::Types::TypeInVariant<TEvent, Event> && std::invocable<F, TEvent&>)
        {
            std::visit(
                [&](auto&& obj) 
                {
                    if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, TEvent>) 
                    {
                        func(obj);
                    }
                },
            m_Event);
        }

    private:
        Event& m_Event;
    };

}
