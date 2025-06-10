// Created by Zachary Mahan on 5/23/25.
#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include <unordered_map>
#include <string>
#include <cstdint>
#include <unordered_set>

#include "isa.h"
#include "parts.h"

namespace assembly {
    class Token;
    class TokenizedInstruction;

    void asmMnemonicSetGenerator();
    //valid set & map for opcode & named operands respectively ------------------------------------------------------
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
    {"sb", isa::Opcode_E::SB}, {"lbrom", isa::Opcode_E::LBROM}, {"lwrom", isa::Opcode_E::LWROM},

    // Ctrl Flow
    {"call", isa::Opcode_E::CALL}, {"ret", isa::Opcode_E::RET}, {"jmp", isa::Opcode_E::JMP},
    {"jcond_z", isa::Opcode_E::JCOND_Z}, {"jcond_nz", isa::Opcode_E::JCOND_NZ},
    {"jcond_neg", isa::Opcode_E::JCOND_NEG}, {"jcond_nneg", isa::Opcode_E::JCOND_NNEG},
    {"jcond_pos", isa::Opcode_E::JCOND_POS}, {"jcond_npos", isa::Opcode_E::JCOND_NPOS},
    {"nop", isa::Opcode_E::NOP}, {"hlt", isa::Opcode_E::HLT}, {"jal", isa::Opcode_E::JAL},
    {"retl", isa::Opcode_E::RETL},

