#include "script/cs_compiler.h"
#include "script/cc_options.h"
#include "script/cc_error.h"
#include "util/filestream.h"
#include "util/file.h"
#include <fstream>
#include <string>
#include "cxxopts.hpp"

std::string readFileToString(const std::string& filename)
{
    std::ifstream ifs(filename);
    std::string content( (std::istreambuf_iterator<char>(ifs) ),
                         (std::istreambuf_iterator<char>()    ) );
    return content;
}

void parse(int argc, char* argv[])
{
    try
    {
        bool isRoomScript = false;
        cxxopts::Options options(argv[0], " - A command line compiler for preprocessed ags scripts.");
        options
                .positional_help("<input>")
                .show_positional_help();

        options.add_options()
                ("i,input", "Preprocessed ASC file to build.", cxxopts::value<std::string>())
                ("H,Headers", "Header Files in order", cxxopts::value<std::vector<std::string>>(), "HEADER")
                ("o,output", "Specify output file name", cxxopts::value<std::string>(), "OUTPUT")
                ("r,roomscript", "Use if is a room script", cxxopts::value<bool>()->default_value("false"))
                ("h,help", "Print usage")
                ;

        options.parse_positional({"input"});

        auto result = options.parse(argc, argv);
        std::vector<std::string> _headers;
        std::string _output;
        std::string _input;

        if (result.count("help"))
        {
            std::cout << options.help() << std::endl;
            exit(0);
        }

        if (result.count("input") && !(result["input"].as<std::string>().empty()))
        {
            _input = result["input"].as<std::string>();
            std::cout << "Input = " << _input << std::endl;
        }
        else
        {
            std::cout << "No preprocessed .asc Script file passed as input." << std::endl;
            exit(1);
        }

        std::string output_filename = "output.o";
        if (result.count("output"))
        {
            _output = result["output"].as<std::string>();
            std::cout << "Output = " << _output << std::endl;
            output_filename = _output;
        }
        if (result.count("Headers"))
        {
            _headers = result["Headers"].as<std::vector<std::string>>();
            std::cout << "Headers = " << std::endl;
            for(const auto& header: _headers)
            {
                std::cout << "  " <<  header << std::endl;
            }
        }
        if(result.count("roomscript"))
        {
            isRoomScript = true;
        }

        ccSetSoftwareVersion("version"); // need to set the version here somehow

        ccSetOption(SCOPT_EXPORTALL, 1);
        ccSetOption(SCOPT_LINENUMBERS, 1);
        // Don't allow them to override imports in the room script
        ccSetOption(SCOPT_NOIMPORTOVERRIDE, isRoomScript);

        ccSetOption(SCOPT_LEFTTORIGHT, 1 /*LeftToRightPrecedence*/);
        ccSetOption(SCOPT_OLDSTRINGS, 0 /*EnforceNewStrings*/);

        // need to figure out what the proper way to read and write files using what is already in AGS Common
        ccRemoveDefaultHeaders();
        std::vector<std::string> heads;
        for(const auto& header: _headers)
        {
            heads.push_back(readFileToString(header));
            ccAddDefaultHeader((char*) heads.back().c_str(), (char*) header.c_str());
        }

        std::string input_content;
        {
            input_content = readFileToString(_input);
        }

        ccScript* script = ccCompileText(input_content.c_str(), _input.c_str());
        if ((script == nullptr) || (ccError != 0))
        {
            std::cout << "ERROR: compile failed at " << ccCurScriptName << ", line " << ccErrorLine <<" : " << ccErrorString << std::endl;
            exit(1);
        }

        if(!AGS::Common::File::TestCreateFile(output_filename.c_str()))
        {
            std::cout << "ERROR: could not create output file " << output_filename << " ." << std::endl;
            exit(1);
        }
        AGS::Common::FileStream* output_file_stream = (AGS::Common::FileStream*) AGS::Common::File::CreateFile(output_filename.c_str());
        if(!output_file_stream->CanWrite())
        {
            std::cout << "ERROR: could not write to output file " << output_filename << " ." << std::endl;
            exit(1);
        }

        script->Write(output_file_stream);
        output_file_stream->Close();
    }
    catch (const cxxopts::OptionException& e)
    {
        std::cout << "error parsing options: " << e.what() << std::endl;
        exit(1);
    }
}

int main(int argc, char* argv[])
{
    parse(argc, argv);

    return 0;
}