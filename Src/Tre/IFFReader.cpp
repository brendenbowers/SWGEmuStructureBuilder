#include "Tre/IFFReader.h"
#include "Tre/Helpers/ReadHelpers.h"

#include <sstream>



namespace SWGEmuStructureBuilder::Tre
{
    IFFReader::IFFReader(std::istream &stream)
        : stream(stream)
    {
        this->stream.seekg(0, std::ios::end);
        streamSize = stream.tellg();
        this->stream.seekg(0, std::ios::beg);
    }

    std::vector<IFFChunk> IFFReader::ReadChunks()
    {
        return ReadChunksInRange(0, streamSize);
    }

    std::vector<IFFChunk> IFFReader::ReadChunksInRange(const uint32_t startOffset, const uint32_t end)
    {
        std::vector<IFFChunk> chunks;

        stream.seekg(startOffset, std::ios::beg);

        uint32_t pos = stream.tellg();
        while(static_cast<uint32_t>(pos) + 8 <= end)
        {
            IFFTag tag(Private::Read<uint32_t, 4, true>(stream));
            int32_t size = Private::Read<int32_t, 4, true>(stream);

            if(size > end - pos - 8 )
            {
                break;
            }

            IFFChunk& chunk = chunks.emplace_back();
            chunk.tag = tag;
            if(tag == "FORM")
            {
                if(size < 4)
                {
                    break;
                }
                chunk.type = IFFTag(Private::Read<uint32_t, 4, true>(stream));
                chunk.dataSize = size - 4;
            }
            else
            {
                chunk.dataSize = size;
            }
            chunk.dataOffset = stream.tellg();
            stream.seekg(pos + size + 8, std::ios::beg);
            pos = stream.tellg();
        }

        return chunks;
    }

    std::vector<IFFChunk> IFFReader::ReadChildren(const IFFChunk &chunk)
    {
        if(chunk.tag != "FORM")
        {
            return {};
        }

        return ReadChunksInRange(chunk.dataOffset, chunk.dataOffset + chunk.dataSize);
    }

    std::vector<IFFChunk> IFFReader::FindChildForms(const IFFChunk &parent)
    {
        std::vector<IFFChunk> chunks;
        for(const IFFChunk& child : ReadChildren(parent))
        {
            if(child.tag == "FORM")
            {
                chunks.emplace_back(std::move(child));
            }
        }

        return chunks;
    }

    std::vector<IFFChunk> IFFReader::FindAllChildChunks(const IFFChunk &parent, const IFFTag tag)
    {
        std::vector<IFFChunk> chunks;
        for(const IFFChunk& child : ReadChildren(parent))
        {
            if(child.tag != "FORM" && child.tag == tag)
            {
                chunks.emplace_back(std::move(child));
            }
        }

        return chunks;
    }

    bool IFFReader::FindFormRecursive(const uint32_t startOffset, const uint32_t end, const IFFTag formType, IFFChunk &outChunk)
    {

        stream.seekg(startOffset, std::ios::beg);
        uint32_t pos = stream.tellg();

        while(pos + 8 < end)
        {
            const IFFTag tag(Private::Read<uint32_t, 4, true>(stream));
            const int32_t size = Private::Read<uint32_t, 4, true>(stream);

            if(size > end - pos - 8)
            {
                break;
            }

            if(tag != "FORM")
            {
                stream.seekg(pos + size + 8, std::ios::beg);
                pos = stream.tellg();
                continue;
            }

            if(size < 4)
            {
                break;
            }

            const IFFTag type(Private::Read<uint32_t, 4, true>(stream));
            if(type == formType)
            {
                outChunk.tag = tag;
                outChunk.type = type;
                outChunk.dataOffset = stream.tellg();
                outChunk.dataSize = size - 4;
                return true;
            }

            if(FindFormRecursive(pos + 12, pos + size + 8,  formType, outChunk))
            {
                return true;
            }

            stream.seekg(pos + size + 8, std::ios::beg);
            pos = stream.tellg();
        }
        
        return false;
    }

    bool IFFReader::FindChildForm(const IFFChunk &chunk, const IFFTag formType, IFFChunk &outChunk)
    {
        for( const IFFChunk& child : ReadChildren(chunk))
        {
            if(child.tag == "FORM" && child.type == formType)
            {
                outChunk = child;
                return true;
            }
        }

        return false;
    }

    std::streambuf *IFFReader::GetChunkData(const IFFChunk &chunk)
    {
        stream.seekg(chunk.dataOffset, std::ios::beg);
        return new DataStream(*stream.rdbuf(), chunk.dataOffset, chunk.dataSize);
    }
}
