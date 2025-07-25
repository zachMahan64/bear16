//
// Created by Zachary Mahan on 7/1/25.
//

#ifndef EXPRESSION_RESOLVER_H
#define EXPRESSION_RESOLVER_H
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace expr_res {
struct ResolutionResult {
    std::string str;
    uint16_t raw;
};
extern const std::unordered_set<char> monoChars;
ResolutionResult
resolveStrExpr(const std::string &input,
               const std::unordered_map<std::string, uint16_t> &labelMap,
               const std::unordered_map<std::string, uint16_t> &constMap);
// Tokens
// helpers
enum class TokenType {
    INTEGER,
    REAL,
    PLUS,
    MIN,
    MULT,
    DIV,
    LPAREN,
    RPAREN,
    CONSTANT,
    LABEL,
    MISTAKE
};
std::optional<float> deduceValue();
// main class
class Token {
    std::string str{};
    TokenType type = TokenType::MISTAKE;
    std::optional<float> value;
    std::optional<const std::unordered_map<std::string, uint16_t>> labelMap{};
    std::optional<const std::unordered_map<std::string, uint16_t>> constMap{};
    [[nodiscard]] TokenType deduceTokenType() const;

  public:
    Token(float valInp, TokenType typeInp);
    Token(std::string_view inp);
    Token(std::string_view inp,
          const std::unordered_map<std::string, uint16_t> &labelMap,
          const std::unordered_map<std::string, uint16_t> &constMap);
    [[nodiscard]] bool isFloat() const { return value && *value; }
    [[nodiscard]] float asFloat() const {
        if (!isFloat())
            throw std::logic_error("Primary does not hold float: " +
                                   std::string(str));
        return *value;
    }
    [[nodiscard]] int asInt() const { return static_cast<int>(*value); }
    [[nodiscard]] TokenType getType() const { return type; }
    [[nodiscard]] std::string getStr() const { return str; }
    [[nodiscard]] std::optional<float> getValue() const { return value; }
};
// Expressions
struct Expression {
    std::vector<Token> exprTokens{};
    float result = 0;
    const std::unordered_map<std::string, uint16_t> &labelMap;
    const std::unordered_map<std::string, uint16_t> &constMap;
    Expression(const std::vector<Token> &tokenizedInput,
               const std::unordered_map<std::string, uint16_t> &labelMap,
               const std::unordered_map<std::string, uint16_t> &constMap);
    [[nodiscard]] Token solve() const;
};
struct Term {
    std::vector<Token> tokens{};
    TokenType sign;
    float value{};
    Term(const TokenType &sign, const std::vector<Token> &tokens)
        : tokens(tokens), sign(sign) {};
};
} // namespace expr_res

#endif // EXPRESSION_RESOLVER_H
