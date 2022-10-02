#include "compiler.h"

absl::Status Compiler::visit(const ExprStmt& es) {
    if (auto status = es.expr().accept(this); !status.ok()) return status;
    if (interactive_) push(Opcode::Print);
    push(Opcode::Pop);
    return absl::OkStatus();
}

absl::Status Compiler::visit(const BoolLiteral& lit) {
    push(Opcode::Push);
    push(lit.value());
    return absl::OkStatus();
}

absl::Status Compiler::visit(const IntLiteral& lit) {
    push(Opcode::Push);
    push(lit.value());
    return absl::OkStatus();
}

absl::Status Compiler::visit(const IfExpr& e) { exit(1); }

absl::StatusOr<std::vector<char>> Compiler::compile(
    const std::vector<std::unique_ptr<Stmt>>& stmts) {
    code_.clear();
    for (const auto& stmt : stmts) {
        if (auto status = stmt->accept(this); !status.ok()) return status;
    }
    return code_;
}
