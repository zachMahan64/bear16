//
// Created by Zachary Mahan on 6/20/25.
//
#include "preprocess.h"
namespace preprocess {

    //Tokens
    //Include Token ----------------------------------------------------------------------------------------------------
    IncludeToken::IncludeToken(std::string path) : path(std::move(path)) {
        buildContents();
    }
    void IncludeToken::adjustPathAccordingToWorkingDirectory() {
        //TODO
    }
    bool IncludeToken::checkPathValidity() const {
        const std::ifstream file(path, std::ios::in | std::ios::binary);
        return file.is_open();
    }
    void IncludeToken::buildContents() {
        //TODO
    }
    void MacroToken::discernParametersFromBody() {
        //TODO
    }
    //Macro Token ------------------------------------------------------------------------------------------------------
    MacroToken::MacroToken(std::string name, std::string body)
    : name(std::move(name)), body(std::move(body)) {
        discernParametersFromBody();
    }
    //Preprocessor -----------------------------------------------------------------------------------------------------
    std::string Preprocessor::preprocessAsmFile(const std::string &path) {
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
        bool inPreprocDir = false;
        for (char c : buffer) {
            if (c == '@') {
                inPreprocDir = true;
                continue;
            }
        }

        return revisedAsm;
    }
    void Preprocessor::addIncludeIfAbsent(const IncludeToken &tkn) {
        if (!includes.contains(tkn)) includes.insert(tkn);
    }
    void Preprocessor::addMacroIfAbsent(const MacroToken& tkn) {
        if (!macros.contains(tkn)) macros.insert(tkn);
    }
}
