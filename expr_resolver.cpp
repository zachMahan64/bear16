//
// Created by Zachary Mahan on 7/1/25.
//

#include "expr_resolver.h"

#include <iostream>
#include <__ostream/basic_ostream.h>

namespace expr_res {
    //meat of the operation
    ResolutionResult resolve(std::string input, const std::unordered_map<std::string, uint16_t> &labelMap,
        const std::unordered_map<std::string, uint16_t> &constMap) {
        ResolutionResult res {};
        return res;
    }
    //tokens
    TokenType deduceTokenType(std::string_view inp) {
        TokenType deducedType = TokenType::MISTAKE;
        if (inp == "+") {
            deducedType = TokenType::PLUS;
        }
    }

    std::optional<Value> deduceValue(std::string_view inp) {
        //check if float
        try {
            size_t pos;
            float floatVal = std::stof(std::string(inp), &pos);
            if (pos == inp.size())
                return Value(floatVal);
        } catch (...) {}

        //check if int
        std::optional<Value> deducedValue = std::nullopt;
        try {
            if (inp.substr(0, 2) == "0x") {
                deducedValue = Value(std::stoi(std::string(inp), nullptr, 16));
            } else if (inp.substr(0, 2) == "0b") {
                deducedValue = Value(std::stoi(std::string(inp), nullptr, 2));
            } else {
                deducedValue = Value(std::stoi(std::string(inp)));
            }
        } catch (...) {} // not integral
        return deducedValue;
    }

    void Token::resolveReference(const std::unordered_map<std::string, uint16_t> &labelMap,
        const std::unordered_map<std::string, uint16_t> &constMap) {
    }

    Token::Token(std::string_view inp, const std::unordered_map<std::string, uint16_t> &labelMap,
                 const std::unordered_map<std::string, uint16_t> &constMap) {
    }

    //expressions
    Expression::Expression(const std::vector<Token> &tokenizedInput,
        const std::unordered_map<std::string, uint16_t> &labelMap,
        const std::unordered_map<std::string, uint16_t> &constMap) {
    }

    void Expression::solve() {

    };
}
