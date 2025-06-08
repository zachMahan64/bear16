//
// Created by Zachary Mahan on 5/23/25.
//
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <regex>
#include "assembly.h"

#define LOG(x) std::cout << x << std::endl
#define LOG_ERR(x) std::cerr << x << std::endl

//tools ---------------------------------------------------------------------------------------------------------
void assembly::asmMnemonicSetGenerator() {
    struct Instr {
        std::string base;
        uint16_t baseOpcode; // Ignored in this version
    };
    const std::vector<Instr> instructions = {
        {"add", 0x0000}, {"sub", 0x0001}, {"mult", 0x0002}, {"div", 0x0003}, {"mod", 0x0004},
        {"and", 0x0005}, {"or", 0x0006}, {"xor", 0x0007}, {"not", 0x0008}, {"nand", 0x0009},
        {"nor", 0x000A}, {"neg", 0x000B}, {"lsh", 0x000C}, {"rsh", 0x000D}, {"rol", 0x000E},
        {"ror", 0x000F}, {"eq", 0x0010}, {"ne", 0x0011}, {"lt", 0x0012}, {"le", 0x0013},
        {"gt", 0x0014}, {"ge", 0x0015}, {"ult", 0x0016}, {"ule", 0x0017}, {"ugt", 0x0018},
        {"uge", 0x0019}, {"z", 0x001A}, {"nz", 0x001B}, {"c", 0x001C}, {"nc", 0x001D},
        {"n", 0x001E}, {"nn", 0x001F}, {"mov", 0x0020}, {"lw", 0x0021}, {"lb", 0x0022},
        {"comp", 0x0023}, {"lea", 0x0024}, {"push", 0x0025}, {"pop", 0x0026}, {"clr", 0x0027},
        {"inc", 0x0028}, {"dec", 0x0029}, {"memcpy", 0x002A}, {"sw", 0x002B}, {"sb", 0x002C},
        {"lbrom", 0x002D}, {"lwrom", 0x002E},
        {"call", 0x0040}, {"ret", 0x0041}, {"jmp", 0x0042}, {"jcond_z", 0x0043},
        {"jcond_nz", 0x0044}, {"jcond_neg", 0x0045}, {"jcond_nneg", 0x0046}, {"jcond_pos", 0x0047},
        {"jcond_npos", 0x0048}, {"nop", 0x0049}, {"hlt", 0x004A}, {"jal", 0x004B}, {"retl", 0x004C},
        {"clrfb", 0x0080}, {"setpx", 0x0081}, {"blit", 0x0082},
    };

    std::vector<std::string> immSuffixes = {"", "i", "i1", "i2"};

    std::cout << "const std::unordered_set<std::string> validOpcodeMnemonics = {\n";
    for (const auto& instr : instructions) {
        for (const auto& suffix : immSuffixes) {
            std::cout << "    \"" << instr.base + suffix << "\",\n";
        }
    }
    std::cout << "};\n";
    std::cin.get();
}
//updated 5/28/2025
std::vector<uint8_t> assembly::Assembler::assembleFromFile(const std::string &path) const {
    std::vector<Token> allTokens = tokenizeAsmFirstPass(path);
    const auto allTokenizedInstructions = parseFirstPassIntoSecondPass(allTokens);
    auto literalInstructions = getLiteralInstructions(allTokenizedInstructions);
    return buildByteVecFromLiteralInstructions(literalInstructions);
}

