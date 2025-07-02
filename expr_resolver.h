//
// Created by Zachary Mahan on 7/1/25.
//

#ifndef EXPRESSION_RESOLVER_H
#define EXPRESSION_RESOLVER_H
#include <string>

class ExprResolver {
    std::string_view input;

    void solve();
};
class Term {
    std::string termStr;
    void solve();
};
class Factor {
    std::string factorStr;
    void solve();
};


#endif //EXPRESSION_RESOLVER_H
