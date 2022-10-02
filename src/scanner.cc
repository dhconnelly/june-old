#include "scanner.h"

#include <cassert>

#include "absl/strings/str_format.h"

absl::Status Scanner::invalid(std::string_view message) const {
    return absl::InvalidArgumentError(
        absl::StrFormat("[line %d] scanner: %s", line_, message));
}

Token Scanner::token(TokenType typ) const {
    return Token{
        .line = line_,
        .cargo = std::string(text_.substr(start_, pos_ - start_)),
        .typ = typ,
    };
}

absl::StatusOr<std::optional<Token>> Scanner::next() {
    start_ = pos_;
    while (!at_end()) {
        char ch = advance().value();
        switch (ch) {
            case '\n': line_++;
            case ' ':
            case '\t': {
                start_ = pos_;
                continue;
            }

            case '#': {
                auto result = advance();
                if (!result) return invalid("unexpected eof");
                char ch = result.value();
                if (ch != 't' && ch != 'f') return invalid("bad bool literal");
                return token(TokenType::Bool);
            }

            default: return invalid("unknown token");
        }
    }
    return std::nullopt;
}

absl::StatusOr<std::vector<Token>> scan(std::string_view text) {
    Scanner scan(text);
    std::vector<Token> toks;
    while (!scan.at_end()) {
        auto result = scan.next();
        if (!result.ok()) return result.status();
        auto tok = result.value();
        if (!tok) break;
        toks.push_back(tok.value());
    }
    return toks;
}
