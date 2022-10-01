#ifndef PARSER_H_
#define PARSER_H_

#include <vector>

#include "absl/status/statusor.h"
#include "ast.h"
#include "token.h"

class Parser {
   public:
    // |toks| must outlive the constructed Parser
    explicit Parser(const std::vector<Token>& toks) {}
};

absl::StatusOr<std::vector<Expr>> parse(const std::vector<Token>& toks);

#endif  // PARSER_H_
