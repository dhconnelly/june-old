#include "evaluator.h"

void Evaluator::evaluate(std::string_view text) {
    auto toks = scan(text);
    if (!toks.ok()) {
        handler_(toks.status());
        return;
    }
    if (log_tokens_) {
        for (const auto& tok : toks.value()) absl::PrintF("%s\n", tok.str());
    }
    auto stmts = parse(toks.value());
    if (!stmts.ok()) {
        handler_(stmts.status());
        return;
    }
    if (log_ast_) {
        for (const auto& st : stmts.value()) absl::PrintF("%s\n", st->str());
    }
    auto code = compiler_.compile(stmts.value());
    if (!code.ok()) {
        handler_(code.status());
        return;
    }
    if (log_code_) {
        for (char ch : code.value()) absl::PrintF("0x%02X\n", ch);
    }
    auto status = vm_.execute(code.value());
    if (!status.ok()) {
        handler_(code.status());
    }
}