//main passes ---------------------------------------------------------------------------------------------------------
std::vector<assembly::Token> assembly::Assembler::tokenizeAsmFirstPass(const std::string &path) {
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
        else if (symbolToTokenMap.contains(std::string(1, c))) {
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
std::vector<assembly::TokenizedInstruction> assembly::Assembler::parseFirstPassIntoSecondPass(const std::vector<Token> &tokens) const {
    //look-up tables
    std::unordered_map<std::string, uint16_t> labelMap = {};
    std::unordered_map<std::string, uint16_t> constMap = {};

    //break down into lines --> count lines for labels, resolve declarations and usages of labels/const --> form tokenized instructions
    std::vector<std::vector<Token>> tokenLines {};
    std::vector<Token> currentLine {};
    int lineNumInOrigAsm = 1;        //num lines on the editor
    int instructionIndex = 0;        //num instructions in the program
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
            instructionIndex++;
        }
        else if (tkn.type == TokenType::LABEL) {
            currentLine.push_back(tkn);
            labelMap.emplace(tkn.body.substr(0, tkn.body.length() - 1), instructionIndex * 8);
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
    std::vector<TokenizedInstruction> finalPassTokenizedInstructions {};
    //form instructions or some shit, resolve references
    int numLines = 0;
    int instructionIndex_ = 0;
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
            continue;
        }
        if (firstTknType == TokenType::CONST) {
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
            instructionIndex_++;
            TokenizedInstruction instrForThisLine = parseLineOfTokens(line, labelMap, constMap, instructionIndex_);
            finalPassTokenizedInstructions.emplace_back(instrForThisLine);
        } else {
            throwAFit(numLines);
            LOG_ERR(line.at(0).body + " (" + toString(line.at(0).type) + ") cannot begin a line");
        }
    }
    return finalPassTokenizedInstructions;
}
//sub section of pass two
assembly::TokenizedInstruction assembly::Assembler::parseLineOfTokens(const std::vector<Token> &line,
        const std::unordered_map<std::string, uint16_t> &labelMap,
        const std::unordered_map<std::string, uint16_t> &constMap,
        const int &instructionIndex
        ) const {
    //first token is the OPCODE!
    OpCode opCode(line.at(0));
    TokenizedInstruction instrForThisLine(opCode);

    std::vector<Token> revisedTokens {}; //use this to build finalized operands, also make a function for this
    //resolve refs
    for (int i = 1; i < line.size(); i++) {
        const Token& tkn = line.at(i);
        std::string revisedRef = tkn.body;
        if (tkn.type == TokenType::REF) {
            if (labelMap.contains(tkn.body)) {
                revisedRef = std::to_string(labelMap.at(tkn.body));
            } else if (constMap.contains(tkn.body)) {
                revisedRef = std::to_string(constMap.at(tkn.body));
            } else {
                throwAFit(tkn.body);
                LOG_ERR("ERROR: bad reference in instruction #" << + instructionIndex);
                LOG_ERR("label map: " + unorderedMapToString(labelMap));
                LOG_ERR("const map: " + unorderedMapToString(constMap));
            }
        }
        revisedTokens.emplace_back(revisedRef);
    }
    bool inPtr = false;
    bool inChar = false;
    std::vector<Operand> operands {};
    std::vector<Token> tokensForOperand {};
    Token prevTkn{'\0'};
    for (const Token &tkn : revisedTokens) {
        if (tkn.type == TokenType::LBRACKET) {
            inPtr = true;
            tokensForOperand.emplace_back(tkn);
        } else if (inPtr && tkn.type == TokenType::RBRACKET) {
            inPtr = false;
            tokensForOperand.emplace_back(tkn);
            operands.emplace_back(tokensForOperand);
            tokensForOperand.clear();
        } else if (inChar && tkn.type == TokenType::SING_QUOTE) {
            inChar = false;
            tokensForOperand.emplace_back(tkn);

            // If the char literal is just '', replace it with '\s'
            if (tokensForOperand.size() == 2 &&
                tokensForOperand[0].type == TokenType::SING_QUOTE &&
                tokensForOperand[1].type == TokenType::SING_QUOTE) {

                tokensForOperand = {
                    Token("'"),
                    Token("\\s"),
                    Token("'")
                };
                LOG("Empty char literal detected. Assumed a space was written or intended -> substituted with \\s");
                }

            operands.emplace_back(tokensForOperand);
            tokensForOperand.clear();
        } else if (tkn.type == TokenType::SING_QUOTE) {
            inChar = true;
            tokensForOperand.emplace_back(tkn);
        } else if (inPtr || inChar) {
            tokensForOperand.emplace_back(tkn);
        } else {
            tokensForOperand.emplace_back(tkn);
            operands.emplace_back(tokensForOperand);
            tokensForOperand.clear();
        }
    }
    instrForThisLine.setOperandsAndAutocorrectImmediates(doNotAutoCorrectImmediates, operands);
    return instrForThisLine;
}

