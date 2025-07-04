//
// Created by Zachary Mahan on 7/1/25.
//

#include "expr_resolver.h"

#include <iostream>
#include <__ostream/basic_ostream.h>

#include "assembly.h"

namespace expr_res {
    //meat of the operation
    const std::unordered_set<char> monoChars {
        '(', ')', '+', '-', '*', '/'
    };
    ResolutionResult resolve(const std::string &input, const std::unordered_map<std::string, uint16_t> &labelMap,
        const std::unordered_map<std::string, uint16_t> &constMap) {

        std::vector<Token> tokenizedInput {};
        std::string curr {};
        auto emplaceToken = [&](std::string_view inp) {
            if (!inp.empty()) tokenizedInput.emplace_back(inp, labelMap, constMap);
        };
        for (char c : input) {
            if (c == ' ') {
                emplaceToken(curr);
                curr.clear();
            }
            else if (monoChars.contains(c)) {
                emplaceToken(curr);
                curr.clear();
                curr += c;
                emplaceToken(curr);
                curr.clear();
            }
            else {
                curr.push_back(c);
            }
        }
        emplaceToken(curr);
        for (const auto& tkn : tokenizedInput) {
            if (tkn.getType() == TokenType::MISTAKE) {
                std::cerr << "MISTAKE declared in expression: " << input << std::endl;
            }
        }
        //break up tokenized input expressions recursively based on parens -> each of those into terms and factors -> solve
        Expression fullExpr(tokenizedInput, labelMap, constMap);
        ResolutionResult res {};
        return res;
    }

    std::optional<Value> deduceValue(const std::string_view inp) {
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

    //tokens
    TokenType Token::deduceTokenType() const {
        if (str == "+") return TokenType::PLUS;
        if (str == "-") return TokenType::MIN;
        if (str == "*") return TokenType::MULT;
        if (str == "/") return TokenType::DIV;
        if (str == "(") return TokenType::LPAREN;
        if (str == ")") return TokenType::RPAREN;
        if (labelMap.contains(std::string(str))) return TokenType::LABEL;
        if (constMap.contains(std::string(str))) return TokenType::CONSTANT;
        return TokenType::MISTAKE;
    }

    Token::Token(std::string_view inp) {
        str = inp;
        type = deduceTokenType();
        value = deduceValue(inp);
    }

    Token::Token(const std::string_view inp,
                 const std::unordered_map<std::string, uint16_t>& labelMap,
                 const std::unordered_map<std::string, uint16_t>& constMap)
    : str(inp), labelMap(labelMap), constMap(constMap)
    {
        type = deduceTokenType();

        if (type == TokenType::LABEL) {
            const auto it = labelMap.find(std::string(str));
            value = Value(it->second);
            type = TokenType::INTEGER;
        }
        else if (type == TokenType::CONSTANT) {
            const auto it = constMap.find(std::string(str));
            value = Value(it->second);
            type = std::holds_alternative<float>(*value) ? TokenType::REAL : TokenType::INTEGER;
        }
        else {
            value = deduceValue(str);
        }
    }

    //expressions
    Expression::Expression(const std::vector<Token> &tokenizedInput,
        const std::unordered_map<std::string, uint16_t> &labelMap,
        const std::unordered_map<std::string, uint16_t> &constMap) : labelMap(labelMap), constMap(constMap), exprTokens(tokenizedInput)
    {}
    Token Expression::solve() const {
        std::vector<Term> terms {};
        auto isNum = [](const Token& tkn) { return tkn.getType() == TokenType::INTEGER || tkn.getType() == TokenType::REAL;};
        auto isPlusOrMinus = [](const Token& tkn) { return tkn.getType() == TokenType::PLUS || tkn.getType() == TokenType::MIN;};
        std::vector<Token> currFactors {};
        TokenType startingTermTokenType = TokenType::PLUS;
        for (const auto& tkn : exprTokens) {
            if (isPlusOrMinus(tkn)) {
                if (!currFactors.empty()) {
                    terms.emplace_back(startingTermTokenType, currFactors);
                    currFactors.clear();
                    startingTermTokenType = tkn.getType();
                }
                currFactors.emplace_back(tkn);
            }
        }
        for (auto& term : terms) {
            if (term.tokens.size() != 3) {
                std::cerr << "ERROR: malformed term in expression. " << std::endl;
                return {"0"};
            }
            if (!isNum(term.tokens.at(0)) || !isNum(term.tokens.at(2))) {
                std::cerr << "ERROR: malformed term (operations on non-number) in expression. " << std::endl;
                return {"0"};
            }

            const TokenType firstFactorType = term.tokens.at(0).getType();
            const TokenType secondFactorType = term.tokens.at(2).getType();
            const Value firstFactorValue = (firstFactorType == TokenType::INTEGER ? term.tokens.at(0).asInt() : term.tokens.at(0).asFloat());
            const Value secondFactorValue = (firstFactorType == TokenType::INTEGER ? term.tokens.at(2).asInt() : term.tokens.at(2).asFloat());


        }
        return {{}, {}, {}};
    }
}
