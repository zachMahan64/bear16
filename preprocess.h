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

//token defs
namespace preprocess {
    enum class PreprocessTokenType {
        INCLUDE,
        MACRO_DEF,
        MACRO_END_DEF,
        MACRO_USE,
        MISTAKE
    };
    class IncludeToken {
        std::string path;
        std::string contents {};
        void adjustPathAccordingToProjectPath(std::string& projectPath);
        [[nodiscard]] bool checkPathValidity() const;
        void buildContents();
    public:
        [[nodiscard]] std::string getPath() const;
        [[nodiscard]] std::string getContents() const;
        friend struct std::hash<IncludeToken>;
        bool operator==(const IncludeToken& other) const {
            return path == other.path;
        }
        explicit IncludeToken(std::string path, std::string projectPath);
    };
    class MacroDefToken {
        std::string name;
        std::string body;
        std::unordered_set<std::string> parameters {};
        void discernParametersFromBody();
    public:
        friend struct std::hash<MacroDefToken>;
        bool operator==(const MacroDefToken& other) const {
            return name == other.name && body == other.body;
        }
        explicit MacroDefToken(std::string name, std::string body);
    };
}
//hashing support for tokens
namespace std {
    template <>
    struct hash<preprocess::IncludeToken> {
        size_t operator()(const preprocess::IncludeToken& token) const noexcept {
            return hash<std::string>{}(token.path);
        }
    };

    template <>
    struct hash<preprocess::MacroDefToken> {
        size_t operator()(const preprocess::MacroDefToken& token) const noexcept {
            const size_t hashName = hash<std::string>{}(token.name);
            const size_t hashBody = hash<std::string>{}(token.body);
            return hashName ^ (hashBody << 1);
        }
    };
}
//preprocessor class & maps
namespace preprocess {
    extern const std::unordered_map<std::string, PreprocessTokenType> stringToPreprocessTokenTypeMap;
    class Preprocessor {
        std::string projectPath {};
        std::string entry {};
        std::vector<IncludeToken> includes {};
        std::vector<MacroDefToken> macros {};
        void addIncludeIfAbsent(const IncludeToken &tkn);
        void addMacroDefIfAbsent(const MacroDefToken& tkn);
        [[nodiscard]] static PreprocessTokenType deducePreprocessTokenType(const std::string &token);
    public:
        Preprocessor() = default;
        void setProject(const std::string &projectPath, const std::string &entry);
        std::string preprocessOpenAsmProject();
    };
}

#endif // PREPROCESS_H