std::vector<parts::Instruction> assembly::Assembler::getLiteralInstructions(const std::vector<TokenizedInstruction>& tknInstructions) {
    std::vector<parts::Instruction> literalInstructions {};
    literalInstructions.reserve(tknInstructions.size());
    for (const TokenizedInstruction &tknInstr : tknInstructions) {
        parts::Instruction litInstr = tknInstr.getLiteralInstruction();
        printLiteralInstruction(litInstr); //for debug
        literalInstructions.emplace_back(litInstr);

    }
    return literalInstructions;
}

void assembly::Assembler::printLiteralInstruction(const parts::Instruction &litInstr) {
    std::cout << "INSTR (DEBUG):\n";
    std::cout << "Opcode: " << std::hex << std::setw(4) << std::setfill('0') << litInstr.opcode << "\n";
    std::cout << "Imm: " << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(litInstr.immFlags) << "\n";
    std::cout << "Op14: " << std::hex << std::setw(4) << std::setfill('0') << litInstr.opCode14 << "\n";
    std::cout << "dest: " << std::hex << std::setw(4) << std::setfill('0') << litInstr.dest << "\n";
    std::cout << "src1: " << std::hex << std::setw(4) << std::setfill('0') << litInstr.src1 << "\n";
    std::cout << "src2: " << std::hex << std::setw(4) << std::setfill('0') << litInstr.src2 << "\n";
    std::cout << "\n";
} //debug

std::vector<uint8_t> assembly::Assembler::buildByteVecFromLiteralInstructions(const std::vector<parts::Instruction>& literalInstructions) {
    std::vector<uint8_t> byteVec;
    byteVec.reserve(literalInstructions.size() * 8);

    for (const auto& instr : literalInstructions) {
        std::vector<uint8_t> vec8Byte = build8ByteVecFromSingleLiteralInstruction(instr);
        byteVec.insert(byteVec.end(), vec8Byte.begin(), vec8Byte.end());
    }

    return byteVec;
}
std::vector<uint8_t> assembly::Assembler::build8ByteVecFromSingleLiteralInstruction(const parts::Instruction& literalInstruction) {
    const uint64_t raw = literalInstruction.toRaw();
    //std::cout << "Raw Instr (debug): " << std::hex << std::setw(16) << std::setfill('0') << raw << "\n";
    std::vector<uint8_t> byteVec {};
    byteVec.reserve(8);
    for (int i = 7; i >= 0; --i) {
        uint8_t byte = (raw >> (8 * i)) & 0xFF;
        //std::cout << "Raw Byte (debug): " << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << "\n";
        byteVec.emplace_back(byte);
    }
    return byteVec;
}

