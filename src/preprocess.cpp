//
// Created by Zachary Mahan on 6/20/25.
//
#include "preprocess.h"

#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <utility>
namespace preprocess {
    //Tokens
    //Include Token ----------------------------------------------------------------------------------------------------
    IncludeToken::IncludeToken(std::string fileName, std::string projectPath) : fileName(std::move(fileName)) {
        adjustFullPathAccordingToProjectPath(projectPath);
        buildContents();
    }
    void IncludeToken::adjustFullPathAccordingToProjectPath(std::string& projectPath) {
        this->projectPath = projectPath;
        // Strip surrounding quotes if present
        if (!fileName.empty() && fileName.front() == '"' && fileName.back() == '"') {
            fileName = fileName.substr(1, fileName.size() - 2);
        }

        // Combine with project path
        fullPath = (std::filesystem::path(projectPath) / fileName).string();
    }
    bool IncludeToken::checkPathValidity() const {
        const std::ifstream file(fullPath, std::ios::in | std::ios::binary);
        return file.is_open();
    }
    void IncludeToken::buildContents() {
        if (!checkPathValidity()) {
            contents = "";
            LOG_ERR("ERROR: Could not open .asm file for preprocessing: " + fullPath);
            return;
        }
        LOG("Successfully opened .asm file: " + fullPath + " for building contents");
        std::ifstream file(fullPath, std::ios::in | std::ios::binary);
        contents = std::string((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
        contents += '\n'; //for safety
        LOG("Included contents: \n" + contents);

    }
    std::string IncludeToken::getFullPath() const {
        return fullPath;
    }
    std::string IncludeToken::getContents() const {
        return contents;
    }
    std::string IncludeToken::getFileName() const {
        return fileName;
    }

    //Macro Token ------------------------------------------------------------------------------------------------------
    MacroDefToken::MacroDefToken(std::string name, std::string body)
    : name(std::move(name)), body(std::move(body)) {
        discernParametersFromBody();
    }
    void MacroDefToken::discernParametersFromBody() {
        //TODO
    }
    //Preprocessor -----------------------------------------------------------------------------------------------------
    inline const std::unordered_map<std::string, PreprocessTokenType> stringToPreprocessTokenTypeMap = {
        {"@include", PreprocessTokenType::INCLUDE},
        {"@def", PreprocessTokenType::MACRO_DEF},
        {"@end_def", PreprocessTokenType::MACRO_END_DEF},
        {"@use", PreprocessTokenType::MACRO_USE},
    };
    void Preprocessor::setProject(const std::string &projectPath, const std::string &entry) {
        this->projectPath = projectPath;
        this->entry = entry;
    }
    std::string Preprocessor::preprocessAsmProject(const std::string &fileName) {
        std::string path = (std::filesystem::path(projectPath) / fileName).string();
        std::string revisedAsm {};
        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (!file) {
            LOG_ERR("Current working directory: " << std::filesystem::current_path());
            LOG_ERR("ERROR: Could not open .asm file for preprocessing: " << path);
            return {};
        }
        LOG("Successfully opened .asm file: " + path + " for preprocessing.");

        std::string buffer((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());

        uint64_t lineNum = 1;
        uint64_t charsThisLine = 0;

        bool inDirectiveDeclaration = false;
        bool inIncludeDirective = false;
        //TODO -> Macros
        bool inMacroDef = false;
        bool inMacroBody = false;
        bool inMacroEndDef = false;
        bool inMacroUse = false;

        auto inPreprocessDirective = [&]() {
            return inDirectiveDeclaration || inIncludeDirective || inMacroDef || inMacroBody ||
                   inMacroEndDef || inMacroUse;
        };
        std::string revisedMainFile{};
        std::string includedFilesContents{};
        std::string currentStr{};
        for (const char& c : buffer) {
            charsThisLine++;
            if (c == '\t' || c == ' ' || c == '\n') {
                revisedMainFile += c;
                if (c == '\n') {
                    lineNum++;
                    charsThisLine = 0;
                }
                if (inDirectiveDeclaration) {
                    if (deducePreprocessTokenType(currentStr) == PreprocessTokenType::INCLUDE) {
                        inIncludeDirective = true;
                    }
                    inDirectiveDeclaration = false;
                    currentStr.clear();
                }
                if (inIncludeDirective && !isBlank(currentStr)) {
                    std::string pathToInclude = currentStr;
                    LOG("pathToInclude = " + pathToInclude);
                    IncludeToken tkn(pathToInclude, projectPath);
                    if (addIncludeIfAbsent(tkn)) {
                        includedFilesContents += preprocessAsmProject(tkn.getFileName());
                    }
                    currentStr.clear();
                    inIncludeDirective = false;
                }
                continue;
            }
            if (c == '@' && charsThisLine == 1) {
                currentStr += c;
                inDirectiveDeclaration = true;
                continue;
            }
            if (inIncludeDirective) {
                currentStr += c;
                continue;
            }
            if (inDirectiveDeclaration) {
                currentStr += c;
            }
            if (!inPreprocessDirective()) {
                revisedMainFile += c;
            }
        }
        revisedMainFile += '\n'; //for safety
        revisedAsm = revisedMainFile; //put this first so we end w/ PC = 0
        revisedAsm += includedFilesContents;
        return revisedAsm;
    }

    void Preprocessor::reset() {
        includes.clear();
        macros.clear();
        LOG("Resetting preprocessor");
    }

    bool Preprocessor::addIncludeIfAbsent(const IncludeToken& tkn) {
        const std::string& fullPath = tkn.getFullPath();
        const std::string entryPath = (std::filesystem::path(projectPath) / entry).string();
        if (fullPath == entryPath) {
            LOG("DID NOT ADD " + fullPath + " SINCE IT IS THE ENTRY FILE.");
            return false;
        }

        if (std::find(includes.begin(), includes.end(), tkn) == includes.end()) {
            includes.push_back(tkn);
            LOG("ADDED " + fullPath + " to includes");
            return true;
        } else {
            LOG("DID NOT ADD " + fullPath + " -> already present in includes");
            return false;
        }
    }
    void Preprocessor::addMacroDefIfAbsent(const MacroDefToken& tkn) {
        if (const auto it = std::find(macros.begin(), macros.end(), tkn); it == macros.end()) {
            macros.emplace_back(tkn);
        }
    }
    PreprocessTokenType Preprocessor::deducePreprocessTokenType(const std::string &token) {
        if (stringToPreprocessTokenTypeMap.contains(token)) {
            return stringToPreprocessTokenTypeMap.at(token);
        }
        return PreprocessTokenType::MISTAKE;
    }
    //helper
    bool isBlank(const std::string& str) {
        if (str.empty()) {
            return true;
        }
        return std::all_of(str.begin(), str.end(), [](unsigned char c) {
            return std::isspace(c);
        });
    }
}
