#ifndef AST_H_
#define AST_H_

#include <string>
#include <utility>
#include <variant>
#include <vector>

struct BoolExpr;
struct IntExpr;
struct SymbolExpr;
struct IfExpr;
struct LetExpr;

using Expr = std::variant<BoolExpr, IntExpr, SymbolExpr, IfExpr, LetExpr>;

struct BoolExpr {
    int line;
    bool value;
};

struct IntExpr {
    int line;
    int value;
};

struct SymbolExpr {
    int line;
    std::string name;
};

struct IfExpr {
    int line;
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Expr> conseq;
    std::unique_ptr<Expr> alt;
};

struct LetExpr {
    int line;
    std::vector<std::pair<std::string, Expr>> bindings;
    std::unique_ptr<Expr> body;
};

using Stmt = std::variant<Expr>;

std::string to_string(const Expr& expr);
std::string to_string(const Stmt& stmt);

#endif  // AST_H_
