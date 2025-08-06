#include "cli_args.h"
#include <filesystem>
#include <unordered_set>
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

std::unordered_set<cli_flag> parseFlags(const std::vector<std::string> &args) {
    std::unordered_set<cli_flag> parsedFlags{};
    parsedFlags.reserve(args.size());
    for (const auto &arg : args) {
        if (stringToArgFlagMap.count(arg)) {
            parsedFlags.insert(stringToArgFlagMap.at(arg));
        }
    }
    // TODO
    return parsedFlags;
}

bool isValidFile(const std::string &filePath, const std::string &fileSuffix) {
    return filePath.ends_with(fileSuffix) && std::filesystem::exists(filePath);
}
