#ifndef COMPILER_H_
#define COMPILER_H_

#include <memory>
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

private:
    void push(Opcode op) { serialize_opcode(op, &code_); }
    void push(bool value) { BoolValue(value).serialize(&code_); }
    void push(int value) { IntValue(value).serialize(&code_); }

    bool interactive_ = false;
    std::vector<char> code_;
};

#endif  // COMPILER_H_
