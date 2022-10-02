#ifndef PARSER_H_
#define PARSER_H_

#include <memory>
#include <optional>
#include <vector>

#include "absl/status/statusor.h"
#include "ast.h"
#include "token.h"

class Parser {
   public:
    // |toks| must outlive the constructed Parser
    explicit Parser(const std::vector<Token>& toks) : toks_(toks) {}
    bool at_end() const { return pos_ >= toks_.size(); }

    absl::StatusOr<std::unique_ptr<Stmt>> stmt();
    absl::StatusOr<std::unique_ptr<Expr>> expr();
    absl::StatusOr<std::unique_ptr<BoolLiteral>> bool_lit();

   private:
    std::optional<const Token*> peek() const;
    std::optional<Token> advance();
    absl::StatusOr<Token> match(TokenType typ);

    const std::vector<Token>& toks_;
    int pos_ = 0;
};

absl::StatusOr<std::vector<std::unique_ptr<Stmt>>> parse(
    const std::vector<Token>& toks);

#endif  // PARSER_H_
