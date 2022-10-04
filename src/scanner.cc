#include "scanner.h"

#include "absl/strings/str_format.h"

namespace {
bool is_whitespace(char ch) { return ch == '\n' || ch == '\t' || ch == ' '; }
bool is_symbol(char ch) {
    switch (ch) {
        case '+':
        case '/':
        case '*':
        case '%':
        case '=':
        case '<':
        case '>':
        case '-': return true;
        default: return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
    }
}
bool is_numeric(char ch) { return '0' <= ch && ch <= '9'; }

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

std::string_view Scanner::peek_cargo() const {
    return text_.substr(start_, pos_ - start_);
}

Token Scanner::token(TokenType typ) const {
    std::string cargo(peek_cargo());
    return Token{.line = line_, .cargo = cargo, .typ = typ};
}

TokenType lookup_keyword(std::string_view s) {
    if (s == "if") return TokenType::If;
    if (s == "let") return TokenType::Let;
    return TokenType::Symbol;
}

absl::StatusOr<Token> Scanner::symbol() {
    while (true) {
        auto ch = peek();
        if (is_terminating(ch)) break;
        if (!is_symbol(ch.value()) && !is_numeric(ch.value())) {
            return invalid(absl::StrFormat("invalid symbol: %c", ch.value()));
        }
        advance();
    }
    return token(lookup_keyword(peek_cargo()));
}

absl::StatusOr<Token> Scanner::number() {
    while (true) {
        auto ch = peek();
        if (is_terminating(ch)) break;
        if (!is_numeric(ch.value())) {
            return invalid(absl::StrFormat("invalid symbol: %c", ch.value()));
        }
        advance();
    }
    return token(TokenType::Int);
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

            case '(': return token(TokenType::Lparen);
            case ')': return token(TokenType::Rparen);

            case '#': {
                auto ch2 = advance();
                if (!ch2) return invalid("unexpected eof");
                if ((*ch2 != 't' && *ch2 != 'f') || !is_terminating(peek())) {
                    return invalid("bad bool literal");
                }
                return token(TokenType::Bool);
            }

            default: {
                if (is_numeric(ch) || (ch == '-' && peek().has_value() &&
                                       is_numeric(peek().value()))) {
                    return number();
                }
                if (is_symbol(ch)) return symbol();
                return invalid(absl::StrFormat("invalid token: %c", ch));
            }
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
