//
// Created by Zachary Mahan on 5/23/25.
//
#include <unordered_map>
#include <string>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <regex>
#include "assembler.h"

#define LOG(x) std::cout << x << std::endl
#define LOG_ERR(x) std::cerr << x << std::endl

//tools
void asmToBinMapGenerator() {
    struct Instr {
        std::string base;
        uint16_t baseOpcode; // 14 bits used here, lower bits of opcode
    };
    const std::vector<Instr> instructions = {
        // Arithmetic & Bitwise
        {"add", 0x0000},
        {"sub", 0x0001},
        {"mult", 0x0002},
        {"div", 0x0003},
        {"mod", 0x0004},
        {"and", 0x0005},
        {"or", 0x0006},
        {"xor", 0x0007},
        {"not", 0x0008},
        {"nand", 0x0009},
        {"nor", 0x000A},
        {"neg", 0x000B},
        {"lsh", 0x000C},
        {"rsh", 0x000D},
        {"rol", 0x000E},
        {"ror", 0x000F},
        // Conditional
        {"eq", 0x0010},
        {"ne", 0x0011},
        {"lt", 0x0012},
        {"le", 0x0013},
        {"gt", 0x0014},
        {"ge", 0x0015},
        {"ult", 0x0016},
        {"ule", 0x0017},
        {"ugt", 0x0018},
        {"uge", 0x0019},
        {"z", 0x001A},
        {"nz", 0x001B},
        {"c", 0x001C},
        {"nc", 0x001D},
        {"n", 0x001E},
        {"nn", 0x001F},
        // Data Transfer
        {"mov", 0x0020},
        {"lw", 0x0021},
        {"lb", 0x0022},
        {"comp", 0x0023},
        {"lea", 0x0024},
        {"push", 0x0025},
        {"pop", 0x0026},
        {"clr", 0x0027},
        {"inc", 0x0028},
        {"dec", 0x0029},
        {"memcpy", 0x002A},
        {"sw", 0x002B},
        {"sb", 0x002C},
        // Control Flow
        {"call", 0x0040},
        {"ret", 0x0041},
        {"jmp", 0x0042},
        {"jcond_z", 0x0043},
        {"jcond_nz", 0x0044},
        {"jcond_neg", 0x0045},
        {"jcond_nneg", 0x0046},
        {"jcond_pos", 0x0047},
        {"jcond_npos", 0x0048},
        {"nop", 0x0049},
        {"hlt", 0x004A},
        // Video
        {"clrfb", 0x0080},
        {"setpx", 0x0081},
        {"blit", 0x0082},
    };

    // imm mode prefix bits:
    // normal:  0x0000
    // i (11):  0xC000
    // i1 (10): 0x8000
    // i2 (01): 0x4000
    std::vector<std::pair<std::string,uint16_t>> immModes = {
        {"",    0x0000},  // normal
        {"i",   0xC000},  // imm mode 11
        {"i1",  0x8000},  // imm mode 10
        {"i2",  0x4000},  // imm mode 01
    };

    std::cout << "const std::unordered_map<std::string, uint16_t> asmToBinMap = {\n";
    for (auto& instr : instructions) {
        for (auto& imm : immModes) {
            std::string mnemonic = instr.base + imm.first;
            uint16_t opcode = imm.second | instr.baseOpcode;
            std::cout << "    {\"" << mnemonic << "\", 0x"
                      << std::hex << std::uppercase << std::setw(4) << std::setfill('0')
                      << opcode << "},\n";
        }
    }
    std::cout << "};\n";
} //currently out-of-date! (5/25/25)
//parsing
std::vector<assembler::Token> assembler::tokenizeAsmFirstPass(const std::string &path) {
    std::vector<Token> firstPassTokens {};

    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Current working directory: " << std::filesystem::current_path() << std::endl;
        std::cerr << "ERROR: Could not open .asm file: " << path << std::endl;
        return firstPassTokens;
    }

    std::string buffer((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());

    std::cout << "Orig asm (debug):\n" << buffer << std::endl;

    std::string currentStr {};
    bool inComment = false;

    for (char c : buffer) {
        if (inComment) {
            if (c == '\n') {
                inComment = false;
                firstPassTokens.emplace_back(currentStr);
                firstPassTokens.emplace_back('\n');
                currentStr.clear();
                continue;
            }
            currentStr += c;
            continue;
        }
        if (c == '#') {
            inComment = true;
            currentStr += c;
            continue;
        }
        if (c == ' ' || c == '\t' || c == '\n') {
            if (!currentStr.empty()) {
                firstPassTokens.emplace_back(currentStr);
                currentStr.clear();
            }
            if (c == '\n') {
                firstPassTokens.emplace_back("\n");
            }
        }
        else if (c == ':') {
            if (!currentStr.empty()) {
                // append colon to current token (label case)
                currentStr += c;
                firstPassTokens.emplace_back(currentStr);
                currentStr.clear();
            } else {
                // colon alone (rare case)
                firstPassTokens.emplace_back(":");
            }
        }
        else if (c == ',' || c == '[' || c == ']') {
            if (!currentStr.empty()) {
                firstPassTokens.emplace_back(currentStr);
                currentStr.clear();
            }
            firstPassTokens.emplace_back(std::string(1, c));
        }
        else {
            currentStr += c;
        }
    }

    // Add last token if exists
    if (!currentStr.empty()) {
        firstPassTokens.emplace_back(currentStr);
    }

    std::cout << "First pass tokens:\n";
    for (auto& token : firstPassTokens) {
        std::cout << toString(token.type) + "-" << token.body << " ";
    }
    std::cout << std::endl;

    return firstPassTokens;
}

