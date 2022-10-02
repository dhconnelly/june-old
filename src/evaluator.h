#ifndef EVALUATOR_H_
#define EVALUATOR_H_

#include <functional>

#include "absl/status/status.h"
#include "compiler.h"
#include "parser.h"
#include "scanner.h"
#include "vm.h"

class Evaluator final {
public:
    using ErrorHandler = std::function<void(absl::Status)>;

    Evaluator(ErrorHandler handler) : handler_(handler) {}
    void evaluate(std::string_view text);

    void set_interactive(bool interactive) {
        compiler_.set_interactive(interactive);
    }
    void set_log_tokens(bool log_tokens) { log_tokens_ = log_tokens; }
    void set_log_ast(bool log_ast) { log_ast_ = log_ast; }
    void set_log_code(bool log_code) { log_code_ = log_code; }
    void set_log_vm(bool log_vm) { vm_.set_log(log_vm); }

private:
    ErrorHandler handler_;
    Compiler compiler_;
    VM vm_;
    bool log_tokens_ = false;
    bool log_ast_ = false;
    bool log_code_ = false;
};

#endif  // EVALUATOR_H_
