#ifndef CLI_ARGS_H
#define CLI_ARGS_H

#include <string>
#include <unordered_map>
#include <vector>
enum class cli_arg { assemble, run, help, version, ui };
extern const std::unordered_map<std::string, cli_arg> stringToArgFlagMap;

std::vector<std::string> vectorizeArgs(int argc, char **argv);
std::vector<cli_arg> parseCLIArgs(std::vector<std::string> args);

#endif
