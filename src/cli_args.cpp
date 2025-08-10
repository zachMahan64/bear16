#include "cli_args.h"
#include <filesystem>
#include <iostream>
#include <unordered_set>
#include <vector>

const std::unordered_map<std::string, cli_flag> stringToArgFlagMap{
    {"-a", cli_flag::assemble},
    {"-r", cli_flag::run},
    {"-h", cli_flag::help},
    {"-v", cli_flag::version},
    {"-u", cli_flag::tui},
    {"--assemble", cli_flag::assemble},
    {"--run", cli_flag::run},
    {"--help", cli_flag::help},
    {"--version", cli_flag::version},
    {"--ui", cli_flag::tui},
    {"--tui", cli_flag::tui},
    {"-ar", cli_flag::valid_multi},
    {"-ar", cli_flag::valid_multi},
    {"-d", cli_flag::set_disk},
    {"--disk", cli_flag::set_disk},
    {"--set-disk", cli_flag::set_disk}};

const std::unordered_map<cli_error_e, std::string> errMsgMap{
    {cli_error_e::multiple_incompatible_flags, "Multiple incompatible flags provided"},
    {cli_error_e::too_many_asm_files, "Too many assembly files provided"},
    {cli_error_e::too_many_bin_files, "Too many binary files provided"},
    {cli_error_e::missing_asm_file, "Missing required assembly file"},
    {cli_error_e::missing_bin_file, "Missing required binary file"},
    {cli_error_e::unrecognized_arg, "Unrecognized argument(s)"},
    {cli_error_e::bin_file_does_not_exist, "Binary file does not exist"},
    {cli_error_e::too_many_arguments, "Too many arguments with given flags"}};

std::vector<std::string> vectorizeArgs(int argc, char **argv) {
    std::vector<std::string> args = {};
    for (int i = 1; i < argc; i++) {
        args.emplace_back(argv[i]);
    }
    return args;
}

std::unordered_set<cli_flag> parseFlags(const std::vector<std::string> &args,
                                        std::unordered_set<cli_error_e> &cliErrorState) {
    std::unordered_set<cli_flag> parsedFlags{};
    parsedFlags.reserve(args.size());
    for (const auto &arg : args) {
        if (arg == "-ar" || arg == "-ra") {
            parsedFlags.insert(cli_flag::assemble);
            parsedFlags.insert(cli_flag::run);
        } else if (stringToArgFlagMap.count(arg)) {
            parsedFlags.insert(stringToArgFlagMap.at(arg));
        }
    }

    if (parsedFlags.size() > 1 &&
        !(parsedFlags.size() == 2 && parsedFlags.contains(cli_flag::assemble) &&
          parsedFlags.contains(cli_flag::run))) {
        cliErrorState.insert(cli_error_e::multiple_incompatible_flags);
    }

    return parsedFlags;
}

bool fileExistsAndIsValid(const std::string &filePath, const std::string &fileSuffix) {
    return filePath.ends_with(fileSuffix) && std::filesystem::exists(filePath);
}

MentionedFiles parseArgsForMentionedFiles(const std::vector<std::string> &args,
                                          std::unordered_set<cli_error_e> &cliErrorState) {
    MentionedFiles mentionedFiles{};
    for (const auto &arg : args) {
        if (fileExistsAndIsValid(arg, asm_suffix)) {
            if (mentionedFiles.asmFile.empty()) {
                mentionedFiles.asmFile = arg;
            } else {
                cliErrorState.insert(cli_error_e::too_many_asm_files);
            }
        } else if (fileExistsAndIsValid(arg, bin_suffix)) {
            if (mentionedFiles.binFile.empty()) {
                mentionedFiles.binFile = arg;
                mentionedFiles.binFileState = bin_file_state::exists;
            } else {
                cliErrorState.insert(cli_error_e::too_many_bin_files);
            }
        } else if (arg.ends_with(bin_suffix)) {
            if (mentionedFiles.binFile.empty()) {
                mentionedFiles.binFile = arg;
                mentionedFiles.binFileState = bin_file_state::does_not_exist;
            } else {
                cliErrorState.insert(cli_error_e::too_many_bin_files);
            }
        }
    }
    return mentionedFiles;
}

bool parseForUnrecognizedArgs(const std::vector<std::string> &args,
                              std::unordered_set<cli_error_e> &cliErrorState) {
    bool errFlag = false;
    auto isUnreckonized = [](std::string arg) {
        return (!stringToArgFlagMap.contains(arg) && !arg.ends_with(asm_suffix) &&
                !arg.ends_with(bin_suffix) && arg != bear16_executable_name);
    };
    for (const auto &arg : args) {
        if (isUnreckonized(arg)) {
            cliErrorState.insert(cli_error_e::unrecognized_arg);
            std::cerr << "Unrecognized argument: \" << arg << \"\n";
            errFlag = true;
        }
    }
    return errFlag;
}
