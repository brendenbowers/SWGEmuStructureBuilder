#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <istream>

#include "Tre/Chunk.h"
#include "Tre/DataStream.h"


namespace SWGEmuStructureBuilder::Tre
{
    class IFFReader {
    public:
        IFFReader(std::istream& stream);


        std::vector<IFFChunk> ReadChunks();
        std::vector<IFFChunk> ReadChunksInRange(const uint32_t startOffset, const uint32_t end);
        std::vector<IFFChunk> ReadChildren(const IFFChunk& chunk);
        std::vector<IFFChunk> FindChildForms(const IFFChunk& parent);
        std::vector<IFFChunk> FindAllChildChunks(const IFFChunk& parent, const IFFTag tag);


        bool FindFormRecursive(const uint32_t startOffset, const uint32_t end, const IFFTag FormType, IFFChunk& outChunk);

        bool FindForm(const IFFTag formType, IFFChunk& outChunk) { return FindFormRecursive(0, streamSize, formType, outChunk); }

        bool FindChildForm(const IFFChunk& chunk, const IFFTag formType, IFFChunk& outChunk);

        std::streambuf* GetChunkData(const IFFChunk& chunk);

    private:
        std::istream& stream;
        uint32_t streamSize;
    };
}