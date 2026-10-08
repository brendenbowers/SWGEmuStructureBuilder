#include "Tre/Tre.h"

#include <cstring>
#include <filesystem>
#include <vector>

#include "Tre/TreHeader.h"

namespace SWGEmuStructureBuilder::Tre {
    namespace Private {
        bool CompareVecToString(const std::vector<char>& vec, const std::string_view str)
        {
            if(vec.size() != str.size()) {
                return false;
            }

            for(size_t i = 0; i < vec.size(); ++i) {
                if(vec[i] != str[i]) {
                    return false;
                }
            }

            return true;
        }

        bool CompareVecToChars(const std::vector<char>& vec, std::initializer_list<char> chars)
        {
            if(vec.size() != chars.size()) {
                return false;
            }

            for(size_t i = 0; i < vec.size(); ++i) {
                if(vec[i] != chars.begin()[i]) {
                    return false;
                }
            }

            return true;
        }

        template<typename T, std::size_t SIZE = sizeof(T)>
        T ReadLittleEndian(std::istream& stream)
        {
            T value;
            ReadLittleEndian(stream, value);
            return value;
        }

        template<typename T, std::size_t SIZE = sizeof(T)>
        void ReadLittleEndian(std::istream& stream, T& value)
        {
            uint8_t buffer[SIZE];
            stream.read(reinterpret_cast<char*>(buffer), SIZE);
            if(stream.gcount() != SIZE) {
                throw std::ios_base::failure("Failed to read from stream");
            }

            std::memcpy(&value, buffer, SIZE);
        }

        template<typename TEnum, typename TStorage, std::size_t SIZE = sizeof(TStorage)>
        void ReadLittleEndian(std::istream& stream, TEnum& value)
        {
            TStorage storageValue;
            ReadLittleEndian<TStorage, SIZE>(stream, storageValue);
            value = static_cast<TEnum>(storageValue);
        }

        template<typename TEnum, typename TStorage, std::size_t SIZE = sizeof(TStorage)>
        TEnum ReadLittleEndian(std::istream& stream)
        {
            TStorage storageValue = ReadLittleEndian<TStorage, SIZE>(stream);
            return static_cast<TEnum>(storageValue);
        }
    
    }


    TreFile::TreFile(const std::string& filePath)
        : filePath(filePath)
    {
    }

    void TreFile::LoadFile()
    {
        std::error_code ec;
        if (!std::filesystem::exists(filePath, ec)) {
            throw std::filesystem::filesystem_error("File does not exist", filePath.c_str(), ec);
        }

#ifdef DEBUG
        fileHandle.exceptions(std::ios::failbit | std::ios::badbit);
#endif
        fileHandle.open(filePath, std::ios::in | std::ios::out| std::ios::binary);

        if(!fileHandle.is_open()) {
            throw std::filesystem::filesystem_error("Failed to open file", filePath.c_str(), std::make_error_code(std::errc::no_such_file_or_directory));
        }

        {
            std::vector<char> buffer(4);
            fileHandle.read(buffer.data(), buffer.size());

            if(fileHandle.gcount() != buffer.size()) {
                throw std::ios_base::failure("Failed to read file header");
            }

            if(!Private::CompareVecToString(buffer, "EERT") && !Private::CompareVecToString(buffer, "TREE")) {
                throw std::ios_base::failure("Invalid file header");
            }
        }

        Private::ReadLittleEndian(fileHandle, header.version);
        Private::ReadLittleEndian(fileHandle, header.fileCount);
        Private::ReadLittleEndian(fileHandle, header.recordOffset);
        Private::ReadLittleEndian<SWGEmuStructureBuilder::Tre::CompressionMethod, std::uint32_t>(fileHandle, header.recordCompression);
        Private::ReadLittleEndian(fileHandle, header.recordSize);
        Private::ReadLittleEndian<SWGEmuStructureBuilder::Tre::CompressionMethod, std::uint32_t>(fileHandle, header.nameCompression);
        Private::ReadLittleEndian(fileHandle, header.nameSize);
        Private::ReadLittleEndian(fileHandle, header.rawNameSize);
    }
}