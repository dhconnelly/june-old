#include "compiler.h"

absl::Status Compiler::visit(const ExprStmt& es) {
    if (auto status = es.expr().accept(this); !status.ok()) return status;
    if (interactive_) push(Opcode::Print);
    push(Opcode::Pop);
    return absl::OkStatus();
}

absl::Status Compiler::visit(const BoolLiteral& lit) {
    push(Opcode::Push);
    push(BoolValue(lit.value()));
    return absl::OkStatus();
}

absl::Status Compiler::visit(const IntLiteral& lit) {
    push(Opcode::Push);
    push(IntValue(lit.value()));
    return absl::OkStatus();
}

absl::Status Compiler::visit(const SymbolExpr& sym) {
    return absl::UnimplementedError("unimplemented");
}

absl::Status Compiler::visit(const IfExpr& e) {
    // evaluate the condition and jump to the alternate if false
    if (auto status = e.cond().accept(this); !status.ok()) return status;
    push(Opcode::JmpIfNot);
    auto target1 = code_.size();
    // fill this in after we know there the alternate starts
    IntValue(0).serialize_value(&code_);

    // evaluate the consequent and jump over the alternate
    if (auto status = e.cons().accept(this); !status.ok()) return status;
    push(Opcode::Jmp);
    auto target2 = code_.size();
    // fill this in after we know there the alternate starts
    IntValue(0).serialize_value(&code_);

    // evaluate the alternate and fall through
    auto dest1 = code_.size();
    if (auto status = e.alt().accept(this); !status.ok()) return status;
    auto dest2 = code_.size();

    // update the jump destinations
    IntValue(dest1).serialize_value(&code_, target1);
    IntValue(dest2).serialize_value(&code_, target2);

    return absl::OkStatus();
}

absl::Status Compiler::visit(const LetExpr& e) {
    return absl::UnimplementedError("unimplemented");
}

absl::StatusOr<std::vector<char>> Compiler::compile(
    const std::vector<std::unique_ptr<Stmt>>& stmts) {
    code_.clear();
    for (const auto& stmt : stmts) {
        if (auto status = stmt->accept(this); !status.ok()) return status;
    }
    return code_;
}
