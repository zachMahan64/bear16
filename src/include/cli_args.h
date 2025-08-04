#ifndef CLI_ARGS_H
#define CLI_ARGS_H

#include <string>
#include <unordered_map>
#include <vector>
enum class cli_flag { assemble, run, help, version, ui };
extern const std::unordered_map<std::string, cli_flag> stringToArgFlagMap;

std::vector<std::string> vectorizeArgs(int argc, char **argv);
std::vector<cli_flag> parseFlags(std::vector<std::string> args);
std::vector<std::string>
parseForAsmFile(); // make sure to get parent directory as "project path"
std::vector<std::string> parseForBinary(); // TODO
bool isValidFile(const std::string &filePath, const std::string &fileSuffix);

#endif
