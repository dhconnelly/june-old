#include "compiler.h"

absl::StatusOr<std::vector<char>> Compiler::compile(
    const std::vector<std::unique_ptr<Expr>>& exprs) {
    return std::vector<char>{};
}
