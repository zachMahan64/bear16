#include "cli_args.h"
#include <vector>

const std::unordered_map<std::string, cli_arg> stringToArgFlagMap{
    {"", cli_arg::assemble}, {"", cli_arg::run}, {"", cli_arg::help},
    {"", cli_arg::version},  {"", cli_arg::ui},
};

std::vector<std::string> vectorizeArgs(int argc, char **argv) {
    std::vector<std::string> args = {};
    for (int i = 0; i < argc; i++) {
        args.emplace_back(argv[i]);
    }
    return args;
}

std::vector<cli_arg> parseCLIArgs(std::vector<std::string> args) {
    std::vector<cli_arg> parsedArgs{};
    parsedArgs.reserve(args.size());
    for (const auto &arg : args) {
        if (stringToArgFlagMap.count(arg)) {
            parsedArgs.emplace_back(stringToArgFlagMap.at(arg));
        }
    }
    // TODO
    return parsedArgs;
}
