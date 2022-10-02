#include "token.h"

#include "absl/strings/str_format.h"

std::string to_string(TokenType typ) {
    switch (typ) {
        case TokenType::Bool: return "Bool";
    }
}

std::string Token::str() const {
    return absl::StrFormat("Token(line=%d, typ=%s, cargo=\"%s\")", line,
                           to_string(typ), cargo);
}
