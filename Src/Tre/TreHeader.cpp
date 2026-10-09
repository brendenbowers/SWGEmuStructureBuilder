#include "Tre/TreHeader.h"

#include <cassert>

#include <cstring>
#include <filesystem>
#include <vector>

#include "Tre/Helpers/ReadHelpers.h"
#include "Tre/Helpers/WriteHelpers.h"


namespace SWGEmuStructureBuilder::Tre
{
    namespace Read
    {
        TreHeader ReadHeader(std::istream &stream)
        {
            {
                std::vector<char> buffer(4);
                stream.read(buffer.data(), buffer.size());

                if(stream.gcount() != buffer.size()) {
                    throw std::ios_base::failure("Failed to read file header");
                }

                if(!Private::CompareVecToString(buffer, "EERT") && !Private::CompareVecToString(buffer, "TREE")) {
                    throw std::ios_base::failure("Invalid file header");
                }
            }

            TreHeader header;
            Private::ReadLittleEndian(stream, header.version);
            Private::ReadLittleEndian(stream, header.fileCount);
            Private::ReadLittleEndian(stream, header.recordOffset);
            Private::ReadLittleEndian<SWGEmuStructureBuilder::Tre::CompressionMethod, std::uint32_t>(stream, header.recordCompression);
            Private::ReadLittleEndian(stream, header.recordSize);
            Private::ReadLittleEndian<SWGEmuStructureBuilder::Tre::CompressionMethod, std::uint32_t>(stream, header.nameCompression);
            Private::ReadLittleEndian(stream, header.nameSize);
            Private::ReadLittleEndian(stream, header.rawNameSize);

            return header;
        }
    }

    namespace Write
    {
        constexpr uint32_t HeaderTreeIdentifier = 0x54524545u; // "EERT" in little-endian

        void WriteHeader(std::ostream &stream, const TreHeader &header)
        {
            Private::WriteLittleEndian(stream, HeaderTreeIdentifier);
            Private::WriteLittleEndian(stream, header.version);
            Private::WriteLittleEndian(stream, header.fileCount);
            Private::WriteLittleEndian(stream, header.recordOffset);
            Private::WriteEnumLittleEndian<CompressionMethod, uint32_t>(stream, header.recordCompression);
            Private::WriteLittleEndian(stream, header.recordSize);
            Private::WriteEnumLittleEndian<CompressionMethod, uint32_t>(stream, header.nameCompression);
            Private::WriteLittleEndian(stream, header.nameSize);
            Private::WriteLittleEndian(stream, header.rawNameSize);
        }
    }
}
