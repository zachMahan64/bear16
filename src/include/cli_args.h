#ifndef CLI_ARGS_H
#define CLI_ARGS_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
// FLAGS
enum class cli_flag { assemble, run, help, version, ui, valid_multi, set_disk };
extern const std::unordered_map<std::string, cli_flag> stringToArgFlagMap;
// ERRORS
enum class cli_error {
    multiple_incompatible_flags,
    too_many_asm_files,
    too_many_num_bin_files,
    missing_asm_file,
    missing_bin_file,
    unrecognized_arg,
    bin_file_does_not_exist
}; // define an enum to string map for throwing these
// FILE EXTENSIONS
static std::string asm_suffix = ".asm", bin_suffix = ".bin";
// FN's
std::vector<std::string> vectorizeArgs(int argc, char **argv);

std::unordered_set<cli_flag>
parseFlags(const std::vector<std::string> &args,
           std::unordered_set<cli_error> &cliErrorState);

std::vector<std::string> parseForAsmFile();
// ^ make sure to get parent directory as "project path"

std::vector<std::string> parseForBinary(); // TODO
//
bool fileExistsAndIsValid(const std::string &filePath,
                          const std::string &fileSuffix);

enum class bin_file_state { does_not_exist, exists };

struct MentionedFiles {
    std::string asmFile{};
    std::string binFile{};
    bin_file_state binFileState;
};

MentionedFiles
parseArgsForMentionedFiles(const std::vector<std::string> &args,
                           std::unordered_set<cli_error> &cliErrorState);

bool parseForUnrecognizedArgs(std::unordered_set<cli_error> &cliErrorState);

#endif
