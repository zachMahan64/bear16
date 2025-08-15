#ifndef CLI_ARGS_H
#define CLI_ARGS_H

#include <cstdint>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
// FLAGS
enum class cli_flag : uint8_t {
    assemble,
    run,
    help,
    version,
    tui,
    valid_multi,
    check_disk,
    set_disk,
    reset_disk,
    doctor
}; // valid_multi currently redundant as of 20250809
extern const std::unordered_map<std::string, cli_flag> stringToArgFlagMap;
// ERRORS
enum class cli_error_e : uint8_t {
    multiple_incompatible_flags,
    too_many_asm_files,
    too_many_bin_files,
    missing_asm_file,
    missing_bin_file,
    unrecognized_arg,
    bin_file_does_not_exist,
    too_many_arguments
};
extern const std::unordered_map<cli_error_e, std::string> errMsgMap;
// FILE EXTENSIONS/STRING CONSTANTS
static std::string ASM_SUFFIX = ".asm", BIN_SUFFIX = ".bin", BEAR16_EXECUTABLE_NAME = "b16";
// FN's
std::vector<std::string> vectorizeArgs(int argc, char** argv);

std::unordered_set<cli_flag> parseFlags(const std::vector<std::string>& args,
                                        std::unordered_set<cli_error_e>& cliErrorState);

bool fileExistsAndIsValid(const std::string& filePath, const std::string& fileSuffix);

enum class bin_file_state : uint8_t { does_not_exist, exists };

struct MentionedFiles {
    std::string asmFile;
    std::string binFile;
    bin_file_state binFileState;
};

MentionedFiles parseArgsForMentionedFiles(const std::vector<std::string>& args,
                                          std::unordered_set<cli_error_e>& cliErrorState);

bool parseForUnrecognizedArgs(const std::vector<std::string>& args,
                              std::unordered_set<cli_error_e>& cliErrorState);

#endif
