#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>

enum class TokenType {
    Bool,
};

std::string to_string(TokenType typ);

struct Token {
    TokenType typ;
    int line;
    std::string cargo;
};

#endif  // TOKEN_H_
