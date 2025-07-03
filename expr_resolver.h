//
// Created by Zachary Mahan on 7/1/25.
//

#ifndef EXPRESSION_RESOLVER_H
#define EXPRESSION_RESOLVER_H
#include <string>
#include <unordered_map>
#include "fixpt_16.h"

namespace expr_res {
    using Value = std::variant<int, float>;
    struct ResolutionResult {
        std::string display;
        uint16_t raw;
    };
    ResolutionResult resolve(std::string input, const std::unordered_map<std::string, uint16_t>& labelMap,
                        const std::unordered_map<std::string, uint16_t>& constMap);
    struct Expression {
        std::string_view exprStr;
        Value result = 0;
        Expression(std::string_view inp, const std::unordered_map<std::string, uint16_t>& labelMap,
                   const std::unordered_map<std::string, uint16_t>& constMap);
        void solve();
    };
    struct Term {
        std::string_view termStr;
        Value result = 0;
        Term(std::string_view inp, const std::unordered_map<std::string, uint16_t>& labelMap,
             const std::unordered_map<std::string, uint16_t>& constMap);
        void solve();
    };
    struct Factor {
        std::string_view factorStr;
        Value result = 0;
        Factor(std::string_view inp, const std::unordered_map<std::string, uint16_t>& labelMap,
         const std::unordered_map<std::string, uint16_t>& constMap);
        void solve();
    };
    enum class PrimarySymbol {
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
    struct Primary {
        std::string_view primaryStr;
        PrimarySymbol sym = PrimarySymbol::MISTAKE;
        std::optional<Value> value;
        void solve();
        Primary(std::string_view inp, const std::unordered_map<std::string, uint16_t>& labelMap,
                const std::unordered_map<std::string, uint16_t>& constMap);
        [[nodiscard]] bool isFloat() const { return value && std::holds_alternative<float>(*value); }
        [[nodiscard]] float asFloat() const {
            if (!isFloat()) throw std::logic_error("Primary does not hold float: " + std::string(primaryStr));
            return std::get<float>(*value);
        }
        [[nodiscard]] bool isInt() const { return value && std::holds_alternative<int>(*value); }
        [[nodiscard]] int asInt() const {
            if (!isInt()) throw std::logic_error("Primary does not hold int: " + std::string(primaryStr));
            return std::get<int>(*value);
        }
    };
}

#endif //EXPRESSION_RESOLVER_H
