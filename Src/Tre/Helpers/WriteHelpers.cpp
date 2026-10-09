#include "Tre/Helpers/WriteHelpers.h"

namespace SWGEmuStructureBuilder::Tre::Private
{
    std::uint32_t SwgCrc(std::string_view path)
    {
        std::uint32_t crc = 0xFFFFFFFFu;

        for (unsigned char byte : path) {
            crc ^= static_cast<std::uint32_t>(byte) << 24;

            for (int bit = 0; bit < 8; ++bit) {
                const bool highBit = (crc & 0x80000000u) != 0;
                crc <<= 1;
                if (highBit)
                {
                    crc ^= 0x04C11DB7u;
                }
            }
        }

        return crc ^ 0xFFFFFFFFu;
    }
}