#include "ast.h"

#include "absl/strings/str_join.h"

std::string ExprStmt::str() const {
    return absl::StrFormat("ExprStmt(%s)", expr_->str());
}

std::string BoolLiteral::str() const {
    const char* s = value() ? "true" : "false";
    return absl::StrFormat("BoolLiteral(line=%d, value=%s)", line(), s);
}

std::string IntLiteral::str() const {
    return absl::StrFormat("IntLiteral(line=%d, value=%d)", line(), value());
}

std::string IfExpr::str() const {
    return absl::StrFormat("IfExpr(cond=%s, cons=%s, alt=%s)", cond_->str(),
                           cons_->str(), alt_->str());
}

std::string LetExpr::str() const {
    std::vector<std::string> bindings;
    for (const auto& [name, val] : bindings_) {
        bindings.push_back(absl::StrFormat("[\"%s\" -> %s]", name, val->str()));
    }
    auto bs = absl::StrFormat("[%s]", absl::StrJoin(bindings, ", "));
    return absl::StrFormat("IfExpr(bindings=%s, subexpr=%s)", bs,
                           subexpr_->str());
}