    // Video
    {"clrfb", isa::Opcode_E::CLRFB}, {"setpx", isa::Opcode_E::SETPX}, {"blit", isa::Opcode_E::BLIT}};
    inline const std::unordered_map<isa::Opcode_E, int> opcodeToOperandMinimumCountMap = {
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
        {isa::Opcode_E::COMP, 2},
        {isa::Opcode_E::LEA, 2}, //(3) but offset
        {isa::Opcode_E::PUSH, 1}, {isa::Opcode_E::POP, 1},
        {isa::Opcode_E::CLR, 1}, {isa::Opcode_E::INC, 1}, {isa::Opcode_E::DEC, 1}, //dest only
        {isa::Opcode_E::MEMCPY, 3},
        {isa::Opcode_E::SW, 2}, {isa::Opcode_E::SB, 2}, //(3) but offset (in src1!)
        {isa::Opcode_E::LWROM, 2}, {isa::Opcode_E::LWROM, 2}, //(3) but offset

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
    inline const std::unordered_set<isa::Opcode_E> opCodesWithOptionalSrc2OffsetArgument = {
        isa::Opcode_E::LW,
        isa::Opcode_E::LB,
        isa::Opcode_E::LEA
        };
    inline const std::unordered_set<isa::Opcode_E> opCodesWithOptionalSrc1OffsetArgument = {
        isa::Opcode_E::SW, isa::Opcode_E::SB
    };
    inline const std::unordered_set<char> validSymbols = {
        '!', '@', '/', '\\', '$', '%', '&', '^', '*', '(', ')', '\'', '~', '-'
    };

    //main class
    class Assembler {
    public:
        bool isEnableDebug = true;
        bool doNotAutoCorrectImmediates = false;
        Assembler(bool enableDebug, bool doNotAutoCorrectImmediates)
        : isEnableDebug(enableDebug), doNotAutoCorrectImmediates(doNotAutoCorrectImmediates) {};

        static Assembler initInstance(bool enableDebug, bool doNotAutoCorrectImmediates) {
            return {enableDebug, doNotAutoCorrectImmediates};
        } //likely unnecessary
        //reading asm file
        [[nodiscard]] std::vector<uint8_t> assembleFromFile(const std::string &path) const;
    private:
        static std::vector<Token> tokenizeAsmFirstPass(const std::string &filename);
        [[nodiscard]] std::vector<TokenizedInstruction> parseListOfTokensIntoTokenizedInstructions(const std::vector<Token> &tokens) const;
        [[nodiscard]] TokenizedInstruction parseLineOfTokens(const std::vector<Token> &line,
                                            const std::unordered_map<std::string, uint16_t> &labelMap,
                                            const std::unordered_map<std::string, uint16_t> &constMap,
                                            const int &instructionIndex
        ) const;
        static std::vector<parts::Instruction> getLiteralInstructions(const std::vector<TokenizedInstruction>& tknInstructions);
        static void printLiteralInstruction(const parts::Instruction &litInstr); //debug
        static std::vector<uint8_t> buildByteVecFromLiteralInstructions(const std::vector<parts::Instruction> &literalInstructions);
        static std::vector<uint8_t> build8ByteVecFromSingleLiteralInstruction(const parts::Instruction &literalInstruction);

        static void throwAFit(const int &lineNum) {
            std::cerr << "MISTAKE MADE ON WRITTEN LINE " << lineNum << std::endl;
        }
        static void throwAFit(const std::string &ref) {
            std::cerr << "MISTAKE MADE IN USAGE OR DEFINITION OF " << ref << std::endl;
        }
    };

    //gen tokens
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
        COLON, //
        DECIMAL, // 10
        HEX, // 0x1000
        BIN, // 0b1000
        EOL, // end of line
        CHAR, //character
        CHAR_SPACE, //space, but as a char (is not ignored)
        COMMENT, // # comment

        LABEL, // label:
        CONST,  // .const

        TEXT, //.text
        DATA, //.data

        WORD,    // .word
        QWORD,   //.qword
        OCTBYTE, // .octbyte

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
            case TokenType::CHAR_SPACE:     return "CHAR_SPACE";
            case TokenType::HEX:            return "HEX";
            case TokenType::BIN:            return "BIN";
            case TokenType::EOL:            return "EOL";
            case TokenType::COMMENT:        return "COMMENT";
            case TokenType::LABEL:          return "LABEL";
            case TokenType::CONST:          return "CONST";
            case TokenType::TEXT:          return "TEXT";
            case TokenType::DATA:          return "DATA";
            case TokenType::REF:            return "REF";
            case TokenType::STRING:         return "STRING";
            default:                        return "*UNKNOWN";
        }
    }
    inline const std::unordered_set<TokenType> validOperandArguments = {
        TokenType::REF, TokenType::STRING, TokenType::GEN_REG, TokenType::SPEC_REG,
        TokenType::IO_PSEUDO_REG, TokenType::DECIMAL, TokenType::HEX, TokenType::BIN,
        TokenType::CHAR, TokenType::CHAR_SPACE
    };

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
        UNRESOLVED,
        SYMBOLIC
    };
    enum class ImmType {
        NO_IM, I, I1, I2
    };
    enum class ValueType {
        IMM, NAMED
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
        ValueType valueType;
        Resolution resolution;
        explicit Operand(std::vector<Token> tokens);
        [[nodiscard]] uint16_t resolveInto16BitLiteral() const;
    };

    //tokenized instructions
    class TokenizedInstruction {
    public:
        OpCode opcode;
        bool i1 = false;
        bool i2 = false;
        std::optional<Operand> dest;
        std::optional<Operand> src1; // not needed for a couple Ops
        std::optional<Operand> src2; // optional for many Ops
        TokenizedInstruction(
            OpCode opcode,
            std::optional<Operand> dest,
            std::optional<Operand> src1,
            std::optional<Operand> src2
            ) : opcode(std::move(opcode)), dest(std::move(dest)), src1(std::move(src1)), src2(std::move(src2)) {}
        explicit TokenizedInstruction(OpCode opcode) : opcode(std::move(opcode)) {}
        void setOperandsAndAutocorrectImmediates(const bool& doNotAutoCorrectImmediates, std::vector<Operand> operands);
        void resolve();
        [[nodiscard]] parts::Instruction getLiteralInstruction() const;
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