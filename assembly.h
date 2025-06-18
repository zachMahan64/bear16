// Created by Zachary Mahan on 5/23/25.
#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include <unordered_map>
#include <string>
#include <cstdint>
#include <unordered_set>
#include <vector>

#include "isa.h"
#include "parts.h"

namespace assembly {
    enum class TokenType;
    class Token;
    class TokenizedInstruction;
    class TokenizedData;
    using TokenizedRomLine = std::variant<TokenizedInstruction, TokenizedData>;

    void asmMnemonicSetGenerator();
    //valid set & map for opcode & named operands respectively ------------------------------------------------------
    extern const std::unordered_set<std::string> validOpcodeMnemonics;
    extern const std::unordered_map<std::string, uint16_t> namedOperandToBinMap;
    extern const std::vector<std::string> namedOperandsInOrder;
    //Enum maps
    extern const std::unordered_map<std::string, isa::Opcode_E> stringToOpcodeMap;
    extern const std::unordered_map<isa::Opcode_E, int> opcodeToOperandMinimumCountMap;
    extern const std::unordered_set<isa::Opcode_E> opCodesWithOptionalSrc2OffsetArgument;
    extern const std::unordered_set<isa::Opcode_E> opCodesWithOptionalSrc1OffsetArgument;
    extern const std::unordered_map<std::string, TokenType> stringToDataDirectives;
    extern const std::unordered_map<TokenType, size_t> dataDirectivesToSizeMap; //in bytes
    extern const std::unordered_set<char> validSymbols;
    extern const std::unordered_set<TokenType> validDataTokenTypes;
    extern const std::unordered_map<TokenType, size_t> fixedDirectivesToSizeMap;
    //main class
    class Assembler {
    public:
        bool isEnableDebug = true;
        bool doNotAutoCorrectImmediates = false;
        Assembler(bool enableDebug, bool doNotAutoCorrectImmediates);
        //reading asm file
        [[nodiscard]] std::vector<uint8_t> assembleFromFile(const std::string &path) const;
    private:
        static std::vector<Token> tokenizeAsmFirstPass(const std::string &filename);

        static void writeToFile(const std::string& filename, const std::vector<uint8_t>& data);
        [[nodiscard]] std::vector<TokenizedRomLine> parseListOfTokensIntoTokenizedRomLines(
            const std::vector<Token> &tokens) const;
        TokenizedInstruction parseLineOfTokensIntoTokenizedInstruction(const std::vector<Token> &line,
                                            const std::unordered_map<std::string, uint16_t> &labelMap,
                                            const std::unordered_map<std::string, uint16_t> &constMap,
                                            const int &instructionIndex
                                            ) const;
        TokenizedData parseLineOfTokensIntoTokenizedData(const std::vector<Token> &line,
                                                         const std::unordered_map<std::string, uint16_t> &labelMap,
                                                         const std::unordered_map<std::string, uint16_t> &constMap, int byteIndex
        ) const;
        std::vector<uint8_t> parseTokenizedDataIntoByteVec(std::vector<TokenizedRomLine> &allTokenizedData) const;
        std::vector<uint8_t> parseDataLineIntoBytes(const TokenizedData &dataLine) const;
        std::vector<uint8_t> parsePieceOfDataIntoBytes(const Token &pieceOfData, const Token &directive) const;
        static std::vector<parts::Instruction> getLiteralInstructions(const std::vector<TokenizedRomLine>& tknRomLines);
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

        WORD_DIR,    // .word
        QWORD_DIR,   //.qword
        BYTE_DIR,    // .byte
        OCTBYTE_DIR, // .octbyte
        STRING_DIR,  // .string

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
            case TokenType::TEXT:           return "TEXT";
            case TokenType::DATA:           return "DATA";
            case TokenType::WORD_DIR:       return "WORD_DIR";
            case TokenType::QWORD_DIR:      return "QWORD_DIR";
            case TokenType::BYTE_DIR:       return "BYTE_DIR";
            case TokenType::OCTBYTE_DIR:    return "OCTBYTE_DIR";
            case TokenType::STRING_DIR:     return "STRING_DIR";
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
            correctNullChar();
        }
        explicit Token(const char c) {
            body = std::string(1, c);
            type = deduceTokenType(body);
        }
        static TokenType deduceTokenType(const std::string &text);
        void correctNullChar();
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
    template <typename MapType, typename ValueType>
    bool containsValue(const MapType& map, const ValueType& value) {
        for (const auto& [key, val] : map) {
            if (val == value) {
                return true;
            }
        }
        return false;
    }
    class TokenizedData {
    public:
        Token directive;
        std::vector<Token> dataTokens;
        explicit TokenizedData(Token directive);
        void setDataTokens(const std::vector<Token>& dataTokens);
    };
    //helper
    inline std::vector<uint8_t> convertWordToBytePair(uint16_t val) {
        std::vector<uint8_t> bytePair{};
        bytePair.emplace_back(static_cast<uint8_t>(val & 0xFF));
        bytePair.emplace_back(static_cast<uint8_t>((val >> 8) & 0xFF));
        return bytePair;
    }
}
#endif //ASSEMBLER_H