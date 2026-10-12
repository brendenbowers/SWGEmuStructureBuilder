#pragma once

#include <cstdint>
#include <string>
#include <assert.h>

struct IFFTag {
    
    uint32_t value = 0;

    constexpr IFFTag() = default;
    constexpr explicit IFFTag(const uint32_t val) : value(val) {}

    constexpr explicit IFFTag(const char* str) : value(
        (static_cast<uint32_t>(str[0]) << 24) |
        (static_cast<uint32_t>(str[1]) << 16) |
        (static_cast<uint32_t>(str[2]) << 8) |
        (static_cast<uint32_t>(str[3]))
    ) {}

    constexpr bool operator==(const IFFTag& other) const {
        return value == other.value;
    }

    constexpr bool operator!=(const IFFTag& other) const {
        return value != other.value;
    }

    constexpr bool operator==(const char* other) const {
        if(other == nullptr || std::char_traits<char>::length(other) != 4)
        {
            return false;
        }
        
        uint32_t otherValue = 
            (static_cast<uint32_t>(other[0]) << 24) |
            (static_cast<uint32_t>(other[1]) << 16) |
            (static_cast<uint32_t>(other[2]) << 8) |
            (static_cast<uint32_t>(other[3]));

        return otherValue == value;
    }

    constexpr bool operator!=(const char* other) const 
    {
        return !(*this == other);
    }

    const std::string ToString() const {
        char str[5] = { 
            static_cast<char>((value >> 24) & 0xFF),
            static_cast<char>((value >> 16) & 0xFF),
            static_cast<char>((value >> 8) & 0xFF),
            static_cast<char>(value & 0xFF),
            '\0'
        };
        return std::string(str);
    }


};

struct IFFChunk {
    IFFTag tag;
    IFFTag type;
    uint32_t dataOffset = 0;
    uint32_t dataSize = 0;
};