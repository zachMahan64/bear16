//
// Created by Zachary Mahan on 7/1/25.
//

#include "expr_resolver.h"
#include <iostream>
#include <iostream>
#include <optional>

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
            if (*tokenizedResult.getValue() > 127.996 || *tokenizedResult.getValue() < -128.000) {
                std::cerr << "WARNING: result of expression " << input << " is outside of 8.8-bit fixpt range (-128.000 to 127.996)" << std::endl;
            }
        } else if (tokenizedResult.getType() == TokenType::INTEGER) {
            if (*tokenizedResult.getValue() < 0) {
                result.raw = static_cast<uint16_t>(static_cast<int16_t>(*tokenizedResult.getValue()));
                if (*tokenizedResult.getValue() > 32767 || *tokenizedResult.getValue() < -32768) {
                    std::cerr << "WARNING: result of expression " << input << " is outside of signed 16-bit range (-32768 to 32767)" << std::endl;
                }
            }
            else {
                result.raw = static_cast<uint16_t>(tokenizedResult.asInt());
                if (*tokenizedResult.getValue() > 65356 || *tokenizedResult.getValue() < 0) {
                    std::cerr << "WARNING: result of expression " << input << " is outside of unsigned 16-bit range (0 to 65536)" << std::endl;
                }
            }
        }
        result.str = std::to_string(result.raw);
        return result;
    }

    std::optional<float> deduceValue(const std::string_view inp) {
        //check if float
        try {
            size_t pos;
            float floatVal = std::stof(std::string(inp), &pos);
            if (pos == inp.size())
                return floatVal;
        } catch (...) {}

        //check if int
        std::optional<float> deducedValue = std::nullopt;
        try {
            if (inp.substr(0, 2) == "0x") {
                deducedValue = static_cast<float>(std::stoi(std::string(inp), nullptr, 16));
            } else if (inp.substr(0, 2) == "0b") {
                deducedValue = static_cast<float>(std::stoi(std::string(inp), nullptr, 2));
            } else {
                deducedValue = static_cast<float>(std::stoi(std::string(inp)));
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
        if (labelMap && labelMap->contains(str)) return TokenType::LABEL;
        if (constMap && constMap->contains(str)) return TokenType::CONSTANT;

        if (const auto valOpt = deduceValue(str)) {
            // Check if integer-valued float or real
            float val = *valOpt;
            if (val == static_cast<int>(val)) {
                return TokenType::INTEGER;
            }
            return TokenType::REAL;
        }
        return TokenType::MISTAKE;
    }


    Token::Token(float valInp, TokenType typeInp) {
        value = valInp;
        type = typeInp;
        if (type == TokenType::INTEGER) {
            str = std::to_string(*value);
        }
        else if (type == TokenType::REAL) {
            str = std::to_string(*value);
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
            value = it->second;
            type = TokenType::INTEGER;
        }
        else if (type == TokenType::CONSTANT) {
            const auto it = constMap.find(str);
            value = it->second;
            type = TokenType::INTEGER;
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
    std::vector<Term> terms{};
    auto isNum = [](const Token& tkn) {
        return tkn.getType() == TokenType::INTEGER || tkn.getType() == TokenType::REAL;
    };
    auto isPlusOrMinus = [](const Token& tkn) {
        return tkn.getType() == TokenType::PLUS || tkn.getType() == TokenType::MIN;
    };
    auto isParen = [](const Token& tkn) {
        return tkn.getType() == TokenType::LPAREN || tkn.getType() == TokenType::RPAREN;
    };

    std::vector<Token> currFactors{};
    TokenType startingTermSign = TokenType::PLUS;
        int leftParenCount = 0;
    // split tokens into additive terms
    for (const auto& tkn : exprTokens) {
        if (isParen(tkn)) {
            if (tkn.getType() == TokenType::LPAREN) {
                if (leftParenCount == 1) {
          std::cerr << "WARNING: Nested expression detected. This is not supported on the current assembler version." << std::endl;
                }
                leftParenCount++;
            }
            continue; // skip parens for now
        }
        if (isPlusOrMinus(tkn)) {
            if (!currFactors.empty()) {
                terms.emplace_back(startingTermSign, currFactors);
                currFactors.clear();
            }
            startingTermSign = tkn.getType();
            continue;
        }
        currFactors.emplace_back(tkn);
    }
    if (!currFactors.empty()) {
        terms.emplace_back(startingTermSign, currFactors);
    }

    // eval each term
    bool resultIsReal = false;
    for (auto& term : terms) {
        if (term.tokens.empty()) {
            std::cerr << "ERROR: empty term." << std::endl;
            return {0.0f, TokenType::INTEGER};
        }

        // eval the left-associative product/division chain
        std::optional<float> value;
        TokenType prevType = TokenType::MISTAKE;

        for (size_t i = 0; i < term.tokens.size(); ++i) {
            const Token& tkn = term.tokens[i];

            if (i % 2 == 0) { // number
                if (!isNum(tkn)) {
                    std::cerr << "ERROR: expected number in term." << std::endl;
                    return {0.0f, TokenType::INTEGER};
                }

                float num = tkn.getValue().value_or(0.0f);
                if (tkn.getType() == TokenType::REAL) resultIsReal = true;

                if (!value.has_value()) {
                    value = num;
                } else {
                    switch (prevType) {
                        case TokenType::MULT:
                            value = *value * num;
                            break;
                        case TokenType::DIV:
                            if (num == 0.0f) {
                                std::cerr << "ERROR: division by zero." << std::endl;
                                return {0.0f, TokenType::INTEGER};
                            }
                            value = *value / num;
                            break;
                        default:
                            std::cerr << "ERROR: unexpected operator before number." << std::endl;
                            return {0.0f, TokenType::INTEGER};
                    }
                }
            } else { // operator
                if (tkn.getType() != TokenType::MULT && tkn.getType() != TokenType::DIV) {
                    std::cerr << "ERROR: expected * or / in term." << std::endl;
                    return {0.0f, TokenType::INTEGER};
                }
                prevType = tkn.getType();
            }
        }

        if (!value.has_value()) {
            std::cerr << "ERROR: no value computed for term." << std::endl;
            return {0.0f, TokenType::INTEGER};
        }

        term.value = *value;
    }

    // Sum or subtract all evaluated terms
    float resultVal = 0.0f;
    for (const auto& term : terms) {
        if (term.sign == TokenType::PLUS) {
            resultVal += term.value;
        } else if (term.sign == TokenType::MIN) {
            resultVal -= term.value;
        } else {
            std::cerr << "ERROR: unexpected term sign." << std::endl;
            return {0.0f, TokenType::INTEGER};
        }
    }

    TokenType resultType = resultIsReal ? TokenType::REAL : TokenType::INTEGER;
    return {resultVal, resultType};
}


}
