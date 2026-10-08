#include <iostream>
#include "CommandLine/ParserSetup.h"
#include "Tre/Tre.h"

using namespace SWGEmuStructureBuilder;

int main(const int argc, const char* const argv[]) {
    argparse::ArgumentParser parser;
    CommandLine::SetupParser(parser);

    try {
        parser.parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    const std::string treDir = parser.get<std::string>("--treDir");
    std::cout << "Tre Directory: " << treDir << std::endl;
    Tre::TreFile treFile(treDir);

    try {

        treFile.LoadFile();
    }
    catch (const std::ios_base::failure& e) {
        std::cerr << "Stream exception caught: " << e.what() << "\n";
        std::cerr << "Error Code: " << e.code().message() << "\n"; // C++11 exact OS reason
        return 1;
    } 
    catch (const std::exception& e) {
        std::cerr << "Error loading .tre file: " << e.what() << std::endl;
        return 1;
    }

    const Tre::TreHeader& header = treFile.GetHeader();

    std::cout << "Tre File Header Information:" << std::endl;
    std::cout << "Version: " << header.version << std::endl;
    std::cout << "File Count: " << header.fileCount << std::endl;   
    std::cout << "Record Offset: " << header.recordOffset << std::endl;
    std::cout << "Record Compression: " << header.recordCompression << std::endl;
    std::cout << "Record Size: " << header.recordSize << std::endl;
    std::cout << "Name Compression: " << header.nameCompression << std::endl;
    std::cout << "Name Size: " << header.nameSize << std::endl;
    std::cout << "Raw Name Size: " << header.rawNameSize << std::endl;

    return 0;
}