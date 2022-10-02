#ifndef COMPILER_H_
#define COMPILER_H_

#include <memory>
#include <vector>

#include "absl/status/statusor.h"
#include "ast.h"
#include "instr.h"
#include "value.h"

class Compiler : public Visitor {
   public:
    absl::StatusOr<std::vector<char>> compile(
        const std::vector<std::unique_ptr<Stmt>>& stmts);

    // Visitor:
    absl::Status visit(const ExprStmt& es) override;
    absl::Status visit(const BoolLiteral& lit) override;

   private:
    void push(Opcode op) { serialize_opcode(op, &code_); }
    void push(bool value) { serialize_value(value, &code_); }

    std::vector<char> code_;
};

#endif  // COMPILER_H_
