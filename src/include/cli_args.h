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
enum class cli_error_e {
    multiple_incompatible_flags,
    too_many_asm_files,
    too_many_bin_files,
    missing_asm_file,
    missing_bin_file,
    unrecognized_arg,
    bin_file_does_not_exist
};
extern const std::unordered_map<cli_error_e, std::string> errMsgMap;
// FILE EXTENSIONS
static std::string asm_suffix = ".asm", bin_suffix = ".bin", bear16_executable_name = "b16";
// FN's
std::vector<std::string> vectorizeArgs(int argc, char **argv);

std::unordered_set<cli_flag> parseFlags(const std::vector<std::string> &args,
                                        std::unordered_set<cli_error_e> &cliErrorState);

std::vector<std::string> parseForAsmFile();
// ^ make sure to get parent directory as "project path"

std::vector<std::string> parseForBinary(); // TODO
//
bool fileExistsAndIsValid(const std::string &filePath, const std::string &fileSuffix);

enum class bin_file_state { does_not_exist, exists };

struct MentionedFiles {
    std::string asmFile{};
    std::string binFile{};
    bin_file_state binFileState;
};

MentionedFiles parseArgsForMentionedFiles(const std::vector<std::string> &args,
                                          std::unordered_set<cli_error_e> &cliErrorState);

bool parseForUnrecognizedArgs(const std::vector<std::string> &args,
                              std::unordered_set<cli_error_e> &cliErrorState);

void throwAnyErrorsFromArgParsing(const std::unordered_set<cli_error_e> &errors);

#endif
