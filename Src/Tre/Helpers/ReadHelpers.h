#pragma once

#include <cassert>

#include <cstring>
#include <filesystem>
#include <vector>
#include <streambuf>

namespace SWGEmuStructureBuilder::Tre::Private
{
    bool CompareVecToString(const std::vector<char>& vec, const std::string_view str);
    bool CompareVecToChars(const std::vector<char>& vec, std::initializer_list<char> chars);

    template<typename T, std::size_t SIZE = sizeof(T)>
    T ReadLittleEndian(std::istream& stream)
    {
        T value;
        ReadLittleEndian(stream, value);
        return value;
    }

    template<typename T, std::size_t SIZE = sizeof(T)>
    void ReadLittleEndian(std::istream& stream, T& value)
    {
        uint8_t buffer[SIZE];
        stream.read(reinterpret_cast<char*>(buffer), SIZE);
        if(stream.gcount() != SIZE) {
            throw std::ios_base::failure("Failed to read from stream");
        }

        std::memcpy(&value, buffer, SIZE);
    }

    template<typename TEnum, typename TStorage, std::size_t SIZE = sizeof(TStorage)>
    void ReadLittleEndian(std::istream& stream, TEnum& value)
    {
        TStorage storageValue;
        ReadLittleEndian<TStorage, SIZE>(stream, storageValue);
        value = static_cast<TEnum>(storageValue);
    }

    template<typename TEnum, typename TStorage, std::size_t SIZE = sizeof(TStorage)>
    TEnum ReadLittleEndian(std::istream& stream)
    {
        TStorage storageValue = ReadLittleEndian<TStorage, SIZE>(stream);
        return static_cast<TEnum>(storageValue);
    }
}