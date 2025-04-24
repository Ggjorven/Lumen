#pragma once

#include "Lumen/Core/Core.hpp"
#include "Lumen/Internal/Enum/Name.hpp"

#include <cstdint>
#include <type_traits>

namespace Lumen::Enum
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Bitwise
    ////////////////////////////////////////////////////////////////////////////////////
    template<typename TEnum> requires(std::is_enum_v<TEnum>)
    struct Customize
    {
    public:
		inline static constexpr bool Bitwise = false;
    };

}

////////////////////////////////////////////////////////////////////////////////////
// BitwiseEnum
////////////////////////////////////////////////////////////////////////////////////
template<typename TEnum>
concept BitwiseEnum = std::is_enum_v<TEnum> && Lumen::Enum::Customize<TEnum>::Bitwise;

////////////////////////////////////////////////////////////////////////////////////
// Global bitwise functions
////////////////////////////////////////////////////////////////////////////////////
template<BitwiseEnum TEnum>
constexpr auto operator & (TEnum lhs, TEnum rhs) noexcept
{
    using U = std::underlying_type_t<TEnum>;
    return static_cast<TEnum>((static_cast<U>(lhs) & static_cast<U>(rhs)));
}     

template<BitwiseEnum TEnum>
constexpr auto operator | (TEnum lhs, TEnum rhs) noexcept
{                                                                                                                   
    using U = std::underlying_type_t<TEnum>;
    return static_cast<TEnum>((static_cast<U>(lhs) | static_cast<U>(rhs)));
}   

template<BitwiseEnum TEnum>
constexpr auto operator ^ (TEnum lhs, TEnum rhs) noexcept
{                                                                                                                   
    using U = std::underlying_type_t<TEnum>;
    return static_cast<TEnum>((static_cast<U>(lhs) ^ static_cast<U>(rhs)));
}  

template<BitwiseEnum TEnum>
constexpr TEnum operator ~ (TEnum value) noexcept
{                                                                                                                   
    using U = std::underlying_type_t<TEnum>;
    return TEnum(~static_cast<U>(value));
}  

template<BitwiseEnum TEnum>
constexpr TEnum& operator &= (TEnum& lhs, TEnum rhs) noexcept
{                                                                                                                   
    return lhs = (lhs & rhs);                                                                                       
}  

template<BitwiseEnum TEnum>
constexpr TEnum& operator |= (TEnum& lhs, TEnum rhs) noexcept
{                                                                                                                   
    return lhs = (lhs | rhs);                                                                                       
}

template<BitwiseEnum TEnum>
constexpr TEnum& operator ^= (TEnum& lhs, TEnum rhs) noexcept
{                                                                                                                   
    return lhs = (lhs ^ rhs);                                                                                       
}