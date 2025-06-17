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
#include "core.h"
#include "assembly.h"
#define LOG(x) std::cout << std::dec << x << std::endl
#define LOG_ERR(x) std::cerr << std::dec << x << std::endl
// variant definition
namespace assembly {
    using TokenizedRomLine = std::variant<TokenizedInstruction, TokenizedData>;

    //maps & sets ----------------------------------------------------------------------------------------------------------
    const std::unordered_set<std::string> validOpcodeMnemonics = {
    "add",
    "addi",
    "addi1",
    "addi2",
    "sub",
    "subi",
    "subi1",
    "subi2",
    "mult",
    "multi",
    "multi1",
    "multi2",
    "div",
    "divi",
    "divi1",
    "divi2",
    "mod",
    "modi",
    "modi1",
    "modi2",
    "and",
    "andi",
    "andi1",
    "andi2",
    "or",
    "ori",
    "ori1",
    "ori2",
    "xor",
    "xori",
    "xori1",
    "xori2",
    "not",
    "noti",
    "noti1",
    "noti2",
    "nand",
    "nandi",
    "nandi1",
    "nandi2",
    "nor",
    "nori",
    "nori1",
    "nori2",
    "neg",
    "negi",
    "negi1",
    "negi2",
    "lsh",
    "lshi",
    "lshi1",
    "lshi2",
    "rsh",
    "rshi",
    "rshi1",
    "rshi2",
    "rol",
    "roli",
    "roli1",
    "roli2",
    "ror",
    "rori",
    "rori1",
    "rori2",
    "eq",
    "eqi",
    "eqi1",
    "eqi2",
    "ne",
    "nei",
    "nei1",
    "nei2",
    "lt",
    "lti",
    "lti1",
    "lti2",
    "le",
    "lei",
    "lei1",
    "lei2",
    "gt",
    "gti",
    "gti1",
    "gti2",
    "ge",
    "gei",
    "gei1",
    "gei2",
    "ult",
    "ulti",
    "ulti1",
    "ulti2",
    "ule",
    "ulei",
    "ulei1",
    "ulei2",
    "ugt",
    "ugti",
    "ugti1",
    "ugti2",
    "uge",
    "ugei",
    "ugei1",
    "ugei2",
    "z",
    "zi",
    "zi1",
    "zi2",
    "nz",
    "nzi",
    "nzi1",
    "nzi2",
    "c",
    "ci",
    "ci1",
    "ci2",
    "nc",
    "nci",
    "nci1",
    "nci2",
    "n",
    "ni",
    "ni1",
    "ni2",
    "nn",
    "nni",
    "nni1",
    "nni2",
    "mov",
    "movi",
    "movi1",
    "movi2",
    "lw",
    "lwi",
    "lwi1",
    "lwi2",
    "lb",
    "lbi",
    "lbi1",
    "lbi2",
    "comp",
    "compi",
    "compi1",
    "compi2",
    "lea",
    "leai",
    "leai1",
    "leai2",
    "push",
    "pushi",
    "pushi1",
    "pushi2",
    "pop",
    "popi",
    "popi1",
    "popi2",
    "clr",
    "clri",
    "clri1",
    "clri2",
    "inc",
    "inci",
    "inci1",
    "inci2",
    "dec",
    "deci",
    "deci1",
    "deci2",
    "memcpy",
    "memcpyi",
    "memcpyi1",
    "memcpyi2",
    "sw",
    "swi",
    "swi1",
    "swi2",
    "sb",
    "sbi",
    "sbi1",
    "sbi2",
    "lbrom",
    "lbromi",
    "lbromi1",
    "lbromi2",
    "lwrom",
    "lwromi",
    "lwromi1",
    "lwromi2",
    "romcpy",
    "romcpyi",
    "romcpyi1",
    "romcpyi2",
    "mults",
    "multsi",
    "multsi1",
    "multsi2",
    "divs",
    "divsi",
    "divsi1",
    "divsi2",
    "mods",
    "modsi",
    "modsi1",
    "modsi2",
    "call",
    "calli",
    "calli1",
    "calli2",
    "ret",
    "reti",
    "reti1",
    "reti2",
    "jmp",
    "jmpi",
    "jmpi1",
    "jmpi2",
    "jcond_z",
    "jcond_zi",
    "jcond_zi1",
    "jcond_zi2",
    "jcond_nz",
    "jcond_nzi",
    "jcond_nzi1",
    "jcond_nzi2",
    "jcond_neg",
    "jcond_negi",
    "jcond_negi1",
    "jcond_negi2",
    "jcond_nneg",
    "jcond_nnegi",
    "jcond_nnegi1",
    "jcond_nnegi2",
    "jcond_pos",
    "jcond_posi",
    "jcond_posi1",
    "jcond_posi2",
    "jcond_npos",
    "jcond_nposi",
    "jcond_nposi1",
    "jcond_nposi2",
    "nop",
    "nopi",
    "nopi1",
    "nopi2",
    "hlt",
    "hlti",
    "hlti1",
    "hlti2",
    "jal",
    "jali",
    "jali1",
    "jali2",
    "retl",
    "retli",
    "retli1",
    "retli2",
    "clrfb",
    "clrfbi",
    "clrfbi1",
    "clrfbi2",
    "setpx",
    "setpxi",
    "setpxi1",
    "setpxi2",
    "blit",
    "bliti",
    "bliti1",
    "bliti2",};
    const std::unordered_map<std::string, uint16_t> namedOperandToBinMap = {
        // temporaries (volatile)
        {"r0", 0}, {"t0", 0},
        {"r1", 1}, {"t1", 1},
        {"r2", 2}, {"t2", 2},
        {"r3", 3}, {"t3", 3},
        {"r4", 4}, {"t4", 4},
        {"r5", 5}, {"t5", 5},
        {"r6", 6}, {"t6", 6},
        {"r7", 7}, {"t7", 7},
        {"r8", 8}, {"t8", 8},
        {"r9", 9}, {"t9", 9},
        {"r10", 10}, {"t10", 10},

        // saved registers
        {"r11", 11}, {"s0", 11},
        {"r12", 12}, {"s1", 12},
        {"r13", 13}, {"s2", 13},
        {"r14", 14}, {"s3", 14},
        {"r15", 15}, {"s4", 15},
        {"r16", 16}, {"s5", 16},
        {"r17", 17}, {"s6", 17},
        {"r18", 18}, {"s7", 18},
        {"r19", 19}, {"s8", 19},
        {"r20", 20}, {"s9", 20},
        {"r21", 21}, {"s10", 21},

        // return value and arguments
        {"r22", 22}, {"rv", 22},
        {"r23", 23}, {"a0", 23},
        {"r24", 24}, {"a1", 24},
        {"r25", 25}, {"a2", 25},

        // return address
        {"r26", 26}, {"ra", 26},

        // io ports
        {"r27", 27}, {"io0", 27},
        {"r28", 28}, {"io1", 28},

        // special-purpose
        {"r29", 29}, {"sp", 29},
        {"r30", 30}, {"fp", 30},
        {"r31", 31}, {"pc", 31}};
    const std::vector<std::string> namedOperandsInOrder {
        // temporaries (volatile)
        "r0", "t0",
        "r1", "t1",
        "r2", "t2",
        "r3", "t3",
        "r4", "t4",
        "r5", "t5",
        "r6", "t6",
        "r7", "t7",
        "r8", "t8",
        "r9", "t9",
        "r10", "t10",

        // saved registers
        "r11", "s0",
        "r12", "s1",
        "r13", "s2",
        "r14", "s3",
        "r15", "s4",
        "r16", "s5",
        "r17", "s6",
        "r18", "s7",
        "r19", "s8",
        "r20", "s9",
        "r21", "s10",

        // return value and arguments
        "r22", "rv",
        "r23", "a0",
        "r24", "a1",
        "r25", "a2",

        // return address
        "r26", "ra",

        // io ports
        "r27", "io0",
        "r28", "io1",

        // special-purpose
        "r29", "sp",
        "r30", "fp",
        "r31", "pc"
    };

