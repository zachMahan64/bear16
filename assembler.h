//
// Created by Zachary Mahan on 5/23/25.
//
#pragma once
#include <unordered_map>
#include <string>
#include <cstdint>

#include "assembler.h"
#include "isa.h"
#include "parts.h"

#ifndef ASSEMBLER_H
#define ASSEMBLER_H
namespace assembler {
    class Token;
    class TokenizedInstruction;

    void asmToBinMapGenerator();
    //string body to binary map (for turing tokenized instr to binary)
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
    };
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
    //Enum maps
    inline const std::unordered_map<std::string, isa::Opcode_E> stringToOpcodeMap = {
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
    {"sb", isa::Opcode_E::SB},

    // Ctrl Flow
    {"call", isa::Opcode_E::CALL}, {"ret", isa::Opcode_E::RET}, {"jmp", isa::Opcode_E::JMP},
    {"jcond_z", isa::Opcode_E::JCOND_Z}, {"jcond_nz", isa::Opcode_E::JCOND_NZ},
    {"jcond_neg", isa::Opcode_E::JCOND_NEG}, {"jcond_nneg", isa::Opcode_E::JCOND_NNEG},
    {"jcond_pos", isa::Opcode_E::JCOND_POS}, {"jcond_npos", isa::Opcode_E::JCOND_NPOS},
    {"nop", isa::Opcode_E::NOP}, {"hlt", isa::Opcode_E::HLT}, {"jal", isa::Opcode_E::JAL},
    {"retl", isa::Opcode_E::RETL},

    // Video
    {"clrfb", isa::Opcode_E::CLRFB}, {"setpx", isa::Opcode_E::SETPX}, {"blit", isa::Opcode_E::BLIT}};
    inline const std::unordered_map<isa::Opcode_E, int> opcodeToOperandCountMap = {
        // Arith & Bitwise
        {isa::Opcode_E::ADD, 3}, {isa::Opcode_E::SUB, 3}, {isa::Opcode_E::MULT, 3},
        {isa::Opcode_E::DIV, 3}, {isa::Opcode_E::MOD, 3}, {isa::Opcode_E::AND, 3},
        {isa::Opcode_E::OR, 3}, {isa::Opcode_E::XOR, 3}, {isa::Opcode_E::NOT, 3},
        {isa::Opcode_E::NAND, 3}, {isa::Opcode_E::NOR, 3}, {isa::Opcode_E::NEG, 3},
        {isa::Opcode_E::LSH, 3}, {isa::Opcode_E::RSH, 3}, {isa::Opcode_E::ROL, 3},
        {isa::Opcode_E::ROR, 3},

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
        {isa::Opcode_E::COMP, 2}, {isa::Opcode_E::LEA, 2}, //(3) but offset
        {isa::Opcode_E::PUSH, 1}, {isa::Opcode_E::POP, 1},
        {isa::Opcode_E::CLR, 1}, {isa::Opcode_E::INC, 1}, {isa::Opcode_E::DEC, 1},
        {isa::Opcode_E::MEMCPY, 3},
        {isa::Opcode_E::SW, 2}, {isa::Opcode_E::SB, 2}, //(3) but offset (in src1!)

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

    //reading asm file
    std::vector<Token> tokenizeAsmFirstPass(const std::string &filename);
    std::vector<TokenizedInstruction> parseFirstPassIntoSecondPass(std::vector<Token> &tokens);
    TokenizedInstruction parseLineOfTokens(std::vector<Token> line,
        std::unordered_map<std::string, uint16_t> &labelMap,
        std::unordered_map<std::string, uint16_t> &constMap,
        int &instructionIndex
        );
    inline void throwAFit(const int &lineNum) {
        std::cerr << "MISTAKE MADE ON WRITTEN LINE " << lineNum << std::endl;
    }
    inline void throwAFit(const std::string &ref) {
        std::cerr << "MISTAKE MADE IN USAGE OR DEFINITION OF " << ref << std::endl;
    }

    enum class TokenType {
        MISTAKE, // obvious error
        GEN_REG, // s1, s2
        SPEC_REG, // sp, fp, pc
        IO_PSEUDO_REG, // io0, io1
        LBRACKET, // [
        RBRACKET, // ]
        SING_QUOTE, // '
        DOUB_QUOTE, // "
        EQUALS, // =
        PLUS, // +
        OPERATION, // mov, add, etc.
        COMMA, // ,
        COLON, // (for labels)
        DECIMAL, // 10
        HEX, // 0x1000
        BIN, // 0b1000
        EOL, // end of line
        CHAR, //character
        COMMENT, // # comment
        LABEL, // label:
        CONST,  // .const
        REF,  // (to label or const, use look up table)
        STRING // "string" --> not supported currently
    };
    inline std::string toString(TokenType type) {
        switch (type) {
            case TokenType::MISTAKE:        return "MISTAKE";
            case TokenType::GEN_REG:        return "GEN_REG";
            case TokenType::SPEC_REG:       return "SPEC_REG";
            case TokenType::IO_PSEUDO_REG:  return "IO_PSEUDO_REG";
            case TokenType::LBRACKET:       return "LBRACKET";
            case TokenType::RBRACKET:       return "RBRACKET";
            case TokenType::EQUALS:         return "EQUALS";
            case TokenType::SING_QUOTE:     return "SING_QUOTE";
            case TokenType::DOUB_QUOTE:     return "DOUB_QUOTE";
            case TokenType::PLUS:           return "PLUS";
            case TokenType::OPERATION:      return "OPERATION";
            case TokenType::COMMA:          return "COMMA";
            case TokenType::COLON:          return "COLON";
            case TokenType::DECIMAL:        return "DECIMAL";
            case TokenType::CHAR:           return "CHAR";
            case TokenType::HEX:            return "HEX";
            case TokenType::BIN:            return "BIN";
            case TokenType::EOL:            return "EOL";
            case TokenType::COMMENT:        return "COMMENT";
            case TokenType::LABEL:          return "LABEL";
            case TokenType::CONST:          return "CONST";
            case TokenType::REF:            return "REF";
            case TokenType::STRING:         return "STRING";
            default:                        return "*UNKNOWN";
        }
    }

    class Token{
    public:
        TokenType type;
        std::string body;
        explicit Token(const std::string & text): body(text) {
            type = deduceTokenType(text);
        }
        explicit Token(const char c) {
            body = std::string(1, c);
            type = deduceTokenType(body);
        }
        static TokenType deduceTokenType(const std::string &text);
    };
    //string to TokenType Map
    inline const std::unordered_map<std::string, TokenType> symbolToTokenMap = {
        {",", TokenType::COMMA},
        {":", TokenType::COLON},
        {"=", TokenType::EQUALS},
        {"+", TokenType::PLUS},
        {"'", TokenType::SING_QUOTE},
        {"\"", TokenType::DOUB_QUOTE},
        {"]", TokenType::RBRACKET},
        {"[", TokenType::LBRACKET},};
    //specific tokens
    enum class Resolution {
        RESOLVED,
        SYMBOLIC
    };
    enum class ImmType {
        NO_IM, I, I1, I2
    };
    class OpCode {
    public:
        Token token;
        ImmType immType;
        isa::Opcode_E opcode_e;
        Resolution resolution;
        explicit OpCode(Token token);
    };
    class Operand {
    public:
        std::vector<Token> tokens;
        std::string significantBody;
        Token significantToken;
        std::string fullBody;
        Resolution resolution;
        explicit Operand(std::vector<Token> tokens);
    };

    //tokenized instructions
    class TokenizedInstruction {
    public:
        OpCode opcode;
        std::optional<Operand> dest;
        std::optional<Operand> src1; // not needed for a couple Ops
        std::optional<Operand> src2; // optional for many Ops
        bool isValid = false;
        TokenizedInstruction(
            OpCode opcode,
            std::optional<Operand> dest,
            std::optional<Operand> src1,
            std::optional<Operand> src2
            ) : opcode(std::move(opcode)), dest(std::move(dest)), src1(std::move(src1)), src2(std::move(src2)) {
            validate();
        }
        explicit TokenizedInstruction(OpCode opcode) : opcode(std::move(opcode)) {}
        void setOperands(std::vector<Operand> operands);
        void validate();
        std::vector<TokenizedInstruction> resolve();
        parts::Instruction getLiteralInstruction();
    };
    std::pair<std::string, std::string> splitOpcodeStr(std::string opcodeStr);
    template <typename K, typename V>
    std::string unorderedMapToString(const std::unordered_map<K, V>& map) {
        std::ostringstream oss;
        oss << "{ ";
        for (auto it = map.begin(); it != map.end(); ++it) {
            oss << it->first << "|" << it->second;
            if (std::next(it) != map.end())
                oss << ", ";
        }
        oss << " }";
        return oss.str();
    }
}
#endif //ASSEMBLER_H