#include "compiler.h"

absl::Status Compiler::visit(const ExprStmt& es) {
    auto result = es.expr().accept(this);
    if (!result.ok()) return result;
    push(Opcode::Pop);
    return absl::OkStatus();
}

absl::Status Compiler::visit(const BoolLiteral& lit) {
    push(Opcode::Push);
    push(lit.value());
    return absl::OkStatus();
}

absl::StatusOr<std::vector<char>> Compiler::compile(
    const std::vector<std::unique_ptr<Stmt>>& stmts) {
    code_.clear();
    for (const auto& stmt : stmts) {
        auto result = stmt->accept(this);
        if (!result.ok()) return result;
    }
    return code_;
}