std::vector<assembler::TokenizedInstruction> assembler::parseFirstPassIntoSecondPass(std::vector<Token> &tokens) {
    //look-up tables
    std::unordered_map<std::string, uint16_t> labelMap = {};
    std::unordered_map<std::string, uint16_t> constMap = {};

    //break down into lines --> count lines for labels, resolve declarations and usages of labels/const --> form tokenized instructions
    std::vector<std::vector<Token>> tokenLines {};
    std::vector<Token> currentLine {};
    int lineNumInOrigAsm = 1;        //num lines on the editor
    //form lines
    for (const Token &tkn: tokens) {
        if (tkn.type == TokenType::MISTAKE) {
            currentLine.push_back(tkn);
            throwAFit(lineNumInOrigAsm);
        }
        else if (tkn.type == TokenType::EOL) {
            lineNumInOrigAsm++;
            if (!currentLine.empty()) {
                tokenLines.push_back(currentLine);
                currentLine.clear();
            }
        }
        else if (tkn.type == TokenType::OPERATION) {
            currentLine.push_back(tkn);
        }
        else if (tkn.type == TokenType::LABEL) {
            currentLine.push_back(tkn);
        }
        else if (tkn.type != TokenType::COMMENT && tkn.type != TokenType::COMMA) {
            currentLine.push_back(tkn);
        } else {
            LOG("IGNORED A TOKEN TYPE (2nd pass):" + toString(tkn.type));
        }
    }
    //handle hanging lines
    if (!currentLine.empty()) {
        tokenLines.push_back(currentLine);
    }
    //debug
    int numLinesInSecondPass = 0;
    std::cout << "\nTokenized lines (second pass):\n";
    for (const std::vector<Token> &line : tokenLines) {
        numLinesInSecondPass++;
        std::cout << "Line " << numLinesInSecondPass << ":" ;
        for (const Token &tkn : line) {
            std::cout << " " << toString(tkn.type) << "-";
            std::cout << tkn.body;
        }
        std::cout << std::endl;
    }

    //za big dada of tokens
    std::vector<TokenizedInstruction> finalPassTokens {};
    //form instructions or some shit, resolve references
    int numLines = 0;
    int instructionIndex = 0;
    for (const std::vector<Token> &line : tokenLines) {
         numLines++;
        if (line.empty()) {
            continue; //just ignore dat hoe
        }
        TokenType firstTknType =  line.at(0).type;
        //lines that should NOT get made into instructions
        if (firstTknType == TokenType::EOL) {
            continue; //blank line
        }
        if (firstTknType == TokenType::LABEL) {
            labelMap.emplace(line.at(0).body.substr(0, line.at(0).body.length() - 1), instructionIndex);
        }
        else if (firstTknType == TokenType::CONST) {
            if (line.size() > 3 && line.at(1).type == TokenType::REF && line.at(2).type == TokenType::EQUALS
                    && (line.at(3).type == TokenType::DECIMAL
                    || line.at(3).type == TokenType::HEX
                    || line.at(3).type == TokenType::BIN))
            {
                int value = 0;
                try {
                    if (line.at(3).type == TokenType::DECIMAL) {
                        value = std::stoi(line.at(3).body, nullptr, 10);
                    } else if (line.at(3).type == TokenType::HEX) {
                        value = std::stoi(line.at(3).body, nullptr, 16);
                    } else if (line.at(3).type == TokenType::BIN) {
                        const auto &body = line.at(3).body;
                        if (body.rfind("0b", 0) == 0 && body.length() > 2)
                            value = std::stoi(body.substr(2), nullptr, 2);
                        else
                            throw std::invalid_argument("Binary constant without 0b prefix");
                    }
                } catch (const std::exception &e) {
                    LOG_ERR("ERROR: bad const value: " << line.at(3).body << " (" << e.what() << ")");
                    throw;
                }
                if(auto [it, inserted] = constMap.emplace(line.at(1).body, value); !inserted) {
                    LOG_ERR("ERROR: duplicate const name: " << line.at(1).body);
                }
            } else if (line.size() > 1){
                std::string complaint = line.at(1).body;
                throwAFit(complaint);
            } else {
                LOG_ERR("ERROR: bad const declaration at line with tokens: " << line.size());
            }
        }
        //lines that SHOULD get made into instructions
        else if (firstTknType == TokenType::OPERATION) {
            instructionIndex++;
            //first token is the OPCODE!
            OpCode opCode(line.at(0));
            TokenizedInstruction instrForThisLine(opCode);
            std::vector<Token> revisedTokens {}; //use this to build finalized operands, also make a function for this
            //add operands, STARTS at pos i = 1!
            for (int i = 1; i < line.size(); i++) {
                Token tkn = line.at(i);
                if (tkn.type == TokenType::REF) {
                    std::string revisedRef = tkn.body;
                    if (labelMap.contains(tkn.body)) {
                        revisedRef = std::to_string(labelMap.at(tkn.body));
                    } else if (constMap.contains(tkn.body)) {
                        revisedRef = std::to_string(constMap.at(tkn.body));
                    }
                    revisedTokens.emplace_back(revisedRef);
                }
            }
        }
    }
    return finalPassTokens;
}

