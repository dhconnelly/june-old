#include "ast.h"

#include <algorithm>

#include "absl/strings/str_join.h"

struct Printer {
    std::string operator()(const Expr& e) const { return to_string(e); }

    std::string operator()(const IntExpr& e) const {
        return std::to_string(e.value);
    }

    std::string operator()(const BoolExpr& e) const {
        return std::to_string(e.value);
    }

    std::string operator()(const SymbolExpr& e) const {
        return "Symbol(" + e.name + ")";
    }

    std::string operator()(const IfExpr& e) const {
        return "If(" + to_string(*e.cond) + ", " + to_string(*e.conseq) + ", " +
               to_string(*e.alt) + ")";
    }

    std::string operator()(const LetExpr& e) const {
        std::string s = "Let(";
        for (const auto& [k, v] : e.bindings) {
            s.append("[" + k + " -> " + to_string(v) + "]");
        }
        return s + ")";
    }
};

std::string to_string(const Expr& expr) {
    static const Printer printer;
    return std::visit(printer, expr);
}

std::string to_string(const Stmt& stmt) {
    static const Printer printer;
    return std::visit(printer, stmt);
}
