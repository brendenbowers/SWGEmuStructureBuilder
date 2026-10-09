#include "Tre/Tre.h"

#include <cassert>

#include <cstring>
#include <filesystem>
#include <vector>
#include <streambuf>

#include "Tre/TreHeader.h"
#include "Tre/DataStream.h"
#include "Tre/Helpers/ReadHelpers.h"
#include "Tre/Helpers/WriteHelpers.h"

namespace SWGEmuStructureBuilder::Tre {
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


        file.exceptions(std::ios::failbit | std::ios::badbit);
        file.open(filePath, std::ios::in | std::ios::out| std::ios::binary);

        if(!file.is_open()) {
            throw std::filesystem::filesystem_error("Failed to open file", filePath.c_str(), std::make_error_code(std::errc::no_such_file_or_directory));
        }

        header = Read::ReadHeader(file);
        ReadRecords();
    }

    void TreFile::SaveFile()
    {
        if(!header.dirty)
        {
            return;
        }

        std::string tempFilePath = filePath + ".tmp";

        std::ofstream fs;
        fs.exceptions(std::ios::failbit | std::ios::badbit);
        fs.open(tempFilePath, std::ios::out | std::ios::binary | std::ios::trunc);

        header.fileCount = records.size();

        // write the paritally completed header
        Write::WriteHeader(fs, header);

        // write the actual data to the block following the header
        for(auto it = records.begin(); it != records.end(); it++)
        {
            TreRecord& record = it->second;
            // hacky way to delete the substream from GetDataStream
            std::unique_ptr<std::streambuf> dataBufferDeleter;
            std::streambuf* dataBuffer = nullptr;

            if(it->second.IsDirty())
            {
                dataBuffer = it->second.GetDirtyData();
            }
            else
            {
                dataBufferDeleter.reset(GetDataStream(it->first));
                dataBuffer = dataBufferDeleter.get();
            }

            record.dataOffset =  fs.tellp();
            fs << dataBuffer;
            record.dataUncompressed = static_cast<size_t>(fs.tellp()) - record.dataOffset;
            record.dataCompressed = record.dataUncompressed;
            record.checksum = Private::SwgCrc(it->first);
        }
        header.recordOffset = fs.tellp();
        // write the records after the data
        for(auto it = records.begin(); it != records.end(); it++)
        {
            Write::WriteRecord(fs, it->second);
        }

        header.recordSize = static_cast<size_t>(fs.tellp()) - header.recordOffset;


        int count = 0;
        std::streampos nameBlockStart = fs.tellp();
        // write the path strings and then update the name offset in the record
        for(auto it = records.begin(); it != records.end(); it++)
        {
            std::streampos cur = fs.tellp();
            uint32_t nameOffset = nameBlockStart - cur;

            fs.seekp(header.recordOffset + (count * 24) + Write::NameOffsetOffset, std::ios::beg);
            Private::WriteLittleEndian(fs, nameOffset);
            fs.seekp(cur, std::ios::beg);
            fs.write(it->first.data(), it->first.size() + 1);
            count++;
        }

        header.nameSize = nameBlockStart - fs.tellp();
        header.rawNameSize = header.nameSize;

        // update the header
        fs.seekp(0, std::ios::beg);
        Write::WriteHeader(fs, header);
        fs.flush();
        fs.close();

        if(file && file.is_open())
        {
            file.close();
        }

        std::filesystem::rename(filePath, filePath + ".bak");
        std::filesystem::rename(filePath + ".tmp", filePath);
    }

    std::streambuf* TreFile::GetDataStream(const std::string &path) const
    {
        const TreRecord* record = GetRecord(path);

        if(!record)
        {
            return nullptr;
        }

        //todo: support compressiopn
        return new DataStream(*file.rdbuf(), record->dataOffset, record->dataUncompressed);
    }

    void TreFile::AddOrUpdateRecord(const std::string& path, std::streambuf& dataStream, CompressionMethod compression)
    {
            header.dirty = true;

            if(const auto& found = records.find(path); found != records.end())
            {
                
                found->second.SetData(&dataStream);
                found->second.dataCompression = compression;
            }
            else
            {
                TreRecord newRecord;
                newRecord.SetData(&dataStream);
                newRecord.dataCompression = compression;
                auto emplaced = records.emplace(path, std::move(newRecord));

                if(!emplaced.second)
                {
                    throw std::runtime_error("Failed to add/update record");
                }

                emplaced.first->second.name = emplaced.first->first;
            }
    }

    void TreFile::ReadRecords()
    {      
        if(header.recordOffset == 0)
        {
            return;
        }

        records.clear();

        file.seekg(header.recordOffset, std::ios::beg);

        std::streampos namesStart = header.recordOffset + (header.fileCount * 24);

        //todo: handle compression

        for(int i = 0; i < header.fileCount; i++)
        {
            
            TreRecord record = Read::ReadRecord(file);

            std::streampos pos = file.tellg();

            std::streampos nameOffset = static_cast<long>(namesStart) + record.nameOffset;
            file.seekg(nameOffset, std::ios::beg);
            
            std::string name;
            std::getline(file, name, '\0');

            auto result = records.emplace(std::move(name), std::move(record));
            if(result.second)
            {
                result.first->second.name = result.first->first;
            }

            file.seekg(pos, std::ios::beg);
        }
    }
}