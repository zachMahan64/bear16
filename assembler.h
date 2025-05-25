//
// Created by Zachary Mahan on 5/23/25.
//
#pragma once
#include <unordered_map>
#include <string>
#include <cstdint>
#include "isa.h"
#include "parts.h"

#ifndef ASSEMBLER_H
#define ASSEMBLER_H
namespace assembler {
    void asmToBinMapGenerator();
    inline const std::unordered_map<std::string, uint16_t> opcodeToBinMap = {
        {"add", 0x0000},
        {"addi", 0xC000},
        {"addi1", 0x8000},
        {"addi2", 0x4000},
        {"sub", 0x0001},
        {"subi", 0xC001},
        {"subi1", 0x8001},
        {"subi2", 0x4001},
        {"mult", 0x0002},
        {"multi", 0xC002},
        {"multi1", 0x8002},
        {"multi2", 0x4002},
        {"div", 0x0003},
        {"divi", 0xC003},
        {"divi1", 0x8003},
        {"divi2", 0x4003},
        {"mod", 0x0004},
        {"modi", 0xC004},
        {"modi1", 0x8004},
        {"modi2", 0x4004},
        {"and", 0x0005},
        {"andi", 0xC005},
        {"andi1", 0x8005},
        {"andi2", 0x4005},
        {"or", 0x0006},
        {"ori", 0xC006},
        {"ori1", 0x8006},
        {"ori2", 0x4006},
        {"xor", 0x0007},
        {"xori", 0xC007},
        {"xori1", 0x8007},
        {"xori2", 0x4007},
        {"not", 0x0008},
        {"noti", 0xC008},
        {"noti1", 0x8008},
        {"noti2", 0x4008},
        {"nand", 0x0009},
        {"nandi", 0xC009},
        {"nandi1", 0x8009},
        {"nandi2", 0x4009},
        {"nor", 0x000A},
        {"nori", 0xC00A},
        {"nori1", 0x800A},
        {"nori2", 0x400A},
        {"neg", 0x000B},
        {"negi", 0xC00B},
        {"negi1", 0x800B},
        {"negi2", 0x400B},
        {"lsh", 0x000C},
        {"lshi", 0xC00C},
        {"lshi1", 0x800C},
        {"lshi2", 0x400C},
        {"rsh", 0x000D},
        {"rshi", 0xC00D},
        {"rshi1", 0x800D},
        {"rshi2", 0x400D},
        {"rol", 0x000E},
        {"roli", 0xC00E},
        {"roli1", 0x800E},
        {"roli2", 0x400E},
        {"ror", 0x000F},
        {"rori", 0xC00F},
        {"rori1", 0x800F},
        {"rori2", 0x400F},
        {"eq", 0x0010},
        {"eqi", 0xC010},
        {"eqi1", 0x8010},
        {"eqi2", 0x4010},
        {"ne", 0x0011},
        {"nei", 0xC011},
        {"nei1", 0x8011},
        {"nei2", 0x4011},
        {"lt", 0x0012},
        {"lti", 0xC012},
        {"lti1", 0x8012},
        {"lti2", 0x4012},
        {"le", 0x0013},
        {"lei", 0xC013},
        {"lei1", 0x8013},
        {"lei2", 0x4013},
        {"gt", 0x0014},
        {"gti", 0xC014},
        {"gti1", 0x8014},
        {"gti2", 0x4014},
        {"ge", 0x0015},
        {"gei", 0xC015},
        {"gei1", 0x8015},
        {"gei2", 0x4015},
        {"ult", 0x0016},
        {"ulti", 0xC016},
        {"ulti1", 0x8016},
        {"ulti2", 0x4016},
        {"ule", 0x0017},
        {"ulei", 0xC017},
        {"ulei1", 0x8017},
        {"ulei2", 0x4017},
        {"ugt", 0x0018},
        {"ugti", 0xC018},
        {"ugti1", 0x8018},
        {"ugti2", 0x4018},
        {"uge", 0x0019},
        {"ugei", 0xC019},
        {"ugei1", 0x8019},
        {"ugei2", 0x4019},
        {"z", 0x001A},
        {"zi", 0xC01A},
        {"zi1", 0x801A},
        {"zi2", 0x401A},
        {"nz", 0x001B},
        {"nzi", 0xC01B},
        {"nzi1", 0x801B},
        {"nzi2", 0x401B},
        {"c", 0x001C},
        {"ci", 0xC01C},
        {"ci1", 0x801C},
        {"ci2", 0x401C},
        {"nc", 0x001D},
        {"nci", 0xC01D},
        {"nci1", 0x801D},
        {"nci2", 0x401D},
        {"n", 0x001E},
        {"ni", 0xC01E},
        {"ni1", 0x801E},
        {"ni2", 0x401E},
        {"nn", 0x001F},
        {"nni", 0xC01F},
        {"nni1", 0x801F},
        {"nni2", 0x401F},
        {"mov", 0x0020},
        {"movi", 0xC020},
        {"movi1", 0x8020},
        {"movi2", 0x4020},
        {"lw", 0x0021},
        {"lwi", 0xC021},
        {"lwi1", 0x8021},
        {"lwi2", 0x4021},
        {"lb", 0x0022},
        {"lbi", 0xC022},
        {"lbi1", 0x8022},
        {"lbi2", 0x4022},
        {"comp", 0x0023},
        {"compi", 0xC023},
        {"compi1", 0x8023},
        {"compi2", 0x4023},
        {"lea", 0x0024},
        {"leai", 0xC024},
        {"leai1", 0x8024},
        {"leai2", 0x4024},
        {"push", 0x0025},
        {"pushi", 0xC025},
        {"pushi1", 0x8025},
        {"pushi2", 0x4025},
        {"pop", 0x0026},
        {"popi", 0xC026},
        {"popi1", 0x8026},
        {"popi2", 0x4026},
        {"clr", 0x0027},
        {"clri", 0xC027},
        {"clri1", 0x8027},
        {"clri2", 0x4027},
        {"inc", 0x0028},
        {"inci", 0xC028},
        {"inci1", 0x8028},
        {"inci2", 0x4028},
        {"dec", 0x0029},
        {"deci", 0xC029},
        {"deci1", 0x8029},
        {"deci2", 0x4029},
        {"memcpy", 0x002A},
        {"memcpyi", 0xC02A},
        {"memcpyi1", 0x802A},
        {"memcpyi2", 0x402A},
        {"sw", 0x002B},
        {"swi", 0xC02B},
        {"swi1", 0x802B},
        {"swi2", 0x402B},
        {"sb", 0x002C},
        {"sbi", 0xC02C},
        {"sbi1", 0x802C},
        {"sbi2", 0x402C},
        {"call", 0x0040},
        {"calli", 0xC040},
        {"calli1", 0x8040},
        {"calli2", 0x4040},
        {"ret", 0x0041},
        {"reti", 0xC041},
        {"reti1", 0x8041},
        {"reti2", 0x4041},
        {"jmp", 0x0042},
        {"jmpi", 0xC042},
        {"jmpi1", 0x8042},
        {"jmpi2", 0x4042},
        {"jcond_z", 0x0043},
        {"jcond_zi", 0xC043},
        {"jcond_zi1", 0x8043},
        {"jcond_zi2", 0x4043},
        {"jcond_nz", 0x0044},
        {"jcond_nzi", 0xC044},
        {"jcond_nzi1", 0x8044},
        {"jcond_nzi2", 0x4044},
        {"jcond_neg", 0x0045},
        {"jcond_negi", 0xC045},
        {"jcond_negi1", 0x8045},
        {"jcond_negi2", 0x4045},
        {"jcond_nneg", 0x0046},
        {"jcond_nnegi", 0xC046},
        {"jcond_nnegi1", 0x8046},
        {"jcond_nnegi2", 0x4046},
        {"jcond_pos", 0x0047},
        {"jcond_posi", 0xC047},
        {"jcond_posi1", 0x8047},
        {"jcond_posi2", 0x4047},
        {"jcond_npos", 0x0048},
        {"jcond_nposi", 0xC048},
        {"jcond_nposi1", 0x8048},
        {"jcond_nposi2", 0x4048},
        {"nop", 0x0049},
        {"nopi", 0xC049},
        {"nopi1", 0x8049},
        {"nopi2", 0x4049},
        {"hlt", 0x004A},
        {"hlti", 0xC04A},
        {"hlti1", 0x804A},
        {"hlti2", 0x404A},
        {"jal", 0x004B},
        {"jali", 0xC04B},
        {"jali1", 0x804B},
        {"jali2", 0x404B},
        {"retl", 0x004C},
        {"retli", 0xC04C},
        {"retli1", 0x804C},
        {"retli2", 0x404C},
        {"clrfb", 0x0080},
        {"clrfbi", 0xC080},
        {"clrfbi1", 0x8080},
        {"clrfbi2", 0x4080},
        {"setpx", 0x0081},
        {"setpxi", 0xC081},
        {"setpxi1", 0x8081},
        {"setpxi2", 0x4081},
        {"blit", 0x0082},
        {"bliti", 0xC082},
        {"bliti1", 0x8082},
        {"bliti2", 0x4082},
    }; //WIP, add operand values too
    inline const std::unordered_map<std::string, uint16_t> namedOperandToBinMap = {
        // temporaries (volatile)
        {"r0", 0}, {"t0", 0},
        {"r1", 1}, {"t1", 1},
        {"r2", 2}, {"t2", 2},
        {"r3", 3}, {"t3", 3},
        // saved registers
        {"r4", 4}, {"s0", 4},
        {"r5", 5}, {"s1", 5},
        {"r6", 6}, {"s2", 6},
        {"r7", 7}, {"s3", 7},
        {"r8", 8}, {"s4", 8},
        {"r9", 9}, {"s5", 9},
        {"r10", 10}, {"s6", 10},
        {"r11", 11}, {"s7", 11},
        // return value and arguments
        {"r12", 12}, {"rv", 12},
        {"r13", 13}, {"a0", 13},
        {"r14", 14}, {"a1", 14},
        {"r15", 15}, {"ra", 15},
        // io ports
        {"io0", 16},
        {"io1", 17},
        // special-purpose
        {"sp", 18},
        {"fp", 19},
        {"pc", 20}};

