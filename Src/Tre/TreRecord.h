#pragma once

#include <cstdint>

#include "Tre/CompressionMethod.h"


namespace SWGEmuStructureBuilder::Tre {
    struct TreRecord {
        std::uint32_t checksum = 0;         // CRC of the filename
        std::uint32_t dataUncompressed = 0; // Original file size
        std::uint32_t dataOffset = 0;       // Position from start of TRE
        CompressionMethod dataCompression = CompressionMethod::None;  // 0 = none, 2 = zlib
        std::uint32_t dataCompressed = 0;   // Stored file size
        std::uint32_t nameOffset = 0;       // Position in decompressed filename block
    };
}