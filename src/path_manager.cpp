#include "path_manager.h"

#include <algorithm> // ignore clang-tidy warnings, required for WIN32 builds
#include <cstdlib>
#include <iostream>
#include <string>

std::filesystem::path getUserHomeDir() {
    constexpr const char* HOME_NOT_FOUND_ERR_MSG = "Critical error: user folder not found";
#ifdef _WIN32
    const char* userFolderEnv = std::getenv("USERPROFILE");
#else
    const char* userFolderEnv = std::getenv("HOME");
#endif

    if ((userFolderEnv == nullptr) || std::string(userFolderEnv).empty()) {
        throw std::runtime_error(HOME_NOT_FOUND_ERR_MSG);
    }

    std::filesystem::path userHomeDir(userFolderEnv);

    if (!std::filesystem::exists(userHomeDir)) {
        throw std::runtime_error(HOME_NOT_FOUND_ERR_MSG);
    }

    return userHomeDir;
}

#ifdef _WIN32
#include <Shlobj.h>
#include <windows.h>
#endif

std::string snipUserHomeDir(const std::string& path) {
    if (path.empty()) {
        return path;
    }

    std::string home;

#ifdef _WIN32
    char buffer[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, buffer))) {
        home = std::string(buffer);
    } else {
        // fallback
        const char* env = std::getenv("USERPROFILE");
        home = env ? env : "";
    }
#else
    const char* env = std::getenv("HOME");
    home = (env != nullptr) ? env : "";
#endif

    if (home.empty()) {
        return path; // no home info
    }

    // Normalize home path separator for matching if needed:
    // On Windows, convert backslashes to forward slashes for comparison
#ifdef _WIN32
    for (auto& c : home)
        if (c == '\\')
            c = '/';
#endif

    std::string path_norm = path;
#ifdef _WIN32
    for (auto& c : path_norm)
        if (c == '\\')
            c = '/';
#endif

    if (path_norm.starts_with(home)) {
        size_t pos = home.size();
        if (path_norm.size() > pos && (path_norm[pos] == '/' || path_norm[pos] == '\\')) {
            pos++; // remove trailing slash
        }
        return path.substr(pos); // return remainder from original path (with
                                 // original separators)
    }

    return path; // no match, return original
}
