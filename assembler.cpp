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
#include "assembler.h"

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
}
void assembler::parseAsmFile(const std::string &path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "ERROR: Could not open .asm file: " << path << std::endl;
        return;
    }

    std::string buffer((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());

    std::string instrRelevantChars;
    instrRelevantChars.reserve(buffer.size());

    bool inComment = false;
    for (char c : buffer) {
        if (inComment) {
            if (c == '\n') {
                inComment = false;
                instrRelevantChars += c; //preserve newlines for line tracking
            }
            continue;
        }
        if (c == '#') {
            inComment = true;
            continue;
        }
        instrRelevantChars += c;
    }
    std::cout << "All Instruction-relevant chars (debug):\n" << instrRelevantChars << std::endl;
    // WIP: tokenize --> categorize tokens --> InstructionFromTokens --> binary
    for (char c : instrRelevantChars) {
        //go line-by-line then word-by-word and tokenize it all

    }
}
