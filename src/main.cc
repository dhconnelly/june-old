#include <iostream>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

using Buf = std::vector<char>;
using Expr = int;

void die(std::string_view reason) {
    std::cerr << reason << std::endl;
    exit(EXIT_FAILURE);
}

class VM {
   public:
    absl::Status execute(const Buf& buf) { return absl::OkStatus(); }
};

absl::StatusOr<std::string> read_file(std::string_view path) { return ""; }

absl::StatusOr<std::vector<Expr>> parse(std::string_view text) {
    return std::vector<Expr>{};
}

absl::StatusOr<std::vector<char>> compile(const std::vector<Expr>& exprs) {
    return std::vector<char>{};
}

void run(std::string_view path) {
    auto text = read_file(path);
    auto exprs = parse(text.value());
    auto code = compile(exprs.value());
    VM vm;
    if (auto status = vm.execute(code.value()); !status.ok()) {
        die(status.message());
    }
}

void repl() {
    VM vm;
    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        auto exprs = parse(line);
        auto code = compile(exprs.value());
        if (auto status = vm.execute(code.value()); !status.ok()) {
            die(status.message());
        }
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc == 1) repl();
    else if (argc == 2) run(argv[1]);
    else die("usage: june <file>");
}
