#include "CommandLine/ParserSetup.h"

void SWGEmuStructureBuilder::CommandLine::SetupParser(argparse::ArgumentParser& Parser)
{
    Parser.add_argument("--treDir")
        .help("Directory containing the .tre files")
        .required();
}