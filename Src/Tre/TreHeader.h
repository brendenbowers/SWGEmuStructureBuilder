#pragma once

#include <cstdint>


#include "Tre/CompressionMethod.h"

namespace SWGEmuStructureBuilder::Tre {
    struct TreHeader {
        std::uint32_t version = 0;
        std::uint32_t fileCount = 0;
        std::uint32_t recordOffset = 36;
        CompressionMethod recordCompression = CompressionMethod::None;
        std::uint32_t recordSize = 0;
        CompressionMethod nameCompression = CompressionMethod::None;
        std::uint32_t nameSize = 0;
        std::uint32_t rawNameSize = 0;
    };
}