#pragma once

#include <cassert>

#include <cstring>
#include <filesystem>
#include <vector>
#include <streambuf>
#include <algorithm>

namespace SWGEmuStructureBuilder::Tre::Private
{
    bool CompareVecToString(const std::vector<char>& vec, const std::string_view str);
    bool CompareVecToChars(const std::vector<char>& vec, std::initializer_list<char> chars);

    template<typename T, std::size_t SIZE = sizeof(T), bool BE = false>
    void Read(std::istream& stream, T& value)
    {
        uint8_t buffer[SIZE];
        stream.read(reinterpret_cast<char*>(buffer), SIZE);
        assert(stream.gcount() == SIZE);

        if constexpr (!BE)
        {
            std::memcpy(&value, buffer, SIZE);
        }
        else
        {
            std::reverse_copy(
                buffer,
                buffer + SIZE,
                reinterpret_cast<unsigned char*>(&value));
        }
    }

    template<typename T, std::size_t SIZE = sizeof(T), bool BE = false>
    T Read(std::istream& stream)
    {
        T value;
        Read<T,SIZE,BE>(stream, value);
        return value;
    }

    template<typename TEnum, typename TStorage, std::size_t SIZE = sizeof(TStorage), bool BE = false>
    void Read(std::istream& stream, TEnum& value)
    {
        TStorage storageValue;
        Read<TStorage, SIZE, BE>(stream, storageValue);
        value = static_cast<TEnum>(storageValue);
    }

    template<typename TEnum, typename TStorage, std::size_t SIZE = sizeof(TStorage), bool BE = false>
    TEnum Read(std::istream& stream)
    {
        TStorage storageValue = Read<TStorage, SIZE, BE>(stream);
        return static_cast<TEnum>(storageValue);
    }
}