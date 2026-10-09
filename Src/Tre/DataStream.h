#pragma once

#include <iostream>
#include <streambuf>

namespace SWGEmuStructureBuilder::Tre
{
    class DataStream : public std::streambuf {
    public:
        DataStream(std::streambuf& parentStream, std::streampos parentOffset, std::streamsize streamSize);
        DataStream(DataStream&) = delete;
        DataStream& operator=(DataStream&) = delete;

    protected:
        std::streambuf::int_type underflow() override;
        std::streambuf::int_type uflow() override;
        std::streamsize xsgetn(char* destination, std::streamsize count) override;
        
    private:
        std::streambuf& parentStream;
        std::streampos parentOffset;
        std::streampos pos;
        std::streamsize streamSize;
    };
}
