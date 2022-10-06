#ifndef PARSER_H_
#define PARSER_H_

#include <memory>
#include <optional>
#include <vector>

#include "absl/status/statusor.h"
#include "ast.h"
#include "token.h"

absl::StatusOr<std::vector<Stmt>> parse(const std::vector<Token>& toks);

#endif  // PARSER_H_
