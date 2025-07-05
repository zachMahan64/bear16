//
// Created by Zachary Mahan on 7/1/25.
//

#include "expr_resolver.h"

#include <any>
#include <iostream>
#include <__ostream/basic_ostream.h>

#include "assembly.h"

namespace expr_res {
    //meat of the operation
    const std::unordered_set<char> monoChars {
        '(', ')', '+', '-', '*', '/'
    };
    ResolutionResult resolveStrExpr(const std::string &input, const std::unordered_map<std::string, uint16_t> &labelMap,
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
                std::cerr << "MISTAKE declared in expression: " << tkn.getStr() << " | " << input << " " << std::endl;
                return {"0"};
            }
        }
        //break up tokenized input expressions recursively based on parens -> each of those into terms and factors -> solve
        Expression fullExpr(tokenizedInput, labelMap, constMap);
        Token tokenizedResult = fullExpr.solve();
        ResolutionResult result {};
        if (tokenizedResult.getType() == TokenType::REAL) {
            fixpt8_8_t fixptRep(tokenizedResult.asFloat());
            result.raw = static_cast<uint16_t>(fixptRep);
            if (std::get<float>(*tokenizedResult.getValue()) > 127.996 || std::get<float>(*tokenizedResult.getValue()) < -128.000) {
                std::cerr << "WARNING: result of expression " << input << " is outside of 8.8-bit fixpt range (-128.000 to 127.996)" << std::endl;
            }
        } else if (tokenizedResult.getType() == TokenType::INTEGER) {
            if (std::get<int>(*tokenizedResult.getValue()) < 0) {
                result.raw = static_cast<uint16_t>(static_cast<int16_t>(std::get<int>(*tokenizedResult.getValue())));
                if (std::get<int>(*tokenizedResult.getValue()) > 32767 || std::get<int>(*tokenizedResult.getValue()) < -32768) {
                    std::cerr << "WARNING: result of expression " << input << " is outside of signed 16-bit range (-32768 to 32767)" << std::endl;
                }
            }
            else {
                result.raw = static_cast<uint16_t>(tokenizedResult.asInt());
                if (std::get<int>(*tokenizedResult.getValue()) > 65356 || std::get<int>(*tokenizedResult.getValue()) < 0) {
                    std::cerr << "WARNING: result of expression " << input << " is outside of unsigned 16-bit range (0 to 65536)" << std::endl;
                }
            }
        }
        result.str = std::to_string(result.raw);
        LOG_ERR("!!! DEBUG: " << result.raw); //TODO, not working (probably trace to the exprRes function)
        return result;
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
        if (labelMap->contains(str)) return TokenType::LABEL;
        if (constMap->contains(str)) return TokenType::CONSTANT;
        Value testVal = *deduceValue(str);
        if (std::holds_alternative<float>(testVal)) return TokenType::REAL;
        if (std::holds_alternative<int>(testVal)) return TokenType::INTEGER;
        return TokenType::MISTAKE;
    }

    Token::Token(Value valInp, TokenType typeInp) {
        value = valInp;
        type = typeInp;
        if (type == TokenType::INTEGER) {
            str = std::to_string(std::get<int>(*value));
        }
        else if (type == TokenType::REAL) {
            str = std::to_string(std::get<float>(*value));
        }
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
            const auto it = labelMap.find(str);
            value = Value(it->second);
            type = TokenType::INTEGER;
        }
        else if (type == TokenType::CONSTANT) {
            const auto it = constMap.find(str);
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
        if (!currFactors.empty()) {
            terms.emplace_back(startingTermTokenType, currFactors);
        }
        bool resultIsReal = false;
        for (auto& term : terms) {
            if (term.tokens.size() == 1) {
                return term.tokens.at(0);
            }
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
            if (firstFactorType == TokenType::REAL || secondFactorType == TokenType::REAL) {
                resultIsReal = true;
            }
            if (term.tokens.at(1).getType() == TokenType::MULT) {
                if (firstFactorType == TokenType::INTEGER && secondFactorType == TokenType::INTEGER) {
                    term.value = term.tokens.at(0).asInt() * term.tokens.at(2).asInt();
                } else if (firstFactorType == TokenType::REAL && secondFactorType == TokenType::INTEGER) {
                    term.value = term.tokens.at(0).asFloat() * term.tokens.at(2).asInt();
                } else if (firstFactorType == TokenType::INTEGER && secondFactorType == TokenType::REAL) {
                    term.value = term.tokens.at(0).asInt() * term.tokens.at(2).asFloat();
                } else if (firstFactorType == TokenType::REAL && secondFactorType == TokenType::REAL) {
                    term.value = term.tokens.at(0).asFloat() * term.tokens.at(2).asFloat();
                } else {
                    std::cerr << "ERROR: malformed term (operations on non-number) in expression. " << std::endl;
                    return {"0"};
                }
            }
            else if (term.tokens.at(1).getType() == TokenType::DIV) {
                if (firstFactorType == TokenType::INTEGER && secondFactorType == TokenType::INTEGER) {
                    term.value = term.tokens.at(0).asInt() / term.tokens.at(2).asInt();
                } else if (firstFactorType == TokenType::REAL && secondFactorType == TokenType::INTEGER) {
                    term.value = term.tokens.at(0).asFloat() / term.tokens.at(2).asInt();
                } else if (firstFactorType == TokenType::INTEGER && secondFactorType == TokenType::REAL) {
                    term.value = term.tokens.at(0).asInt() / term.tokens.at(2).asFloat();
                } else if (firstFactorType == TokenType::REAL && secondFactorType == TokenType::REAL) {
                    term.value = term.tokens.at(0).asFloat() / term.tokens.at(2).asFloat();
                } else {
                    std::cerr << "ERROR: malformed term (operations on non-number) in expression. " << std::endl;
                    return {"0"};
                }
            }
            else {
                std::cerr << "ERROR: malformed term (invalid operation) in expression. " << std::endl;
                return {"0"};
            }
        }
        Value resultVal;
        TokenType resultType;
        if (resultIsReal) {
            resultVal = 0.0f;
            resultType = TokenType::REAL;
        } else {
            resultVal = 0;
            resultType = TokenType::INTEGER;
        }
        for (const auto& term : terms) {
            Value termVal = term.value;
            if (term.sign == TokenType::PLUS) {
                if (std::holds_alternative<float>(termVal)) {
                    resultVal = std::get<float>(resultVal) + std::get<float>(termVal);
                } else if (std::holds_alternative<int>(termVal) && resultIsReal) {
                    resultVal = std::get<float>(resultVal) + static_cast<float>(std::get<int>(termVal));
                }
                else if (std::holds_alternative<int>(termVal)) {
                    resultVal = std::get<int>(resultVal) + std::get<int>(termVal);
                }
            } else if (term.sign == TokenType::MIN) {
                if (std::holds_alternative<float>(termVal)) {
                    resultVal = std::get<float>(resultVal) - std::get<float>(termVal);
                } else if (std::holds_alternative<int>(termVal) && resultIsReal) {
                    resultVal = std::get<float>(resultVal) - static_cast<float>(std::get<int>(termVal));
                }
                else if (std::holds_alternative<int>(termVal)) {
                    resultVal = std::get<int>(resultVal) - std::get<int>(termVal);
                }
            }
        }
        return {resultVal, resultType};
    }
}
