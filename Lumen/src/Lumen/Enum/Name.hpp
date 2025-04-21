#pragma once

#include <cstdint>
#include <utility>

#include "Lumen/Internal/Enum/Name.hpp"

namespace Lumen::Enum
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Public functions
    ////////////////////////////////////////////////////////////////////////////////////
    template<typename TEnum> requires(std::is_enum_v<TEnum>)
    constexpr std::string_view Name(const TEnum value)
    {
        constexpr const auto entries = Lumen::Internal::Enum::Entries<TEnum>;
        
        for (const auto& [val, name] : entries)
        {
            if (val == value)
                return name;
        }

        return "";
    }

}