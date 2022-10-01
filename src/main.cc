#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "compiler.h"
#include "parser.h"
#include "scanner.h"
#include "vm.h"

void die(std::string_view reason) {
    std::cerr << reason << std::endl;
    exit(EXIT_FAILURE);
}

absl::StatusOr<std::string> read_file(std::string_view path) {
    std::ifstream is(path);
    if (!is) {
        die(absl::StrFormat("can't open file %s: %s", path, strerror(errno)));
    }
    // https://stackoverflow.com/a/2602258
    std::stringstream buf;
    buf << is.rdbuf();
    std::string text = buf.str();
    absl::PrintF("read %d bytes\n", text.size());
    return text;
}

void run(std::string_view path) {
    auto text = read_file(path);
    auto toks = scan(text.value());
    auto exprs = parse(toks.value());
    Compiler compiler;
    auto code = compiler.compile(exprs.value());
    VM vm;
    if (auto status = vm.execute(code.value()); !status.ok()) {
        die(status.message());
    }
}

void repl() {
    Compiler compiler;
    VM vm;
    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        auto toks = scan(line);
        auto exprs = parse(toks.value());
        auto code = compiler.compile(exprs.value());
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