//helpers----------------------------------------------------------------------------------------------------------------
assembly::TokenType assembly::Token::deduceTokenType(const std::string &text) {
    TokenType type = TokenType::MISTAKE;
    if (validOpcodeMnemonics.contains(text)) {
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
    else if (std::regex_match(text, std::regex("^[-+]?[0-9]+$"))) type = TokenType::DECIMAL;
    else if (text[0] == '#') type = TokenType::COMMENT;
    else if (text[0] == '\n') type = TokenType::EOL;
    else if (text.length() == 1 && (std::isalpha(text[0]) || validSymbols.contains(text[0]))) type = TokenType::CHAR;
    else if (text == "\\s") type = TokenType::CHAR_SPACE;
    else if (text.length() > 1 && text.back() == ':') type = TokenType::LABEL; //needs trimming?
    else if (text == ".const") type = TokenType::CONST;
    else if (std::ranges::all_of(text, [](char c) { return (std::isalnum(c) || c == '_'); })) type = TokenType::REF;
    return type;
}
std::pair<std::string, std::string> assembly::splitOpcodeStr(std::string opcodeStr) {
    char lastChar = opcodeStr.back();
    if (lastChar == 'i') {
        return {opcodeStr.substr(0, opcodeStr.length() - 1), "i"};
    }
    if (lastChar == '1') {
        return {opcodeStr.substr(0, opcodeStr.length() - 2), "i1"};
    }
    if (lastChar == '2') {
        return {opcodeStr.substr(0, opcodeStr.length() - 2), "i2"};
    }
    return {opcodeStr, ""};
}
void assembly::TokenizedInstruction::setOperandsAndAutocorrectImmediates(const bool& doNotAutoCorrectImmediates, std::vector<Operand> operands) {
    const isa::Opcode_E &opE = opcode.opcode_e;
    bool opHasNoWritImm = opcode.immType == ImmType::NO_IM;
    try {
        if (operands.size() < opcodeToOperandMinimumCountMap.at(opE)) {
            LOG_ERR("ERROR: too few of operands for opcode: " + opcode.token.body);
            return;
        }
    } catch (std::out_of_range &e) {
        LOG_ERR("ERROR: opcode undefined in \"opcodeToOperandMinimumCountMap\" - " + opcode.token.body);
        return;
    }
    if (operands.size() > opcodeToOperandMinimumCountMap.at(opE)
        && !(opCodesWithOptionalSrc2OffsetArgument.contains(opE) || opCodesWithOptionalSrc1OffsetArgument.contains(opE))) {
        LOG_ERR("ERROR: too many operands for opcode: " + opcode.token.body);
        return;
        }
    if (operands.size() == 3) {
        dest = operands.at(0);
        src1 = operands.at(1);
        src2 = operands.at(2);
    } else if (operands.size() == 2 && opCodesWithOptionalSrc1OffsetArgument.contains(opE)) {
        dest = operands.at(0);
        src2 = operands.at(1);
    } else if (operands.size() == 2) {
        dest = operands.at(0);
        src1 = operands.at(1);
    } else if (operands.size() == 1 && opE == isa::Opcode_E::PUSH) {
        src1 = operands.at(0);
    } else if (operands.size() == 1) {
        dest = operands.at(0);
    }
    //set imm
    if (doNotAutoCorrectImmediates) return;
    if (src1 && opHasNoWritImm && src1->valueType == ValueType::IMM) {
        LOG("Corrected imm to i1 for " + opcode.token.body);
        i1 = true;
    } if (src2 && opHasNoWritImm && src2->valueType == ValueType::IMM) {
        LOG("Corrected imm to i2 for " + opcode.token.body);
        i2 = true;
    }
    if (!src1) {
        LOG("Corrected imm to i1 for (absent operand)" + opcode.token.body);
        i1 = true;
    }
    if (!src2) {
        LOG("Corrected imm to i2 for (absent operand)" + opcode.token.body);
        i2 = true;
    } if (opcode.immType == ImmType::I) {
        i1 = true;
        i2 = true;
    } else if (opcode.immType == ImmType::I1) {
        i1 = true;
    } else if (opcode.immType == ImmType::I2) {
        i2 = true;
    }

} //WIP

//constructors for tokenized instruction components ----------------------------------------------------------------------
assembly::OpCode::OpCode(Token token): token(std::move(token)) {
    resolution = Resolution::UNRESOLVED;
    auto opCodeStrSplit = splitOpcodeStr(this->token.body);
    if (!stringToOpcodeMap.contains(opCodeStrSplit.first)) {
        LOG_ERR("Unknown opcode: " + opCodeStrSplit.first + " (" + toString(token.type) + ", " + this->token.body + ")");
        throw std::runtime_error("Unknown opcode: -" + this->token.body + "-");
    }
    opcode_e = stringToOpcodeMap.at(opCodeStrSplit.first);
    if (opCodeStrSplit.second == "i") {
        immType = ImmType::I;
    } else if (opCodeStrSplit.second == "i1") {
        immType = ImmType::I1;
    } else if (opCodeStrSplit.second == "i2") {
        immType = ImmType::I2; // Fix here!
    } else {
        immType = ImmType::NO_IM;
    }
}
assembly::Operand::Operand(std::vector<Token> tokens) : tokens(std::move(tokens)), significantToken('\0') {
    significantToken = this->tokens.at(0);
    //also symbolic support in the future
    resolution = Resolution::RESOLVED;
    if (this->tokens.size() == 1) {
        significantBody = this->tokens.at(0).body;
        fullBody = significantBody;
    } else {
        fullBody = "";
        for (Token &tkn: this->tokens) {
            fullBody += tkn.body;
        }
        significantBody = fullBody.substr(1, fullBody.length() - 2);
        Token sigTkn(significantBody);
        significantToken = sigTkn;
        //handle edge cases
        if (significantToken.type == TokenType::OPERATION) {
            significantToken.type = TokenType::CHAR;
        } //handles the weird edge case of single letter operations!
        else if (significantToken.type == TokenType::CONST || significantToken.type == TokenType::LABEL) {
            LOG_ERR("MISUSED DECLARATION: " + significantBody + " (" + toString(significantToken.type));
        }
    }
    //set imm/named delineation for operand
    if (significantToken.type == TokenType::HEX
        || significantToken.type == TokenType::BIN
        || significantToken.type == TokenType::DECIMAL
        || significantToken.type == TokenType::CHAR
        || significantToken.type == TokenType::CHAR_SPACE)
    {
        valueType = ValueType::IMM;
    } else {
        valueType = ValueType::NAMED;
    }
    if (!validOperandArguments.contains(significantToken.type)) {
        LOG_ERR("INVALID OPERAND: " + significantBody + " (" + toString(significantToken.type) + ")");
    }
    LOG("full body:" + fullBody + ", significant body: " + significantBody + ", significant type: " + toString(significantToken.type));
}

//methods for resolving tokenized instr into literals----------------------------------------------------------------------
uint16_t assembly::Operand::resolveInto16BitLiteral() const {
    if (namedOperandToBinMap.contains(significantBody)) {
        return namedOperandToBinMap.at(significantBody);
    }
    if (significantToken.type == TokenType::HEX) {
        return std::stoi(significantBody.substr(2), nullptr, 16);
    }
    if (significantToken.type == TokenType::DECIMAL) {
        return std::stoi(significantBody, nullptr, 10);
    }
    if (significantToken.type == TokenType::BIN) {
        return std::stoi(significantBody.substr(2), nullptr, 2);
    }
    if (significantToken.type == TokenType::CHAR) {
        return static_cast<uint16_t>(significantBody.at(0));
    }
    if (significantToken.type == TokenType::CHAR_SPACE) {
        return 32; //value of space character
    }
    return {};
}

parts::Instruction assembly::TokenizedInstruction::getLiteralInstruction() const {
    parts::Instruction thisInstr{};
    thisInstr.opCode14 = static_cast<uint16_t>(opcode.opcode_e);
    uint8_t i1 = this->i1 ? 1 : 0;
    uint8_t i2 = this->i2 ? 1 : 0;
    thisInstr.immFlags = i1 << 1 | i2;
    thisInstr.opcode = thisInstr.immFlags << 14 | thisInstr.opCode14;
    if (dest) {
        thisInstr.dest = dest->resolveInto16BitLiteral();
    } else {
        thisInstr.dest = 0;
    }
    if (src1) {
        thisInstr.src1 = src1->resolveInto16BitLiteral();
    } else {
        thisInstr.src1 = 0;
    }
    if (src2) {
        thisInstr.src2 = src2->resolveInto16BitLiteral();
    } else {
        thisInstr.src2 = 0;
    }
    return thisInstr;

}

void assembly::TokenizedInstruction::resolve() {
    opcode.resolution = Resolution::RESOLVED;
} // (largely WIP)





