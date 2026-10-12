#include "Tre/TreRecord.h"
#include "Tre/Helpers/ReadHelpers.h"
#include "Tre/Helpers/WriteHelpers.h"

#include <cassert>

namespace SWGEmuStructureBuilder::Tre {
    namespace Read {
        TreRecord ReadRecord(std::istream& stream)
        {
            TreRecord record;
            std::streampos pos = stream.tellg();
            
            Private::Read(stream, record.checksum);
            Private::Read(stream, record.dataUncompressed);
            Private::Read(stream, record.dataOffset);
            Private::Read<SWGEmuStructureBuilder::Tre::CompressionMethod, std::uint32_t>(stream, record.dataCompression);
            Private::Read(stream, record.dataCompressed);
            Private::Read(stream, record.nameOffset);

            assert(stream.tellg() - pos == 24);

            return record;
        }
    }

    namespace Write {
        
        void WriteRecord(std::ostream& stream, TreRecord &record)
        {
            
            Private::WriteLittleEndian(stream, record.checksum);
            Private::WriteLittleEndian(stream, record.dataUncompressed);
            Private::WriteLittleEndian(stream, record.dataOffset);
            Private::WriteEnumLittleEndian<CompressionMethod, uint32_t>(stream, record.dataCompression);
            Private::WriteLittleEndian(stream, record.dataCompressed);
            Private::WriteLittleEndian(stream, record.nameOffset);
        }
    }
}