#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <string>
#include <fstream>
#include <iostream>
#include <utility>
#include <unordered_set>

// Logging macros
#define LOG(x) std::cout << std::dec << x << std::endl
#define LOG_ERR(x) std::cerr << std::dec << x << std::endl

namespace preprocess {
    class IncludeToken {
        std::string path;
        std::string contents {};
        void adjustPathAccordingToWorkingDirectory();
        [[nodiscard]] bool checkPathValidity() const;
        void buildContents();
    public:
        friend struct std::hash<IncludeToken>;
        bool operator==(const IncludeToken& other) const {
            return path == other.path;
        }
        explicit IncludeToken(std::string path);
    };

    class MacroToken {
        std::string name;
        std::string body;
        std::unordered_set<std::string> parameters {};
        void discernParametersFromBody();
    public:
        friend struct std::hash<MacroToken>;
        bool operator==(const MacroToken& other) const {
            return name == other.name && body == other.body;
        }
        explicit MacroToken(std::string name, std::string body);
    };
}

namespace std {
    template <>
    struct hash<preprocess::IncludeToken> {
        size_t operator()(const preprocess::IncludeToken& token) const noexcept {
            return hash<std::string>{}(token.path);
        }
    };

    template <>
    struct hash<preprocess::MacroToken> {
        size_t operator()(const preprocess::MacroToken& token) const noexcept {
            const size_t h1 = hash<std::string>{}(token.name);
            const size_t h2 = hash<std::string>{}(token.body);
            return h1 ^ (h2 << 1);
        }
    };
}

namespace preprocess {
    class Preprocessor {
        enum class PreprocessTokenType {
            INCLUDE,
            MACRO,
            MISTAKE
        };
        std::unordered_set<IncludeToken> includes {};
        std::unordered_set<MacroToken> macros {};
    public:
        static std::string preprocessAsmFile(const std::string &path);
        void addIncludeIfAbsent(const IncludeToken &tkn);
        void addMacroIfAbsent(const MacroToken& tkn);
    };
}

#endif // PREPROCESS_H
