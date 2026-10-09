#pragma once

#include <cstdint>
#include <string>

#include "Tre/CompressionMethod.h"


namespace SWGEmuStructureBuilder::Tre {
    struct TreRecord {
        std::uint32_t checksum = 0;         // CRC of the filename
        std::uint32_t dataUncompressed = 0; // Original file size
        std::uint32_t dataOffset = 0;       // Position from start of TRE
        CompressionMethod dataCompression = CompressionMethod::None;  // 0 = none, 2 = zlib
        std::uint32_t dataCompressed = 0;   // Stored file size
        std::uint32_t nameOffset = 0;       // Position in decompressed filename block

        std::string_view name;

        bool IsDirty() const { return isDirty; }

        void SetData(std::streambuf* stream) 
        {
            dataStream = stream;
            isDirty = true;
        }

        std::streambuf* GetDirtyData() { return dataStream; }

    protected:
        bool isDirty = false;
        std::streambuf* dataStream = nullptr;
    };

    namespace Read {
        TreRecord ReadRecord(std::istream& stream);   
    }

    namespace Write {
        // @brief the offset in a stream to the nameOffset
        constexpr uint32_t NameOffsetOffset = 20;

        void WriteRecord(std::ostream& stream, TreRecord& record);
    }
}