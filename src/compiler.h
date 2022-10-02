#ifndef COMPILER_H_
#define COMPILER_H_

#include <memory>
#include <vector>

#include "absl/status/statusor.h"
#include "ast.h"

class Compiler {
   public:
    absl::StatusOr<std::vector<char>> compile(
        const std::vector<std::unique_ptr<Stmt>>& exprs);
};

#endif  // COMPILER_H_
