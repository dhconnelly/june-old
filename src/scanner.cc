#include "scanner.h"

#include "absl/strings/str_format.h"

namespace {
bool is_whitespace(char ch) { return ch == '\n' || ch == '\t' || ch == ' '; }

bool is_terminating(std::optional<char> ch) {
    return !ch.has_value() || is_whitespace(ch.value()) || ch.value() == ')';
}
}  // namespace

std::optional<char> Scanner::advance() {
    return at_end() ? std::nullopt : std::optional{text_[pos_++]};
}

std::optional<char> Scanner::peek() const {
    return at_end() ? std::nullopt : std::optional{text_[pos_]};
}

absl::Status Scanner::invalid(std::string_view message) const {
    return absl::InvalidArgumentError(
        absl::StrFormat("[line %d] scanner: %s", line_, message));
}

Token Scanner::token(TokenType typ) const {
    std::string cargo(text_.substr(start_, pos_ - start_));
    return Token{.line = line_, .cargo = cargo, .typ = typ};
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
                auto ch2 = advance();
                if (!ch2) return invalid("unexpected eof");
                if ((*ch2 != 't' && *ch2 != 'f') || !is_terminating(peek())) {
                    return invalid("bad bool literal");
                }
                return token(TokenType::Bool);
            }

            default: return invalid(absl::StrFormat("invalid token: %c", ch));
        }
    }
    return std::nullopt;
}

absl::StatusOr<std::vector<Token>> scan(std::string_view text) {
    Scanner scan(text);
    std::vector<Token> toks;
    while (!scan.at_end()) {
        auto tok = scan.next();
        if (!tok.ok()) return tok.status();
        if (!tok->has_value()) break;
        toks.push_back(tok->value());
    }
    return toks;
}