    const std::unordered_map<std::string, isa::Opcode_E> stringToOpcodeMap = {
        // Arith & Bitwise
        {"add", isa::Opcode_E::ADD}, {"sub", isa::Opcode_E::SUB}, {"mult", isa::Opcode_E::MULT},
        {"div", isa::Opcode_E::DIV}, {"mod", isa::Opcode_E::MOD}, {"and", isa::Opcode_E::AND},
        {"or", isa::Opcode_E::OR}, {"xor", isa::Opcode_E::XOR}, {"not", isa::Opcode_E::NOT},
        {"nand", isa::Opcode_E::NAND}, {"nor", isa::Opcode_E::NOR}, {"neg", isa::Opcode_E::NEG},
        {"lsh", isa::Opcode_E::LSH}, {"rsh", isa::Opcode_E::RSH}, {"rol", isa::Opcode_E::ROL},
        {"ror", isa::Opcode_E::ROR},

        // Cond
        {"eq", isa::Opcode_E::EQ}, {"ne", isa::Opcode_E::NE}, {"lt", isa::Opcode_E::LT},
        {"le", isa::Opcode_E::LE}, {"gt", isa::Opcode_E::GT}, {"ge", isa::Opcode_E::GE},
        {"ult", isa::Opcode_E::ULT}, {"ule", isa::Opcode_E::ULE}, {"ugt", isa::Opcode_E::UGT},
        {"uge", isa::Opcode_E::UGE}, {"z", isa::Opcode_E::Z}, {"nz", isa::Opcode_E::NZ},
        {"c", isa::Opcode_E::C}, {"nc", isa::Opcode_E::NC}, {"n", isa::Opcode_E::N},
        {"nn", isa::Opcode_E::NN},

        // Data Trans
        {"mov", isa::Opcode_E::MOV}, {"lw", isa::Opcode_E::LW}, {"lb", isa::Opcode_E::LB},
        {"comp", isa::Opcode_E::COMP}, {"lea", isa::Opcode_E::LEA}, {"push", isa::Opcode_E::PUSH},
        {"pop", isa::Opcode_E::POP}, {"clr", isa::Opcode_E::CLR}, {"inc", isa::Opcode_E::INC},
        {"dec", isa::Opcode_E::DEC}, {"memcpy", isa::Opcode_E::MEMCPY}, {"sw", isa::Opcode_E::SW},
        {"sb", isa::Opcode_E::SB}, {"lbrom", isa::Opcode_E::LBROM}, {"lwrom", isa::Opcode_E::LWROM},
        {"romcpy", isa::Opcode_E::ROMCPY}, {"mults", isa::Opcode_E::MULTS}, {"divs", isa::Opcode_E::DIVS},
        {"mods", isa::Opcode_E::MODS},

        // Ctrl Flow
        {"call", isa::Opcode_E::CALL}, {"ret", isa::Opcode_E::RET}, {"jmp", isa::Opcode_E::JMP},
        {"jcond_z", isa::Opcode_E::JCOND_Z}, {"jcond_nz", isa::Opcode_E::JCOND_NZ},
        {"jcond_neg", isa::Opcode_E::JCOND_NEG}, {"jcond_nneg", isa::Opcode_E::JCOND_NNEG},
        {"jcond_pos", isa::Opcode_E::JCOND_POS}, {"jcond_npos", isa::Opcode_E::JCOND_NPOS},
        {"nop", isa::Opcode_E::NOP}, {"hlt", isa::Opcode_E::HLT}, {"jal", isa::Opcode_E::JAL},
        {"retl", isa::Opcode_E::RETL},

        // Video
        {"clrfb", isa::Opcode_E::CLRFB}, {"setpx", isa::Opcode_E::SETPX}, {"blit", isa::Opcode_E::BLIT}};
    const std::unordered_map<isa::Opcode_E, int> opcodeToOperandMinimumCountMap = {
        // Arith & Bitwise
{isa::Opcode_E::ADD, 3}, {isa::Opcode_E::SUB, 3}, {isa::Opcode_E::MULT, 3},
        {isa::Opcode_E::DIV, 3}, {isa::Opcode_E::MOD, 3}, {isa::Opcode_E::AND, 3},
        {isa::Opcode_E::OR, 3}, {isa::Opcode_E::XOR, 3}, {isa::Opcode_E::NOT, 2},
        {isa::Opcode_E::NAND, 3}, {isa::Opcode_E::NOR, 3}, {isa::Opcode_E::NEG, 2},
        {isa::Opcode_E::LSH, 3}, {isa::Opcode_E::RSH, 3}, {isa::Opcode_E::ROL, 3},
        {isa::Opcode_E::ROR, 3}, {isa::Opcode_E::MULTS, 3}, {isa::Opcode_E::DIVS, 3},
        {isa::Opcode_E::MODS, 3},

        // Cond
        {isa::Opcode_E::EQ, 3}, {isa::Opcode_E::NE, 3}, {isa::Opcode_E::LT, 3},
        {isa::Opcode_E::LE, 3}, {isa::Opcode_E::GT, 3}, {isa::Opcode_E::GE, 3},
        {isa::Opcode_E::ULT, 3}, {isa::Opcode_E::ULE, 3}, {isa::Opcode_E::UGT, 3},
        {isa::Opcode_E::UGE, 3}, {isa::Opcode_E::Z, 0}, {isa::Opcode_E::NZ, 0},
        {isa::Opcode_E::C, 0}, {isa::Opcode_E::NC, 0}, {isa::Opcode_E::N, 0},
        {isa::Opcode_E::NN, 0},

        // Data Trans
        {isa::Opcode_E::MOV, 2},
        {isa::Opcode_E::LW, 2}, {isa::Opcode_E::LB, 2}, //(3) but offset
        {isa::Opcode_E::COMP, 2},
        {isa::Opcode_E::LEA, 2}, //(3) but offset
        {isa::Opcode_E::PUSH, 1}, {isa::Opcode_E::POP, 1},
        {isa::Opcode_E::CLR, 1}, {isa::Opcode_E::INC, 1}, {isa::Opcode_E::DEC, 1}, //dest only
        {isa::Opcode_E::MEMCPY, 3}, {isa::Opcode_E::ROMCPY, 3},
        {isa::Opcode_E::SW, 2}, {isa::Opcode_E::SB, 2}, //(3) but offset (in src1!)
        {isa::Opcode_E::LWROM, 2}, {isa::Opcode_E::LBROM, 2}, //(3) but offset

        // Ctrl Flow
        {isa::Opcode_E::CALL, 1}, {isa::Opcode_E::RET, 0}, {isa::Opcode_E::JMP, 1},
        {isa::Opcode_E::JCOND_Z, 1}, {isa::Opcode_E::JCOND_NZ, 1},
        {isa::Opcode_E::JCOND_NEG, 1}, {isa::Opcode_E::JCOND_NNEG, 1},
        {isa::Opcode_E::JCOND_POS, 1}, {isa::Opcode_E::JCOND_NPOS, 1},
        {isa::Opcode_E::NOP, 0}, {isa::Opcode_E::HLT, 0}, {isa::Opcode_E::JAL, 1},
        {isa::Opcode_E::RETL, 0},

        // Video
        {isa::Opcode_E::CLRFB, 0}, {isa::Opcode_E::SETPX, 3}, {isa::Opcode_E::BLIT, 3}
    };
    const std::unordered_set<isa::Opcode_E> opCodesWithOptionalSrc2OffsetArgument = {
        isa::Opcode_E::LW,
        isa::Opcode_E::LB,
        isa::Opcode_E::LBROM,
        isa::Opcode_E::LWROM,
        isa::Opcode_E::LEA
        };
    const std::unordered_set<isa::Opcode_E> opCodesWithOptionalSrc1OffsetArgument = {
        isa::Opcode_E::SW, isa::Opcode_E::SB
    };
    const std::unordered_set<char> validSymbols = {
        '!', '@', '/', '\\', '$', '%', '&', '^', '*', '(', ')', '\'', '~', '-', '\0', ':', ';', '[', ']', ' ', '{', '}', '?',
        '=', '|', ',', '.', '+', '<',
    };
    const std::unordered_map<std::string, TokenType> stringToDataDirectives = {
        {".string", TokenType::STRING_DIR},
        {".byte", TokenType::BYTE_DIR},
        {".octbyte", TokenType::OCTBYTE_DIR},
        {".word", TokenType::WORD_DIR},
        {".qword", TokenType::QWORD_DIR}};
    const std::unordered_map<TokenType, size_t> dataDirectivesToSizeMap {
            {TokenType::STRING_DIR, 0}, //default, must be manually adjusted
            {TokenType::BYTE_DIR, 1},
            {TokenType::OCTBYTE_DIR, 1},
            {TokenType::WORD_DIR, 2},
            {TokenType::QWORD_DIR, 2}
    };
    const std::unordered_map<TokenType, size_t> fixedDirectivesToSizeMap {
                {TokenType::OCTBYTE_DIR, 8},
                {TokenType::QWORD_DIR, 4}
    };
    const std::unordered_set<TokenType> validDataTokenTypes {
        TokenType::HEX, TokenType::BIN, TokenType::DECIMAL, TokenType::CHAR, TokenType::CHAR_SPACE
    };
    //tools ----------------------------------------------------------------------------------------------------------------
    void asmMnemonicSetGenerator() {
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
            {"lbrom", 0x002D}, {"lwrom", 0x002E}, {"romcpy", 0x002F}, {"mults", 0x0030}, {"divs", 0x0031},
            {"mods", 0x0032},
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
    //assembler class
    Assembler::Assembler(bool enableDebug, bool doNotAutoCorrectImmediates)
        : isEnableDebug(enableDebug), doNotAutoCorrectImmediates(doNotAutoCorrectImmediates) {};
    std::vector<uint8_t> Assembler::assembleFromFile(const std::string &path) const {
        std::vector<Token> allTokens = tokenizeAsmFirstPass(path);
        std::vector<TokenizedRomLine> allTokenizedInstructions = parseListOfTokensIntoTokenizedRomLines(allTokens);
        std::vector<parts::Instruction> literalInstructions = getLiteralInstructions(allTokenizedInstructions);
        std::vector<uint8_t> byteVec = buildByteVecFromLiteralInstructions(literalInstructions);
        std::vector<uint8_t> dataByteVec = parseTokenizedDataIntoByteVec(allTokenizedInstructions);
        byteVec.insert(byteVec.end(), dataByteVec.begin(), dataByteVec.end());
        LOG("Final ROM Size (bytes): " << std::dec << byteVec.size());
        return byteVec;
    }

    //main passes ----------------------------------------------------------------------------------------------------------
    //1st
    std::vector<Token> Assembler::tokenizeAsmFirstPass(const std::string &path) {
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
        bool inString = false;
        bool inChar = false;

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
            if (inString) {
                if (c == '"') {
                    inString = false;
                    firstPassTokens.emplace_back(currentStr);
                    firstPassTokens.emplace_back('"');
                    currentStr.clear();
                    continue;
                }
                currentStr += c;
                continue;
            }
            if (c == '"') {
                inString = true;
                firstPassTokens.emplace_back(c);
                continue;
            }
            if (inChar) {
                if (c == '\'') {
                    inChar = false;
                    if (currentStr.length() > 1) {
                        LOG_ERR("ERROR: invalid char declaration (length greater than 1 char)");
                    }
                    Token charTkn(currentStr);
                    charTkn.type = TokenType::CHAR;
                    firstPassTokens.emplace_back(charTkn);
                    firstPassTokens.emplace_back('\'');
                    currentStr.clear();
                    continue;
                }
                currentStr += c;
                continue;
            }
            if (c == '\'') {
                inChar = true;
                firstPassTokens.emplace_back(c);
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
    //2nd
    std::vector<TokenizedRomLine> Assembler::parseListOfTokensIntoTokenizedRomLines(
        const std::vector<Token> &tokens) const {
        //look-up tables
        std::unordered_map<std::string, uint16_t> labelMap = {};
        std::unordered_map<std::string, uint16_t> constMap = {};
        //break down into lines --> count lines for labels, resolve declarations and usages of labels/const --> form tokenized instructions
        std::vector<std::vector<Token>> tokenLines_TEXT {};
        std::vector<Token> currentLine_TEXT {};
        std::vector<std::vector<Token>> tokenLines_DATA {};
        std::vector<Token> currentLine_DATA {};
        int lineNumInOrigAsm = 1;        //num lines on the editor
        //form lines of tokens to be made into instr
        bool inText = false;
        bool inData = false;
        int byteIndex  = 0;
        for (const Token &tkn: tokens) {
            if (tkn.type == TokenType::TEXT) {
                inText = true;
                inData = false;
                continue;
            }
            if (tkn.type == TokenType::DATA) {
                inData = true;
                inText = false;
                continue;
            }
            if (!inText && !inData && (tkn.type != TokenType::EOL && tkn.type != TokenType::COMMENT)) {
                LOG_ERR("NOT IN TEXT OR DATA SECTION: " + toString(tkn.type) + "-" + tkn.body);
            }
            else if (tkn.type == TokenType::MISTAKE) {
                throwAFit(lineNumInOrigAsm);
                LOG_ERR("ERROR: MISTAKE -> " + tkn.body);
                if (inText) currentLine_TEXT.push_back(tkn);
                if (inData) currentLine_DATA.push_back(tkn);
            }
            else if (tkn.type == TokenType::EOL) {
                lineNumInOrigAsm++;
                if (inText) {
                    if (!currentLine_TEXT.empty()) {
                        tokenLines_TEXT.push_back(currentLine_TEXT);
                        currentLine_TEXT.clear();
                    }
                }
                if (inData) {
                    if (!currentLine_DATA.empty()) {
                        tokenLines_DATA.push_back(currentLine_DATA);
                        currentLine_DATA.clear();
                    }
                }
            }
            else if (tkn.type == TokenType::OPERATION) {
                Token newTkn(tkn.body);
                if (inData && tkn.body.length() != 1) {
                    LOG_ERR("WARNING: instruction in data section: " + tkn.body + " on written line " + std::to_string(lineNumInOrigAsm));
                } else {
                    if (tkn.body.length() == 1) {
                        newTkn.type = TokenType::CHAR;
                    }
                }
                if (inText) currentLine_TEXT.push_back(newTkn);
                if (inData) {
                    currentLine_DATA.push_back(newTkn);
                }
            }
            else if (tkn.type != TokenType::COMMENT && tkn.type != TokenType::COMMA) {
                if (inText) currentLine_TEXT.push_back(tkn);
                if (inData) currentLine_DATA.push_back(tkn);
            } else {
                LOG("IGNORED A TOKEN TYPE (2nd pass):" + toString(tkn.type));
            }
        }
        //handle hanging lines
        if (!currentLine_TEXT.empty()) {
            tokenLines_TEXT.push_back(currentLine_TEXT);
        }
        //handle hanging lines
        if (!currentLine_DATA.empty()) {
            tokenLines_DATA.push_back(currentLine_DATA);
        }

        std::vector<std::vector<Token>> tokenLines_TEXT_and_DATA {};
        tokenLines_TEXT_and_DATA.reserve(tokenLines_TEXT.size() + tokenLines_DATA.size());
        for (const std::vector<Token> &line : tokenLines_TEXT) {
            tokenLines_TEXT_and_DATA.push_back(line);
        }
        for (const std::vector<Token> &line : tokenLines_DATA) {
            tokenLines_TEXT_and_DATA.push_back(line);
        }

        //IMP: MUST UPDATE HERE W/ LABEL RES FOR DATA DIR
        int byteNum  = 0;
        for (std::vector<Token> &line: tokenLines_TEXT_and_DATA) {
            Token firstTkn = line.at(0);
            if (firstTkn.type == TokenType::OPERATION) {
                byteNum += 8;
            }
            else if (firstTkn.type == TokenType::LABEL) {
                std::string labelName = firstTkn.body.substr(0, firstTkn.body.length() - 1);
                const int& labelValue = byteNum;
                labelMap.emplace(labelName, labelValue);
                LOG("placed label " << labelName << " at " << std::hex << labelValue);
            }
            else if (containsValue(stringToDataDirectives, firstTkn.type)) {
                if (firstTkn.type == TokenType::STRING_DIR) {
                    try {
                        if (line.at(1).type == TokenType::DOUB_QUOTE && line.at(line.size()-1).type == TokenType::DOUB_QUOTE) {
                            std::vector<Token> fixedStrLine {};
                            fixedStrLine.emplace_back(line.at(0));
                            fixedStrLine.emplace_back(line.at(1));
                            std::string newStrBuffer {};
                            for (int i = 2; i < line.size()-1; i++) {
                                newStrBuffer += line.at(i).body;
                            }
                            Token newStrTkn(newStrBuffer);
                            newStrTkn.type = TokenType::STRING;
                            fixedStrLine.emplace_back(newStrTkn);
                            fixedStrLine.emplace_back(line.at(line.size()-1));
                            line = fixedStrLine;
                            LOG ("REVISING STRING");
                            for (const Token &tkn : line) {
                                std::cout << toString(tkn.type) << "-" << tkn.body << " | ";
                            }
                            std::cout << std::endl;
                            byteNum += static_cast<int>(newStrTkn.body.length()) + 1; //+1 for null terminator
                        }
                        else {
                            LOG_ERR("ERROR: invalid string declaration at byte index: " << byteNum <<  std::endl);
                            for (const Token &tkn : line) {
                                std::cout << tkn.body << " | ";
                            }
                        }
                    } catch (std::out_of_range &e) {
                        LOG_ERR("ERROR: invalid string declaration at byte index: ") << byteNum << " | " <<  std::endl;
                        for (const Token &tkn : line) {
                            std::cout << tkn.body << " | ";
                        }
                    }
                } else {
                    int startingByteNum = byteNum;
                    for (const Token &tkn : line) {
                        if (validDataTokenTypes.contains(tkn.type)) {
                            byteNum += static_cast<int>(dataDirectivesToSizeMap.at(firstTkn.type));
                        }
                    }
                    int byteNumDiff = byteNum - startingByteNum;
                    std::cout << byteNumDiff << " bytes added for " << toString(firstTkn.type) << " directive \n";
                }
            }
        }

        //debug
        int numLinesInSecondPass = 0;
        std::cout << "\nTokenized lines (second pass):\n";
        for (const std::vector<Token> &line : tokenLines_TEXT_and_DATA) {
            numLinesInSecondPass++;
            std::cout << "Line " << numLinesInSecondPass << ":" ;
            for (const Token &tkn : line) {
                std::cout << " " << toString(tkn.type) << "-";
                std::cout << tkn.body;
            }
            std::cout << std::endl;
        }

        std::vector<TokenizedRomLine> finalRomLines {};
        //form instructions or some shit, resolve references
        int numLines = 0;
        byteIndex  = 0;
        for (const std::vector<Token> &line : tokenLines_TEXT_and_DATA) {
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
                continue; //becuz we already placed the label!
            }
            if (firstTknType == TokenType::CONST) {
                try {
                    if (line.size() > 3 && line.at(1).type == TokenType::REF && line.at(2).type == TokenType::EQUALS
                            && (line.at(3).type == TokenType::DECIMAL
                            || line.at(3).type == TokenType::HEX
                            || line.at(3).type == TokenType::BIN)
                            || (line.at(3).type == TokenType::SING_QUOTE
                                && line.at(4).type == TokenType::CHAR
                                && line.at(5).type == TokenType::SING_QUOTE))
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
                            } else if (line.at(4).type == TokenType::CHAR) {
                                value = static_cast<int>(line.at(4).body.at(0));
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
                } catch (std::out_of_range &e) {
                    LOG_ERR("ERROR: bad const declaration");
                }
            }
            //INSTR -> lines that SHOULD get made into instructions
            else if (firstTknType == TokenType::OPERATION) {
                byteIndex += 8;
                TokenizedInstruction instrForThisLine = parseLineOfTokensIntoTokenizedInstruction(line, labelMap, constMap, byteIndex);
                finalRomLines.emplace_back(instrForThisLine);
            }
            //DATA DIRECTIVES
            else if (containsValue(stringToDataDirectives, firstTknType)) {
                TokenizedData dataForThisLine = parseLineOfTokensIntoTokenizedData(line, labelMap, constMap, byteIndex);
                finalRomLines.emplace_back(dataForThisLine);
            } else {
                throwAFit(numLines);
                LOG_ERR(line.at(0).body + " (" + toString(line.at(0).type) + ") cannot begin a line");
            }
        }
        return finalRomLines;
    }
    //Sub-methods
    TokenizedInstruction Assembler::parseLineOfTokensIntoTokenizedInstruction(const std::vector<Token> &line,
            const std::unordered_map<std::string, uint16_t> &labelMap,
            const std::unordered_map<std::string, uint16_t> &constMap,
            const int &byteIndex
            ) const {
        int instructionIndex = byteIndex / 8;
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
                    LOG_ERR("ERROR: bad reference: " << tkn.body << " " << toString(tkn.type) << " in instruction #" << + instructionIndex);
                    LOG_ERR("label map: " + unorderedMapToString(labelMap));
                    LOG_ERR("const map: " + unorderedMapToString(constMap));
                }
            }
            revisedTokens.emplace_back(revisedRef);
        }
        bool inPtr = false;
        bool inChar = false;
        bool inString = false;
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
                if (tkn.type == TokenType::OPERATION) {
                    Token newTkn(tkn.body);
                    newTkn.type = TokenType::CHAR;
                    tokensForOperand.emplace_back(newTkn);
                } else {
                    tokensForOperand.emplace_back(tkn);
                }

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
            } else if (inString && tkn.type == TokenType::DOUB_QUOTE) {
                inString = false;
                tokensForOperand.emplace_back(tkn);

                // If the string literal is just " ", replace it with '\s'
                if (tokensForOperand.size() == 2 &&
                    tokensForOperand[0].type == TokenType::SING_QUOTE &&
                    tokensForOperand[1].type == TokenType::SING_QUOTE) {

                    tokensForOperand = {
                        Token("'"),
                        Token("\\s"),
                        Token("'")
                    };
                    LOG("Empty string literal detected. Assumed a space was written or intended -> substituted with \\s");
                    }

                operands.emplace_back(tokensForOperand);
                tokensForOperand.clear();
            } else if (tkn.type == TokenType::DOUB_QUOTE) {
                inString = true;
                tokensForOperand.emplace_back(tkn);
            } else if (inPtr || inChar || inString) {
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
    //.text-------------------------------------------------------------------------------------------------------------
    std::vector<parts::Instruction> Assembler::getLiteralInstructions(const std::vector<TokenizedRomLine>& tknRomLines) {
        std::vector<parts::Instruction> literalInstructions {};
        literalInstructions.reserve(tknRomLines.size());
        for (const auto& line : tknRomLines) {
            if (std::holds_alternative<TokenizedInstruction>(line)) {
                TokenizedInstruction lineAsInstr = std::get<TokenizedInstruction>(line);
                parts::Instruction litInstr = lineAsInstr.getLiteralInstruction();
                //printLiteralInstruction(litInstr); //for debug
                literalInstructions.emplace_back(litInstr);
            }
        }
        return literalInstructions;
    }
    void Assembler::printLiteralInstruction(const parts::Instruction &litInstr) {
        std::cout << "INSTR (DEBUG):\n";
        std::cout << "Opcode: " << std::hex << std::setw(4) << std::setfill('0') << litInstr.opcode << "\n";
        std::cout << "Imm: " << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(litInstr.immFlags) << "\n";
        std::cout << "Op14: " << std::hex << std::setw(4) << std::setfill('0') << litInstr.opCode14 << "\n";
        std::cout << "dest: " << std::hex << std::setw(4) << std::setfill('0') << litInstr.dest << "\n";
        std::cout << "src1: " << std::hex << std::setw(4) << std::setfill('0') << litInstr.src1 << "\n";
        std::cout << "src2: " << std::hex << std::setw(4) << std::setfill('0') << litInstr.src2 << "\n";
        std::cout << "\n";
    } //debug
    std::vector<uint8_t> Assembler::buildByteVecFromLiteralInstructions(const std::vector<parts::Instruction>& literalInstructions) {
        std::vector<uint8_t> byteVec;
        byteVec.reserve(literalInstructions.size() * 8);

        for (const auto& instr : literalInstructions) {
            std::vector<uint8_t> vec8Byte = build8ByteVecFromSingleLiteralInstruction(instr);
            byteVec.insert(byteVec.end(), vec8Byte.begin(), vec8Byte.end());
        }

        return byteVec;
    }
    std::vector<uint8_t> Assembler::build8ByteVecFromSingleLiteralInstruction(const parts::Instruction& literalInstruction) {
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
    //.data---------------------------------------------------------------------------------------------------------------
    TokenizedData Assembler::parseLineOfTokensIntoTokenizedData(const std::vector<Token> &line,
                                                                const std::unordered_map<std::string, uint16_t> &labelMap,
                                                                const std::unordered_map<std::string, uint16_t> &constMap,
                                                                int byteIndex) const {
        if (line.empty()) {
            LOG_ERR("ERROR: empty line fed to be made into TokenizedData");
            return TokenizedData(Token('\0'));
        }

        const Token& firstTkn = line.at(0);

        std::vector<Token> revisedDataTokens {}; //use this to build finalized data tokens, also make a function for this
        for (const auto& tkn : line) {
            if (dataDirectivesToSizeMap.contains(tkn.type) || validDataTokenTypes.contains(tkn.type)
                || tkn.type == TokenType::DOUB_QUOTE || tkn.type == TokenType::SING_QUOTE || tkn.type == TokenType::REF
                || tkn.type == TokenType::STRING) {
                if (validDataTokenTypes.contains(tkn.type) || tkn.type == TokenType::STRING) {
                    revisedDataTokens.emplace_back(tkn);
                }
                if (tkn.type == TokenType::REF) {
                    if (labelMap.contains(tkn.body)) {
                        revisedDataTokens.emplace_back(std::to_string(labelMap.at(tkn.body)));
                        LOG(".data: converted raw REF to label-mapped val: " + tkn.body);
                    } else if (constMap.contains(tkn.body)) {
                        revisedDataTokens.emplace_back(std::to_string(constMap.at(tkn.body)));
                        LOG(".data: converted raw REF to const-mapped val: " + tkn.body);
                    } else {
                        Token fixedStrTkn(tkn.body);
                        fixedStrTkn.type = TokenType::STRING;
                        revisedDataTokens.emplace_back(fixedStrTkn);
                        LOG(".data: converted raw REF to STRING since ref was not found in label or const map:" + fixedStrTkn.body);
                        if (firstTkn.type != TokenType::STRING_DIR) {
                            LOG_ERR("ERROR: misused string or invalid reference in directive " << toString(firstTkn.type));
                        }
                    }
                }
            }
            else {
                LOG_ERR("ERROR: bad data/directive in .data section: " << toString(tkn.type));
            }
        }
        TokenizedData dataLine(firstTkn);
        if (dataDirectivesToSizeMap.contains(firstTkn.type)) {
            dataLine.setDataTokens(std::vector<Token>(revisedDataTokens.begin(), revisedDataTokens.end()));
        }
        else {
            throwAFit(firstTkn.body);
            LOG_ERR("ERROR: bad directive" << line.size());
        }
        //enforce fixed directives
        if (!fixedDirectivesToSizeMap.contains(firstTkn.type)) {
            return dataLine;
        }
        size_t d_size = fixedDirectivesToSizeMap.at(firstTkn.type);
        if (dataLine.dataTokens.size() != d_size) {
            LOG_ERR("ERROR: (" << dataLine.directive.body <<  ") bad fixed-size directive: " << dataLine.dataTokens.size() << " != " << d_size);
            for (const Token &tkn : dataLine.dataTokens) {
                std::cerr << tkn.body << " | ";
            }
        }
        return dataLine;
    }

    std::vector<uint8_t> Assembler::parseTokenizedDataIntoByteVec(std::vector<TokenizedRomLine> &allTokenizedData) const {
        std::vector<uint8_t> byteVec {};
        for (const auto& romLine : allTokenizedData) {
            if (std::holds_alternative<TokenizedData>(romLine)) {
                auto dataLine = std::get<TokenizedData>(romLine);
                auto lineBytes = parseDataLineIntoBytes(dataLine);
                for (const auto& byte : lineBytes) {
                    byteVec.emplace_back(byte);
                }
            }
        }
        std::cout << "DEBUG, serialized data section: \n";
        for (const auto& byte : byteVec) {
            std::cout << byte;
        }
        std::cout << std::endl;

        return byteVec;
    }

    std::vector<uint8_t> Assembler::parseDataLineIntoBytes(const TokenizedData &dataLine) const {
        auto lineTkns = dataLine.dataTokens;
        const auto & directive = dataLine.directive;
        std::vector<uint8_t> lineBytes {};
        for (const auto& tkn : lineTkns) {
            auto thisTranslatedTkn = parsePieceOfDataIntoBytes(tkn, directive);
            for (const auto& byte : thisTranslatedTkn) {
                lineBytes.emplace_back(byte);
            }
        }
        return lineBytes;
    }

    std::vector<uint8_t> Assembler::parsePieceOfDataIntoBytes(const Token &pieceOfData, const Token &directive) const {
        std::vector<uint8_t> byteVec {};
        std::string strBody = pieceOfData.body;
        const TokenType& tknT = pieceOfData.type;
        if (tknT == TokenType::STRING) {
            for (const char& c : strBody) {
                byteVec.emplace_back(static_cast<uint8_t>(c));
            }
            byteVec.emplace_back(0); //null terminator
        }
        else if (tknT == TokenType::HEX) {
            uint16_t hexVal = static_cast<uint16_t>(std::stoi(strBody.substr(2), nullptr, 16));
            byteVec = convertWordToBytePair(hexVal);
        }
        else if (tknT == TokenType::DECIMAL) {
            auto hexVal = static_cast<uint16_t>(std::stoi(strBody, nullptr, 10));
            byteVec = convertWordToBytePair(hexVal);
        }
        else if (tknT == TokenType::BIN) {
            uint16_t hexVal = static_cast<uint16_t>(std::stoi(strBody.substr(2), nullptr, 2));
            byteVec = convertWordToBytePair(hexVal);
        }
        else if (tknT == TokenType::CHAR) {
            byteVec.emplace_back(static_cast<uint8_t>(strBody.at(0)));
        }
        else if (tknT == TokenType::CHAR_SPACE) {
            byteVec.emplace_back(32); //value of space character
        }
        if ((directive.type == TokenType::WORD_DIR || directive.type == TokenType::QWORD_DIR)
            && byteVec.size() == 1)
            {
            byteVec.emplace_back(0);
        }
        if ((directive.type == TokenType::BYTE_DIR || directive.type == TokenType::OCTBYTE_DIR)
            && byteVec.size() == 2) {
            byteVec.pop_back();
        }
        return byteVec;
    }


    //helpers-----------------------------------------------------------------------------------------------------------
    TokenType Token::deduceTokenType(const std::string &text) {
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
        else if (stringToDataDirectives.contains(text)) type = stringToDataDirectives.at(text); //for directives like .string
        else if (std::regex_match(text, std::regex("^0x[0-9A-Fa-f]+$"))) type = TokenType::HEX;
        else if (std::regex_match(text, std::regex("^0b[01]+$"))) type = TokenType::BIN;
        else if (std::regex_match(text, std::regex("^[-+]?[0-9]+$"))) type = TokenType::DECIMAL;
        else if (text[0] == '#') type = TokenType::COMMENT;
        else if (text[0] == '\n') type = TokenType::EOL;
        else if (text.length() == 1 && (std::isalpha(text[0]) || std::__format_spec::__is_ascii(text[0]))) type = TokenType::CHAR;
        else if (text == "\\0"){
            type = TokenType::CHAR;
        }
        else if (text == "\\s") type = TokenType::CHAR_SPACE;
        else if (text.length() > 1 && text.back() == ':') type = TokenType::LABEL; //needs trimming?
        else if (text == ".const") type = TokenType::CONST;
        else if (text == ".data") type = TokenType::DATA;
        else if (text == ".text") type = TokenType::TEXT;
        else if (std::ranges::all_of(text, [](char c) { return (std::isalnum(c) || c == '_'); })) type = TokenType::REF;
        else if (std::ranges::all_of(text, [](char c) { return (std::isalnum(c) || validSymbols.contains(c)); })) type = TokenType::STRING;
        return type;
    }

    void Token::correctNullChar() {
        if (type != TokenType::CHAR) return;
        if (body == "\\0") {
            body = "";
            body += '\0';
        }
    }

    std::pair<std::string, std::string> splitOpcodeStr(std::string opcodeStr) {
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

    TokenizedData::TokenizedData(Token directive) : directive(std::move(directive)) {}

    void TokenizedData::setDataTokens(const std::vector<Token> &dataTokens){
        this->dataTokens = dataTokens;
    };

    void TokenizedInstruction::setOperandsAndAutocorrectImmediates(const bool& doNotAutoCorrectImmediates, std::vector<Operand> operands) {
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
        } else if (operands.size() == 2 && opE == isa::Opcode_E::COMP) {
            src1 = operands.at(0);
            src2 = operands.at(1);
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

    //constructors for tokenized instruction components --------------------------------------------------------------------
    OpCode::OpCode(Token token): token(std::move(token)) {
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
    Operand::Operand(std::vector<Token> tokens) : tokens(std::move(tokens)), significantToken('\0') {
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

    //methods for resolving tokenized instr into literals-------------------------------------------------------------------
    uint16_t Operand::resolveInto16BitLiteral() const {
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

    parts::Instruction TokenizedInstruction::getLiteralInstruction() const {
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

    void TokenizedInstruction::resolve() {
        opcode.resolution = Resolution::RESOLVED;
    } // (largely WIP)
}




