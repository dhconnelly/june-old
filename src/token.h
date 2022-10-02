#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>

enum class TokenType {
    Bool,
};

std::string to_string(TokenType typ);

struct Token final {
    TokenType typ;
    int line;
    std::string cargo;

    std::string str() const;
};

#endif  // TOKEN_H_
