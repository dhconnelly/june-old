#include "parser.h"

absl::StatusOr<Expr> Parser::expr() {
    return absl::UnimplementedError("unimplemented");
}

absl::StatusOr<std::vector<Expr>> parse(const std::vector<Token>& toks) {
    Parser parse(toks);
    std::vector<Expr> exprs;
    while (!parse.at_end()) {
        auto result = parse.expr();
        if (!result.ok()) return result.status();
        exprs.push_back(result.value());
    }
    return exprs;
}
