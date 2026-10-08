#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include "Tre/TreHeader.h"


namespace SWGEmuStructureBuilder::Tre {
    class TreFile {
    public:
        TreFile(const std::string& filePath);

        const std::string& GetFilePath() const { return filePath; }
        const std::fstream& GetFileHandle() const { return fileHandle; }
        std::fstream& GetFileHandle() { return fileHandle; }
        
        const TreHeader& GetHeader() const { return header; }

        void LoadFile();

    private:
        std::string filePath;
        std::fstream fileHandle;

        TreHeader header;
    };
}