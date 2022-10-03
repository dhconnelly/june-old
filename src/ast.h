#ifndef AST_H_
#define AST_H_

#include <string>

#include "absl/status/status.h"
#include "absl/strings/str_format.h"

class ExprStmt;
class BoolLiteral;
class IntLiteral;
class IfExpr;

class Visitor {
public:
    virtual absl::Status visit(const ExprStmt&) = 0;
    virtual absl::Status visit(const BoolLiteral&) = 0;
    virtual absl::Status visit(const IntLiteral&) = 0;
    virtual absl::Status visit(const IfExpr&) = 0;
};

class Node {
public:
    Node(int line) : line_(line) {}
    virtual ~Node() {}
    virtual std::string str() const = 0;
    int line() const { return line_; }
    virtual absl::Status accept(Visitor* v) const = 0;

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
    absl::Status accept(Visitor* v) const override { return v->visit(*this); }
    std::string str() const override {
        return absl::StrFormat("ExprStmt(%s)", expr_->str());
    }
    const Expr& expr() const { return *expr_; }

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
    absl::Status accept(Visitor* v) const override { return v->visit(*this); }
    std::string str() const override {
        const char* s = value() ? "true" : "false";
        return absl::StrFormat("BoolLiteral(line=%d, value=%s)", line(), s);
    }
};

class IntLiteral final : public Literal<int> {
public:
    IntLiteral(int line, int value) : Literal(line, value) {}
    absl::Status accept(Visitor* v) const override { return v->visit(*this); }
    std::string str() const override {
        return absl::StrFormat("IntLiteral(line=%d, value=%d)", line(),
                               value());
    }
};

class IfExpr final : public Expr {
public:
    IfExpr(int line, std::unique_ptr<Expr> cond, std::unique_ptr<Expr> cons,
           std::unique_ptr<Expr> alt)
        : Expr(line),
          cond_(std::move(cond)),
          cons_(std::move(cons)),
          alt_(std::move(alt)) {}
    std::string str() const override {
        return absl::StrFormat("IfExpr(cond=%s, cons=%s, alt=%s)", cond_->str(),
                               cons_->str(), alt_->str());
    }
    absl::Status accept(Visitor* v) const override { return v->visit(*this); }

    Expr& cond() const { return *cond_; }
    Expr& cons() const { return *cons_; }
    Expr& alt() const { return *alt_; }

private:
    std::unique_ptr<Expr> cond_;
    std::unique_ptr<Expr> cons_;
    std::unique_ptr<Expr> alt_;
};

#endif  // AST_H_
