#include "cli_args.h"
#include <filesystem>
#include <unordered_set>
#include <vector>

const std::unordered_map<std::string, cli_flag> stringToArgFlagMap{
    {"-a", cli_flag::assemble},
    {"-r", cli_flag::run},
    {"-h", cli_flag::help},
    {"-v", cli_flag::version},
    {"-u", cli_flag::ui},
    {"--assemble", cli_flag::assemble},
    {"--run", cli_flag::run},
    {"--help", cli_flag::help},
    {"--version", cli_flag::version},
    {"--ui", cli_flag::ui},
    {"-ar", cli_flag::valid_multi},
    {"-ar", cli_flag::valid_multi}};

std::vector<std::string> vectorizeArgs(int argc, char **argv) {
    std::vector<std::string> args = {};
    for (int i = 1; i < argc; i++) {
        args.emplace_back(argv[i]);
    }
    return args;
}

std::unordered_set<cli_flag>
parseFlags(const std::vector<std::string> &args,
           std::unordered_set<cli_error> &cliErrorState) {
    std::unordered_set<cli_flag> parsedFlags{};
    parsedFlags.reserve(args.size());
    for (const auto &arg : args) {
        if (stringToArgFlagMap.count(arg)) {
            parsedFlags.insert(stringToArgFlagMap.at(arg));
        } else if (arg == "-ar" || arg == "-ra") {
            parsedFlags.insert(cli_flag::assemble);
            parsedFlags.insert(cli_flag::run);
        }
    }

    if (parsedFlags.size() > 1 &&
        !(parsedFlags.size() == 2 && parsedFlags.contains(cli_flag::assemble) &&
          parsedFlags.contains(cli_flag::run))) {
        cliErrorState.insert(cli_error::multiple_incompatible_flags);
    }

    return parsedFlags;
}

bool isValidFile(const std::string &filePath, const std::string &fileSuffix) {
    return filePath.ends_with(fileSuffix) && std::filesystem::exists(filePath);
}

MentionedFiles
parseArgsForMentionedFiles(const std::vector<std::string> &args,
                           std::unordered_set<cli_error> &cliErrorState) {
    MentionedFiles mentionedFiles{};
    for (const auto &arg : args) {
        if (isValidFile(arg, asm_suffix)) {
            if (!mentionedFiles.asmFile.empty()) {
                mentionedFiles.asmFile = arg;
            } else {
                cliErrorState.insert(cli_error::too_many_asm_files);
            }
        } else if (isValidFile(arg, bin_suffix)) {
            if (!mentionedFiles.binFile.empty()) {
                mentionedFiles.binFile = arg;
            } else {
                cliErrorState.insert(cli_error::too_many_num_bin_files);
            }
        }
    }
    return mentionedFiles;
}

bool parseForUnrecognizedArgs(std::unordered_set<cli_error> &cliErrorState) {
    // TODO
}
