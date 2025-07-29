#include "path_manager.h"
#include <iostream>
std::filesystem::path getBear16RootDir() {
#ifdef _WIN32
    const char* userFolderEnv = std::getenv("USERPROFILE");
#else
    const char* userFolderEnv = std::getenv("HOME");
#endif

    if (!userFolderEnv || std::string(userFolderEnv).empty())
        throw std::runtime_error("User folder not found");

    std::filesystem::path userFolder(userFolderEnv);
    std::filesystem::path bear16Path = userFolder / "bear16";

    if (!std::filesystem::exists(bear16Path)) {
        std::cerr << "[bear16] Notice: ~/bear16 does not exist.\n";
        std::cerr << "          Create it manually or symlink to your cloned repo:\n";
        std::cerr << "          ln -s /path/to/your/repo ~/bear16\n";
        throw std::runtime_error("bear16 root directory not found.");
    }

    return bear16Path;
}
