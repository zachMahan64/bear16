#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <string>
#include <fstream>
#include <iostream>
#include <utility>
#include <unordered_set>

// Logging macros
//macros
//#define DEBUG_MODE
#ifdef DEBUG_MODE
  #define LOG(x) std::cout << x << std::endl
#else
  #define LOG(x)
#endif
#define LOG_ERR(x) std::cerr << x << std::endl

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
        std::string fullPath {};
        std::string projectPath {};
        std::string fileName {};
        std::string contents {};
        void adjustFullPathAccordingToProjectPath(std::string& projectPath);
        [[nodiscard]] bool checkPathValidity() const;
        void buildContents();
    public:
        [[nodiscard]] std::string getFullPath() const;
        [[nodiscard]] std::string getContents() const;
        [[nodiscard]] std::string getFileName() const;
        friend struct std::hash<IncludeToken>;
        bool operator==(const IncludeToken& other) const {
            return fullPath == other.fullPath;
        }
        explicit IncludeToken(std::string fileName, std::string projectPath);
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
            return hash<std::string>{}(token.fullPath);
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
        bool addIncludeIfAbsent(const IncludeToken &tkn);
        void addMacroDefIfAbsent(const MacroDefToken& tkn);
        [[nodiscard]] static PreprocessTokenType deducePreprocessTokenType(const std::string &token);
    public:
        Preprocessor() = default;
        void setProject(const std::string &projectPath, const std::string &entry);
        auto preprocessAsmProject(const std::string &fileName) -> std::string;
    };
    //helper
    bool isBlank(const std::string& str);
}

#endif // PREPROCESS_H
