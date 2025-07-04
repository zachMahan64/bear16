//
// Created by Zachary Mahan on 7/1/25.
//

#ifndef EXPRESSION_RESOLVER_H
#define EXPRESSION_RESOLVER_H
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "fixpt8_8_t.h"

namespace expr_res {
    using Value = std::variant<int, float>;
    struct ResolutionResult {
        std::string str;
        uint16_t raw;
    };
    extern const std::unordered_set<char> monoChars;
    ResolutionResult resolve(const std::string &input, const std::unordered_map<std::string, uint16_t>& labelMap,
                        const std::unordered_map<std::string, uint16_t>& constMap);
    //Tokens
    //helpers
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
    std::optional<Value> deduceValue();
    //main class
    class Token {
        std::string_view str;
        TokenType type = TokenType::MISTAKE;
        std::optional<Value> value;
        const std::unordered_map<std::string, uint16_t>& labelMap{};
        const std::unordered_map<std::string, uint16_t>& constMap{};
        [[nodiscard]] TokenType deduceTokenType() const;
    public:
        Token(std::string_view inp);
        Token(std::string_view inp, const std::unordered_map<std::string, uint16_t>& labelMap,
              const std::unordered_map<std::string, uint16_t>& constMap);
        [[nodiscard]] bool isFloat() const { return value && std::holds_alternative<float>(*value); }
        [[nodiscard]] float asFloat() const {
            if (!isFloat()) throw std::logic_error("Primary does not hold float: " + std::string(str));
            return std::get<float>(*value);
        }
        [[nodiscard]] bool isInt() const { return value && std::holds_alternative<int>(*value); }
        [[nodiscard]] int asInt() const {
            if (!isInt()) throw std::logic_error("Primary does not hold int: " + std::string(str));
            return std::get<int>(*value);
        }
        [[nodiscard]] TokenType getType() const { return type; }
    };
    //Expressions
    struct Expression {
        std::vector<Token> exprTokens{};
        Value result = 0;
        const std::unordered_map<std::string, uint16_t>& labelMap;
        const std::unordered_map<std::string, uint16_t>& constMap;
        Expression(const std::vector<Token>& tokenizedInput, const std::unordered_map<std::string, uint16_t>& labelMap,
                   const std::unordered_map<std::string, uint16_t>& constMap);
        Token solve() const;
    };
    struct Term {
        std::vector<Token> tokens{};
        TokenType sign;
        Value value{};
        Term(const TokenType& sign, const std::vector<Token>& tokens) : tokens(tokens), sign(sign) {};
    };
}

#endif //EXPRESSION_RESOLVER_H
