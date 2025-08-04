#include "cli_args.h"
#include <filesystem>
#include <vector>

const std::unordered_map<std::string, cli_flag> stringToArgFlagMap{
    {"", cli_flag::assemble}, {"", cli_flag::run}, {"", cli_flag::help},
    {"", cli_flag::version},  {"", cli_flag::ui},
};

std::vector<std::string> vectorizeArgs(int argc, char **argv) {
    std::vector<std::string> args = {};
    for (int i = 0; i < argc; i++) {
        args.emplace_back(argv[i]);
    }
    return args;
}

std::vector<cli_flag> parseFlags(std::vector<std::string> args) {
    std::vector<cli_flag> parsedArgs{};
    parsedArgs.reserve(args.size());
    for (const auto &arg : args) {
        if (stringToArgFlagMap.count(arg)) {
            parsedArgs.emplace_back(stringToArgFlagMap.at(arg));
        }
    }
    return parsedArgs;
}

bool isValidFile(const std::string &filePath, const std::string &fileSuffix) {
    return filePath.ends_with(fileSuffix) && std::filesystem::exists(filePath);
}
