#pragma once

#include <cassert>

#include <cstring>
#include <filesystem>
#include <array>


namespace SWGEmuStructureBuilder::Tre::Private {
    
    std::uint32_t SwgCrc(std::string_view path);

    template<typename T, std::size_t SIZE = sizeof(T)>
    void WriteLittleEndian(std::ostream& stream, const T& value)
    {
        static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>);
        static_assert(SIZE <= sizeof(T));

        std::array<char, SIZE> bytes{};
        T remaining = value;

        for (auto& byte : bytes) {
            byte = static_cast<char>(remaining & 0xFFu);
            remaining >>= 8;
        }

        stream.write(bytes.data(), bytes.size());
    }

    template<typename TEnum, typename TStorage, std::size_t SIZE = sizeof(TStorage)>
    void WriteEnumLittleEndian(std::ostream& stream, const TEnum& value)
    {
        WriteLittleEndian<TStorage, SIZE>(stream, static_cast<TStorage>(value));
    }
}
