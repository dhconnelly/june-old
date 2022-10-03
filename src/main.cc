#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "evaluator.h"

ABSL_FLAG(bool, log_tokens, false, "print tokens after scanning");
ABSL_FLAG(bool, log_ast, false, "print ast after parsing");
ABSL_FLAG(bool, log_code, false, "print bytecode after compiling");
ABSL_FLAG(bool, log_vm, false, "print instructions when executing");

Evaluator build_evaluator(std::function<void(absl::Status)> handler,
                          bool interactive) {
    Evaluator evaluator(handler);
    evaluator.set_interactive(interactive);
    evaluator.set_log_tokens(absl::GetFlag(FLAGS_log_tokens));
    evaluator.set_log_ast(absl::GetFlag(FLAGS_log_ast));
    evaluator.set_log_code(absl::GetFlag(FLAGS_log_code));
    evaluator.set_log_vm(absl::GetFlag(FLAGS_log_vm));
    return evaluator;
}

void die(absl::Status status) {
    std::cerr << status.message() << std::endl;
    exit(EXIT_FAILURE);
}

absl::StatusOr<std::string> read_file(std::string_view path) {
    std::ifstream is(path);
    if (!is) {
        die(absl::UnavailableError(
            absl::StrFormat("can't open %s: %s", path, strerror(errno))));
    }
    // https://stackoverflow.com/a/2602258
    std::stringstream buf;
    buf << is.rdbuf();
    std::string text = buf.str();
    return text;
}

void run(std::string_view path) {
    auto text = read_file(path);
    if (!text.ok()) die(text.status());
    auto eval = build_evaluator(die, false);
    eval.evaluate(text.value());
}

void repl() {
    auto eval = build_evaluator(
        [](absl::Status status) {
            if (absl::IsFailedPrecondition(status)) die(status);
            else absl::FPrintF(stderr, "%s\n", status.message());
        },
        true);
    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        eval.evaluate(line);
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    auto args = absl::ParseCommandLine(argc, argv);
    if (args.size() == 1) repl();
    else if (args.size() == 2) run(args[1]);
    else die(absl::InvalidArgumentError("usage: june <file>"));
}
