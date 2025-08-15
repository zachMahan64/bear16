#ifndef PATH_MANAGER_H
#define PATH_MANAGER_H

#include <filesystem>

std::filesystem::path getUserHomeDir();
std::string snipUserHomeDir(const std::string& path);
#endif // PATH_MANAGER_H
