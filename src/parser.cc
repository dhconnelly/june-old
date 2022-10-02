#include "parser.h"

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
    if (at_end()) return {};
    return &toks_[pos_];
}

std::optional<Token> Parser::advance() {
    if (at_end()) return {};
    return toks_[pos_++];
}

absl::StatusOr<Token> Parser::match(TokenType want) {
    auto result = peek();
    if (!result) return unexpected_eof();
    if (result.value()->typ != want) return invalid(want, *result.value());
    return advance().value();
}

absl::StatusOr<std::unique_ptr<BoolLiteral>> Parser::bool_lit() {
    auto result = match(TokenType::Bool);
    if (!result.ok()) return result.status();
    auto tok = result.value();
    bool value;
    if (tok.cargo == "#t") value = true;
    else if (tok.cargo == "#f") value = false;
    else return invalid(TokenType::Bool, tok);
    return std::make_unique<BoolLiteral>(result.value().line, value);
}

absl::StatusOr<std::unique_ptr<Expr>> Parser::expr() {
    auto result = peek();
    if (!result) return unexpected_eof();
    const auto& tok = *result.value();
    switch (tok.typ) {
        case TokenType::Bool: return bool_lit();
    }
    return err(tok.line, "invalid expr");
}

absl::StatusOr<std::unique_ptr<Stmt>> Parser::stmt() {
    auto result = expr();
    if (!result.ok()) return result.status();
    return std::make_unique<ExprStmt>(std::move(result.value()));
}

absl::StatusOr<std::vector<std::unique_ptr<Stmt>>> parse(
    const std::vector<Token>& toks) {
    Parser parse(toks);
    std::vector<std::unique_ptr<Stmt>> exprs;
    while (!parse.at_end()) {
        auto result = parse.stmt();
        if (!result.ok()) return result.status();
        exprs.push_back(std::move(result.value()));
    }
    return exprs;
}
