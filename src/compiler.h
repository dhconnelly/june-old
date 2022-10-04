#ifndef COMPILER_H_
#define COMPILER_H_

#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

#include "absl/status/statusor.h"
#include "ast.h"
#include "instr.h"
#include "value.h"

class Compiler final : public Visitor {
public:
    absl::StatusOr<std::vector<char>> compile(
        const std::vector<std::unique_ptr<Stmt>>& stmts);

    // interactive mode prints the value that is on top of the stack after
    // each statement
    void set_interactive(bool interactive) { interactive_ = interactive; }

    // Visitor:
    absl::Status visit(const ExprStmt& es) override;
    absl::Status visit(const BoolLiteral& lit) override;
    absl::Status visit(const IntLiteral& lit) override;
    absl::Status visit(const IfExpr& e) override;
    absl::Status visit(const LetExpr& e) override;
    absl::Status visit(const SymbolExpr& e) override;

private:
    using Scope = std::map<std::string, int>;

    void push(Opcode op) { serialize_opcode(op, &code_); }
    void push(const Value& value) { value.serialize(&code_); }
    void push_scope() { scopes_.emplace_back(); }
    void pop_scope() {
        if (scopes_.empty()) {
            throw new std::logic_error("compiling at global scope, cannot pop");
        }
        scopes_.pop_back();
    }
    Scope& top_scope() { return scopes_.back(); }

    bool interactive_ = false;
    std::vector<char> code_;
    std::vector<Scope> scopes_;
};

#endif  // COMPILER_H_
