#pragma once

#include <cstdint>
#include <cstddef>
#include <set>
#include <map>
#include <array>
#include <ranges>
#include <utility>
#include <numeric>
#include <optional>
#include <algorithm>
#include <string_view>
#include <type_traits>

#include "Lumen/Internal/Utils/Preprocessor.hpp"

namespace Lumen::Enum
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Customizations
    ////////////////////////////////////////////////////////////////////////////////////
    template<typename TEnum> requires(std::is_enum_v<TEnum>)
    struct Range
    {
    public:
        inline static constexpr int32_t Min = ((std::is_signed_v<std::underlying_type_t<TEnum>>) ? -128 : 0);
        inline static constexpr int32_t Max = ((std::is_signed_v<std::underlying_type_t<TEnum>>) ? 128 : 255);
        
        static_assert((Max - Min <= std::numeric_limits<uint16_t>::max()), "[Max - Min] must not exceed uint16 max value.");
    };

}

namespace Lumen::Internal::Enum
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Global values
    ////////////////////////////////////////////////////////////////////////////////////
    constexpr const std::string_view g_InvalidName = "1nvalid";
    
    ////////////////////////////////////////////////////////////////////////////////////
    // Internal naming
    ////////////////////////////////////////////////////////////////////////////////////
    #if defined(LU_COMPILER_MSVC)

    template<typename TEnum, TEnum EValue> requires(std::is_enum_v<TEnum>)
    class ConstexprName
    {
    public:
        constexpr static const std::string_view ClassToken = "Lumen::Internal::Enum::ConstexprName<";

    public:
        template <TEnum> requires(std::is_enum_v<TEnum>)
        constexpr static std::string_view FullNameImpl()
        {
            constexpr std::string_view FunctionToken = ">::FullNameImpl<";

            // Function signature
            constexpr std::string_view function = __FUNCSIG__;

            // Class token
            constexpr auto startClassToken = function.find(ClassToken);
            constexpr auto endClassToken = startClassToken + ClassToken.size();
            if constexpr (startClassToken == std::string_view::npos)
                return g_InvalidName;

            // Function token
            constexpr auto startFunctionToken = function.find(FunctionToken, endClassToken);
            constexpr auto endFunctionToken = startFunctionToken + FunctionToken.size();
            if constexpr (startFunctionToken == std::string_view::npos)
                return g_InvalidName;

            // Close marker
            constexpr auto closeMarker = function.find('>', endFunctionToken);
            if constexpr (closeMarker == std::string_view::npos)
                return g_InvalidName;

            // Full name
            constexpr std::string_view fullName = function.substr(endFunctionToken, closeMarker - endFunctionToken);

            // Brackets (if brackets, then not a valid enum name, ex. (enum Dummy)0xa instead of Dummy::First)
            if constexpr (fullName.find('(') != std::string_view::npos)
                return g_InvalidName;

            return fullName;
        }

        constexpr static std::string_view ElementNameImpl()
        {
            constexpr std::string_view fullName = FullNameImpl<EValue>();

            // Check for invalid name
            if constexpr (!fullName.compare(g_InvalidName))
                return g_InvalidName;

            // '::' marker
            constexpr auto startColon = fullName.find("::");
            constexpr auto endColon = startColon + std::string_view("::").size();
            if constexpr (startColon == std::string_view::npos) // If not an enum class but a regular enum
                return fullName;

            // Element name
            constexpr std::string_view elementName = fullName.substr(endColon, fullName.size() - endColon);

            return elementName;
        }

        template<TEnum>
        constexpr static std::string_view FunctionSignatureImpl()
        {
            return { __FUNCSIG__, sizeof(__FUNCSIG__) };
        }

    public:
        constexpr static const std::string_view FullName = FullNameImpl<EValue>();
        constexpr static const std::string_view ElementName = ElementNameImpl();

        constexpr static const std::string_view FunctionSignature = FunctionSignatureImpl<EValue>();
    };

    #elif defined(LU_COMPILER_GCC) || defined(LU_COMPILER_CLANG)
    
    template <typename TEnum, TEnum EValue> requires(std::is_enum_v<TEnum>)
    class ConstexprName
    {
        constexpr static std::string_view FullNameImpl()
        {
            #if defined(LU_COMPILER_CLANG)
            constexpr auto end = std::string_view(__PRETTY_FUNCTION__).find_last_of(']');
            if constexpr (end == std::string_view::npos)
                return g_InvalidName;

            // '=' marker
            constexpr auto start = std::string_view(__PRETTY_FUNCTION__).rfind("EValue = ") + 7;
            if constexpr (start == std::string_view::npos)
                return g_InvalidName;

            #else

            // Ending marker
            constexpr auto end = std::string_view(__PRETTY_FUNCTION__).find_last_of(';');
            if constexpr (end == std::string_view::npos)
                return g_InvalidName;

            // '=' marker
            constexpr auto start = std::string_view(__PRETTY_FUNCTION__).find_last_of('=', end);
            if constexpr (start == std::string_view::npos)
                return g_InvalidName;

            #endif

            // 0 <= start < end
            if constexpr (end - start <= 2)
                return g_InvalidName;

            return std::string_view(__PRETTY_FUNCTION__).substr(start + 2, end - start - 2);
        }

        constexpr static std::string_view ElementNameImpl()
        {
            constexpr std::string_view fullName = FullNameImpl();

            // Check for invalid name
            if constexpr (!fullName.compare(g_InvalidName))
                return g_InvalidName;

            // '::' marker
            constexpr auto startColon = fullName.find("::");
            constexpr auto endColon = startColon + std::string_view("::").size();
            if constexpr (startColon == std::string_view::npos) // If not an enum class but a regular enum
                return fullName;

            // Element name
            constexpr std::string_view elementName = fullName.substr(endColon, fullName.size() - endColon);

            return elementName;
        }

        constexpr static std::string_view FunctionSignatureImpl() 
        { 
            return { __PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) }; 
        }

    public:
        constexpr static const std::string_view FullName = FullNameImpl();
        constexpr static const std::string_view ElementName = ElementNameImpl();

        constexpr static const std::string_view FunctionSignature = FunctionSignatureImpl();
    };

    #else
        #error Lumen Enum: Unsupported compiler...
    #endif

    ////////////////////////////////////////////////////////////////////////////////////
    // Helper functions
    ////////////////////////////////////////////////////////////////////////////////////
    template<typename TEnum, TEnum EValue> requires(std::is_enum_v<TEnum>)
    constexpr std::string_view Name()
    {
        if constexpr (ConstexprName<TEnum, EValue>::ElementName == g_InvalidName)
            return {};

        return ConstexprName<TEnum, EValue>::ElementName;
    }

    template<typename TEnum, TEnum EValue> requires(std::is_enum_v<TEnum>)
    constexpr bool IsValid()
    {
        constexpr TEnum value = static_cast<TEnum>(EValue);
        return !Name<TEnum, value>().empty();
    }

    template<typename TEnum> requires(std::is_enum_v<TEnum>)
    constexpr TEnum UAlue(size_t v)
    {
        return static_cast<TEnum>(Lumen::Enum::Range<TEnum>::Min + v);
    }

    template<size_t N>
    constexpr size_t CountValues(const bool (&valid)[N])
    {
        size_t count = 0;
        for (size_t n = 0; n < N; n++) 
        {
            if (valid[n])
                ++count;
        }

        return count;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Values
    ////////////////////////////////////////////////////////////////////////////////////
    template<typename TEnum, size_t... I> requires(std::is_enum_v<TEnum>)
    constexpr auto ValuesImpl(std::index_sequence<I...>)
    {
        constexpr bool valid[sizeof...(I)] = { IsValid<TEnum, UAlue<TEnum>(I)>()... };
        constexpr auto validCount = CountValues(valid);
        static_assert(validCount > 0, "No support for empty enums."); 

        std::array<TEnum, validCount> values = {};
        for (size_t offset = 0, n = 0; n < validCount; offset++) 
        {
            if (valid[offset]) 
            {
                values[n] = UAlue<TEnum>(offset);
                ++n;
            }
        }

        return values;
    }

    template<typename TEnum> requires(std::is_enum_v<TEnum>)
    constexpr auto ValuesImpl()
    {
        constexpr auto enumSize = Lumen::Enum::Range<TEnum>::Max - Lumen::Enum::Range<TEnum>::Min + 1;
        return ValuesImpl<TEnum>(std::make_index_sequence<enumSize>({}));
    }

    template<typename TEnum> requires(std::is_enum_v<TEnum>)
    inline constexpr auto Values = ValuesImpl<TEnum>();

    ////////////////////////////////////////////////////////////////////////////////////
    // Entries
    ////////////////////////////////////////////////////////////////////////////////////
    template<typename TEnum, size_t... I> requires(std::is_enum_v<TEnum>)
    constexpr auto EntriesImpl(std::index_sequence<I...>)
    {
        return std::array<std::pair<TEnum, std::string_view>, sizeof...(I)>{
            { { Values<TEnum>[I], Name<TEnum, Values<TEnum>[I]>() }... }
        };
    }
    
    template<typename TEnum> requires(std::is_enum_v<TEnum>)
    inline constexpr auto Entries = EntriesImpl<TEnum>(std::make_index_sequence<Values<TEnum>.size()>());

}