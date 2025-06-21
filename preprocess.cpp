//
// Created by Zachary Mahan on 6/20/25.
//
#include "preprocess.h"

#include <utility>
namespace preprocess {
    //Tokens
    //Include Token ----------------------------------------------------------------------------------------------------
    IncludeToken::IncludeToken(std::string path, std::string projectPath) : path(std::move(path)) {

        adjustPathAccordingToProjectPath(projectPath);
        buildContents();
    }
    void IncludeToken::adjustPathAccordingToProjectPath(std::string& projectPath) {
        // Strip surrounding quotes if present
        if (!path.empty() && path.front() == '"' && path.back() == '"') {
            path = path.substr(1, path.size() - 2);
        }

        // Combine with project path
        path = std::filesystem::path(projectPath) / path;
    }
    bool IncludeToken::checkPathValidity() const {
        const std::ifstream file(path, std::ios::in | std::ios::binary);
        return file.is_open();
    }
    void IncludeToken::buildContents() {
        if (!checkPathValidity()) {
            contents = "";
            LOG_ERR("ERROR: Could not open .asm file for preprocessing: " + path);
            return;
        }
        LOG("Successfully opened .asm file: " + path + " for preprocessing");
        std::ifstream file(path, std::ios::in | std::ios::binary);
        contents = std::string((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());

    }
    std::string IncludeToken::getPath() const {
        return path;
    }
    std::string IncludeToken::getContents() const {
        return contents;
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
    std::string Preprocessor::preprocessOpenAsmProject() {
        std::string path = projectPath + entry;
        std::string revisedAsm {};
        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (!file) {
            LOG_ERR("Current working directory: " << std::filesystem::current_path());
            LOG_ERR("ERROR: Could not open .asm file for preprocessing: " << path);
            return {};
        }
        LOG("Successfully opened .asm file: " + path);

        std::string buffer((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());

        uint64_t lineNum = 1;

        bool inDirectiveDeclaration = false;
        bool inIncludeDirective = false;
        bool inMacroDef = false;
        bool inMacroBody = false;
        bool inMacroEndDef = false;
        bool inMacroUse = false;
        auto inPreprocessDirective = [&]() {
            return inDirectiveDeclaration || inIncludeDirective || inMacroDef || inMacroBody ||
                   inMacroEndDef || inMacroUse;
        };
        std::string revisedMainFile{};
        std::string currentStr{};
        for (const char& c : buffer) {
            if (c == '\t' || c == ' ' || c == '\n') {
                if (c == '\n') lineNum++;
                if (inDirectiveDeclaration) {
                    if (deducePreprocessTokenType(currentStr) == PreprocessTokenType::INCLUDE) {
                        inIncludeDirective = true;
                    }
                    inDirectiveDeclaration = false;
                    currentStr.clear();
                }
                if (inIncludeDirective) {
                    std::string pathToInclude = currentStr;
                    IncludeToken tkn(pathToInclude, projectPath);
                    this->addIncludeIfAbsent(tkn);
                    currentStr.clear();
                    inIncludeDirective = false;
                }
                continue;
            }
            if (c == '@') {
                currentStr += c;
                inDirectiveDeclaration = true;
                continue;
            }
            if (inDirectiveDeclaration) {
                currentStr += c;
            }
            if (!inPreprocessDirective()) {
                revisedMainFile += c;
            }
        }
        for (const auto& includeTkn : includes) {
            revisedAsm += includeTkn.getContents();
        }
        revisedAsm += revisedMainFile;
        return revisedAsm;
    }
    void Preprocessor::addIncludeIfAbsent(const IncludeToken& tkn) {
        if (const auto it = std::find(includes.begin(), includes.end(), tkn); it == includes.end()) {
            includes.emplace_back(tkn);
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
}
