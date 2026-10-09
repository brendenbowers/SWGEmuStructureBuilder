#include "Tre/DataStream.h"


namespace SWGEmuStructureBuilder::Tre
{

    DataStream::DataStream(std::streambuf &parentStream, std::streampos parentOffset, std::streamsize streamSize)
        : parentStream(parentStream)
        , parentOffset(parentOffset)
        , pos(0)
        , streamSize(streamSize)
    {
        if (parentOffset < std::streampos(0) || streamSize < 0)
        {
            throw std::invalid_argument("Invalid substream bounds");
        }

        if (parentStream.pubseekpos(parentOffset, std::ios::in) == std::streampos(std::streamoff(-1))) {
            throw std::ios_base::failure("Cannot seek parent stream");
        }
    }

    std::streambuf::int_type DataStream::underflow()
    {
        
        if(pos == streamSize)
        {
            return traits_type::eof();
        }

        return parentStream.sgetc();        
    }
    std::streambuf::int_type Tre::DataStream::uflow()
    {
        if(pos == streamSize)
        {
            return traits_type::eof();
        }

        const int value = parentStream.sbumpc();
        if (!traits_type::eq_int_type(value, traits_type::eof()))
        {
            pos += 1;
        }

        return value;
    }
    std::streamsize Tre::DataStream::xsgetn(char *destination, std::streamsize count)
    {
        count = std::min(count, streamSize - pos);
        if (count <= 0)
            return 0;

        const std::streamsize received = parentStream.sgetn(destination, count);
        pos += received;
        return received;
    }
}
