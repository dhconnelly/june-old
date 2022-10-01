#ifndef COMPILER_H_
#define COMPILER_H_

#include <vector>

#include "absl/status/statusor.h"
#include "ast.h"

class Compiler {
   public:
    absl::StatusOr<std::vector<char>> compile(const std::vector<Expr>& exprs);
};

#endif  // COMPILER_H_
