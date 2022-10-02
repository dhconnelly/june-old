#ifndef AST_H_
#define AST_H_

#include <string>

#include "absl/strings/str_format.h"

class Node {
   public:
    Node(int line) : line_(line) {}
    virtual ~Node() {}
    virtual std::string str() const = 0;
    int line() const { return line_; }

   private:
    int line_;
};

class Stmt : public Node {
   public:
    Stmt(int line) : Node(line) {}
};

class Expr : public Node {
   public:
    Expr(int line) : Node(line) {}
};

class ExprStmt final : public Stmt {
   public:
    explicit ExprStmt(std::unique_ptr<Expr> expr)
        : Stmt(expr->line()), expr_(std::move(expr)) {}
    std::string str() const override {
        return absl::StrFormat("ExprStmt(%s)", expr_->str());
    }

   private:
    std::unique_ptr<Expr> expr_;
};

template <typename T>
class Literal : public Expr {
   public:
    Literal(int line, T value) : Expr(line), value_(value) {}
    const T value() const { return value_; }

   private:
    T value_;
};

class BoolLiteral final : public Literal<bool> {
   public:
    BoolLiteral(int line, bool value) : Literal(line, value) {}
    std::string str() const {
        const char* s = value() ? "true" : "false";
        return absl::StrFormat("BoolLiteral(line=%d, value=%s)", line(), s);
    }
};

#endif  // AST_H_