    //reading asm file
    void parseAsmFile(const std::string &filename);

    //generic tokens
    enum class TokenType {
        REGISTER,      // s1, s2
        DECIMAL,       // 10
        LBRACKET,      // [
        RBRACKET,      // ]
        OPERATION,     // mov, add, etc.
        COMMA,         // ,
        COLON,         // (for labels)
        LABEL,         // label:
        COMMENT,       // # comment
        SYMBOL,        // $, @, etc.
        STRING,        // "string"
        HEX,           // 0x1000
        BIN,           // 0b1000
        EOL,           // end of line
        MISTAKE,       // obvious error
    };
    class Token {
        TokenType type;
        std::string text;
        explicit Token(const std::string &text) : text(text) {
            type = deduceTokenType(text);
        }
        TokenType deduceTokenType(std::string text);
    };
    enum class Resolution {
        RESOLVED,
        SYMBOLIC
    };
    //specific tokens
    enum class OperandType {
        REGISTER,
        DECIMAL,
        SYMBOL,
        STRING,
        HEX,
        BIN,
        LABEL,
        IRRELEVANT, // for operands that are not needed for the instruction
    };
    class Operand {
        std::string body;   // "s2", "0x1000"
        OperandType type;
        Resolution resolution;
        Operand(Token token);
    };
    enum class OpCodeType {
        SINGLE_MAP,  // 1:1 mapping to a single instruction -> 1 cycle
        MULTI_STEP,  // 1:1 mapping to a single instruction -> but takes multiple cycles
        MULTI_MAP,   // pseudo-instruction -> expands to multiple real instructions
    };
    class OpCode {
        std::string body;
        OpCodeType type;
        Resolution resolution;
        OpCode(Token token);
    };
    //tokenized instructions
    class InstructionFromTokens {
        OpCode opcode;
        std::optional<Operand> dest;
        std::optional<Operand> src1; // not needed for a couple Ops
        std::optional<Operand> src2; // optional for many Ops
        bool isValid = false;
        InstructionFromTokens::InstructionFromTokens(
            OpCode opcode,
            std::optional<Operand> dest,
            std::optional<Operand> src1,
            std::optional<Operand> src2
            ) : opcode(std::move(opcode)), dest(std::move(dest)), src1(std::move(src1)), src2(std::move(src2)) {
            validate();
        }
        void validate();
        std::vector<InstructionFromTokens> resolve();
        parts::Instruction getLiteralInstruction();
    };

}
#endif //ASSEMBLER_H