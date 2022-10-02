#include "parser.h"

#include <stdexcept>

#include "absl/strings/str_format.h"

namespace {
absl::Status unexpected_eof() {
    return absl::InvalidArgumentError("unexpected eof");
}

absl::Status err(int line, std::string_view message) {
    auto s = absl::StrFormat("[line %d] parser: %s", line, message);
    return absl::InvalidArgumentError(s);
}

absl::Status invalid(TokenType want, const Token& tok) {
    auto s = absl::StrFormat("want %s, got %s", to_string(want), tok.cargo);
    return err(tok.line, s);
}
}  // namespace

std::optional<const Token*> Parser::peek() const {
    return at_end() ? std::nullopt : std::optional{&toks_[pos_]};
}

std::optional<Token> Parser::advance() {
    return at_end() ? std::nullopt : std::optional{toks_[pos_++]};
}

absl::StatusOr<Token> Parser::match(TokenType want) {
    auto tok = peek();
    if (!tok) return unexpected_eof();
    if (tok.value()->typ != want) return invalid(want, *tok.value());
    return advance().value();
}

absl::StatusOr<std::unique_ptr<BoolLiteral>> Parser::bool_lit() {
    auto tok = match(TokenType::Bool);
    if (!tok.ok()) return tok.status();
    auto bool_value = [&tok]() -> absl::StatusOr<bool> {
        if (tok->cargo == "#t") return true;
        if (tok->cargo == "#f") return false;
        return invalid(TokenType::Bool, *tok);
    }();
    if (!bool_value.ok()) return bool_value.status();
    return std::make_unique<BoolLiteral>(tok->line, bool_value.value());
}

absl::StatusOr<std::unique_ptr<IntLiteral>> Parser::int_lit() {
    auto tok = match(TokenType::Int);
    if (!tok.ok()) return tok.status();
    int int_value;
    try {
        int_value = std::stoi(tok.value().cargo);
    } catch (std::out_of_range& e) {
        return err(tok->line,
                   absl::StrFormat("int out of range: %s", tok->cargo));
    }
    return std::make_unique<IntLiteral>(tok->line, int_value);
}

absl::StatusOr<std::unique_ptr<Expr>> Parser::expr() {
    auto tok = peek();
    if (!tok) return unexpected_eof();
    switch (tok.value()->typ) {
        case TokenType::Bool: return bool_lit();
        case TokenType::Int: return int_lit();
    }
    return err(tok.value()->line, "invalid expr");
}

absl::StatusOr<std::unique_ptr<Stmt>> Parser::stmt() {
    auto e = expr();
    if (!e.ok()) return e.status();
    return std::make_unique<ExprStmt>(std::move(e.value()));
}

absl::StatusOr<std::vector<std::unique_ptr<Stmt>>> parse(
    const std::vector<Token>& toks) {
    Parser parse(toks);
    std::vector<std::unique_ptr<Stmt>> stmts;
    while (!parse.at_end()) {
        auto st = parse.stmt();
        if (!st.ok()) return st.status();
        stmts.push_back(std::move(st.value()));
    }
    return stmts;
}
