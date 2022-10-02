#include "token.h"

#include "absl/strings/str_format.h"

std::string to_string(TokenType typ) {
    switch (typ) {
        case TokenType::Bool: return "Bool";
        case TokenType::Lparen: return "Lparen";
        case TokenType::Rparen: return "Rparen";
        case TokenType::Symbol: return "Symbol";
    }
}

std::string Token::str() const {
    return absl::StrFormat("Token(line=%d, typ=%s, cargo=\"%s\")", line,
                           to_string(typ), cargo);
}
