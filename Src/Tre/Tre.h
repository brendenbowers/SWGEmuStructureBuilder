#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <map>

#include "Tre/TreHeader.h"
#include "Tre/TreRecord.h"


namespace SWGEmuStructureBuilder::Tre {
    class TreFile {
    public:
        TreFile(const std::string& filePath);

        const std::string& GetFilePath() const { return filePath; }
        const std::fstream& GetFileHandle() const { return file; }
        std::fstream& GetFileHandle() { return file; }
        
        const TreHeader& GetHeader() const { return header; }

        const std::map<std::string, TreRecord>& GetRecords() const { return records; }

        const TreRecord* GetRecord(const std::string& path) const {
            if(const auto& found = records.find(path); found != records.end())
            {
                return &found->second;
            }

            return nullptr;
         }

         TreRecord* Getrecord(const std::string& path)
         {
            if(const auto& found = records.find(path); found != records.end())
            {
                return &found->second;
            }

            return nullptr;
         }


        std::streambuf* GetDataStream(const std::string& path) const;

        void AddOrUpdateRecord(const std::string& path, std::streambuf& dataStream, CompressionMethod compression = CompressionMethod::None);

        void LoadFile();
        void SaveFile();

    private:
        void ReadHeader();
        void ReadRecords();

        std::string filePath;
        std::fstream file;

        TreHeader header;

        std::map<std::string, TreRecord> records;
        
    };
}