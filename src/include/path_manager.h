#ifndef PATH_MANAGER_H
#define PATH_MANAGER_H

#include <filesystem>

std::filesystem::path getBear16DefaultRootDir();
std::string snipHomeDir(const std::string &path);
#endif // PATH_MANAGER_H
