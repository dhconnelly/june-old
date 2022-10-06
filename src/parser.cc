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

class Parser final {
public:
    // |toks| must outlive the constructed Parser
    explicit Parser(const std::vector<Token>& toks) : toks_(toks) {}
    bool at_end() const { return pos_ >= toks_.size(); }

    absl::StatusOr<Stmt> stmt();
    absl::StatusOr<Expr> expr();
    absl::StatusOr<BoolExpr> bool_lit();
    absl::StatusOr<IntExpr> int_lit();
    absl::StatusOr<IfExpr> if_expr();
    absl::StatusOr<LetExpr> let_expr();
    absl::StatusOr<SymbolExpr> symbol_expr();

private:
    std::optional<const Token*> peek(int n = 0) const;
    bool peek_is(TokenType typ, int n = 0) const;
    std::optional<Token> advance();
    absl::StatusOr<Token> match(TokenType typ);

    const std::vector<Token>& toks_;
    int pos_ = 0;
};

std::optional<const Token*> Parser::peek(int n) const {
    return pos_ + n >= toks_.size() ? std::nullopt
                                    : std::optional{&toks_[pos_ + n]};
}

bool Parser::peek_is(TokenType typ, int n) const {
    auto tok = peek(n);
    return tok.has_value() && (*tok)->typ == typ;
}

std::optional<Token> Parser::advance() {
    return at_end() ? std::nullopt : std::optional{toks_[pos_++]};
}

absl::StatusOr<Token> Parser::match(TokenType want) {
    auto tok = peek();
    if (!tok) return unexpected_eof();
    if ((*tok)->typ != want) return invalid(want, **tok);
    return *advance();
}

absl::StatusOr<BoolExpr> Parser::bool_lit() {
    auto tok = match(TokenType::Bool);
    if (!tok.ok()) return tok.status();
    auto bool_value = [&tok]() -> absl::StatusOr<bool> {
        if (tok->cargo == "#t") return true;
        if (tok->cargo == "#f") return false;
        return invalid(TokenType::Bool, *tok);
    }();
    if (!bool_value.ok()) return bool_value.status();
    return BoolExpr{.line = tok->line, .value = *bool_value};
}

absl::StatusOr<IntExpr> Parser::int_lit() {
    auto tok = match(TokenType::Int);
    if (!tok.ok()) return tok.status();
    int int_value;
    try {
        int_value = std::stoi(tok.value().cargo);
    } catch (std::out_of_range& e) {
        return err(tok->line,
                   absl::StrFormat("int out of range: %s", tok->cargo));
    }
    return IntExpr{.line = tok->line, .value = int_value};
}

absl::StatusOr<SymbolExpr> Parser::symbol_expr() {
    auto tok = match(TokenType::Symbol);
    if (!tok.ok()) return tok.status();
    return SymbolExpr{.line = tok->line, .name = tok->cargo};
}

absl::StatusOr<IfExpr> Parser::if_expr() {
    auto tok = match(TokenType::Lparen);
    if (!tok.ok()) return tok.status();
    if (auto tok = match(TokenType::If); !tok.ok()) return tok.status();
    // TODO: improve error handling here
    auto cond = expr();
    if (!cond.ok()) return cond.status();
    auto cons = expr();
    if (!cons.ok()) return cons.status();
    auto alt = expr();
    if (!alt.ok()) return alt.status();
    if (auto tok = match(TokenType::Rparen); !tok.ok()) return tok.status();
    return IfExpr{
        .line = tok->line,
        .cond = std::make_unique<Expr>(*std::move(cond)),
        .conseq = std::make_unique<Expr>(*std::move(cons)),
        .alt = std::make_unique<Expr>(*std::move(alt)),
    };
}

absl::StatusOr<LetExpr> Parser::let_expr() {
    auto tok = match(TokenType::Lparen);
    if (!tok.ok()) return tok.status();
    if (auto tok = match(TokenType::Let); !tok.ok()) return tok.status();

    // bindings
    std::vector<std::pair<std::string, Expr>> bindings;
    if (auto tok = match(TokenType::Lparen); !tok.ok()) return tok.status();
    while (!peek_is(TokenType::Rparen)) {
        if (auto tok = match(TokenType::Lparen); !tok.ok()) return tok.status();
        auto name = match(TokenType::Symbol);
        if (!name.ok()) return name.status();
        auto binding = expr();
        if (!binding.ok()) return binding.status();
        bindings.emplace_back(name->cargo, *std::move(binding));
        if (auto tok = match(TokenType::Rparen); !tok.ok()) return tok.status();
    }
    if (auto tok = match(TokenType::Rparen); !tok.ok()) return tok.status();

    // value
    auto subexpr = expr();
    if (!subexpr.ok()) return subexpr.status();

    if (auto tok = match(TokenType::Rparen); !tok.ok()) return tok.status();
    return LetExpr{
        .line = tok->line,
        .bindings = std::move(bindings),
        .body = std::make_unique<Expr>(*std::move(subexpr)),
    };
}

absl::StatusOr<Expr> Parser::expr() {
    auto tok = peek();
    if (!tok.has_value()) return unexpected_eof();
    switch ((*tok)->typ) {
        case TokenType::Bool: return bool_lit();
        case TokenType::Int: return int_lit();
        case TokenType::Symbol: return symbol_expr();
        case TokenType::Lparen: {
            if (peek_is(TokenType::If, 1)) return if_expr();
            if (peek_is(TokenType::Let, 1)) return let_expr();
        }
        default: return err((*tok)->line, "invalid expr");
    }
}

absl::StatusOr<Stmt> Parser::stmt() { return expr(); }

absl::StatusOr<std::vector<Stmt>> parse(const std::vector<Token>& toks) {
    Parser parse(toks);
    std::vector<Stmt> stmts;
    while (!parse.at_end()) {
        auto st = parse.stmt();
        if (!st.ok()) return st.status();
        stmts.push_back(*std::move(st));
    }
    return stmts;
}
