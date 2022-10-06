#include "evaluator.h"

#include "absl/strings/str_format.h"

void Evaluator::evaluate(std::string_view text) {
    auto toks = scan(text);
    if (!toks.ok()) {
        handler_(toks.status());
        return;
    }
    if (log_tokens_) {
        for (const auto& tok : *toks) absl::PrintF("%s\n", tok.str());
    }

    auto stmts = parse(toks.value());
    if (!stmts.ok()) {
        handler_(stmts.status());
        return;
    }
    if (log_ast_) {
        for (const auto& stmt : *stmts) absl::PrintF("%s\n", to_string(stmt));
    }

    auto code = compiler_.compile(stmts.value());
    if (!code.ok()) {
        handler_(code.status());
        return;
    }
    if (log_code_) {
        for (char ch : *code) absl::PrintF("0x%02X\n", ch);
    }

    auto status = vm_.execute(*code);
    if (!status.ok()) {
        handler_(status);
    }
}
