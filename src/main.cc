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

void report(absl::Status status) {
    if (!status.ok()) std::cerr << status.message() << std::endl;
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
    auto stmts = parse(toks.value());
    Compiler compiler;
    auto code = compiler.compile(stmts.value());
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
        if (!toks.ok()) {
            report(toks.status());
            continue;
        }
        for (const auto& tok : toks.value()) absl::PrintF("%s\n", tok.str());

        auto stmts = parse(toks.value());
        if (!stmts.ok()) {
            report(stmts.status());
            continue;
        }
        for (const auto& stmt : stmts.value()) {
            absl::PrintF("%s\n", stmt->str());
        }

        auto code = compiler.compile(stmts.value());
        if (!code.ok()) {
            report(code.status());
            continue;
        }
        for (const char ch : code.value()) {
            absl::PrintF("0x%02x\n", ch);
        }

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