//helpers
assembler::TokenType assembler::Token::deduceTokenType(const std::string &text) {
    TokenType type = TokenType::MISTAKE;
    if (opcodeToBinMap.contains(text)) {
        type = TokenType::OPERATION;
    } else if (namedOperandToBinMap.contains(text)) {
        if (text == "pc" || text == "sp" || text == "fp") {
            type = TokenType::SPEC_REG;
        } else if (text == "io0" || text == "io1") {
            type = TokenType::IO_PSEUDO_REG;
        } else {
            type = TokenType::GEN_REG;
        }
    }
    else if (symbolToTokenMap.contains(text)) type = symbolToTokenMap.at(text); //for '[', ']', ',', "+", "=", and ':'
    else if (std::regex_match(text, std::regex("^0x[0-9A-Fa-f]+$"))) type = TokenType::HEX;
    else if (std::regex_match(text, std::regex("^0b[01]+$"))) type = TokenType::BIN;
    else if (std::regex_match(text, std::regex("^[0-9]+$"))) type = TokenType::DECIMAL;
    else if (text[0] == '#') type = TokenType::COMMENT;
    else if (text[0] == '\n') type = TokenType::EOL;
    else if (text.length() > 1 && text.back() == ':') type = TokenType::LABEL; //needs trimming
    else if (text == ".const") type = TokenType::CONST; //needs trimming
    else if (std::ranges::all_of(text, [](char c) { return std::isalnum(c); })) type = TokenType::REF;
    return type;
}

assembler::OpCode::OpCode(Token token): token(std::move(token)) {
    //also symbolic support in the future
    resolution = Resolution::RESOLVED;
}

assembler::Operand::Operand(std::vector<Token> tokens) {

}


void assembler::TokenizedInstruction::validate() {
}

std::vector<assembler::TokenizedInstruction> assembler::TokenizedInstruction::resolve() {
}

parts::Instruction assembler::TokenizedInstruction::getLiteralInstruction() {
}