#include "vm.h"

#include <cassert>

#include "absl/strings/str_format.h"
#include "instr.h"

absl::Status VM::invalid(std::string_view message) const {
    return absl::InvalidArgumentError(
        absl::StrFormat("[pc=%d] vm: %s", pc_, message));
}

absl::Status VM::push() {
    auto maybe_value = Value::deserialize(*code_, pc_);
    if (!maybe_value.ok()) return maybe_value.status();
    auto value = std::move(maybe_value.value());
    // std::cout << "PUSH " << value->str() << std::endl;
    pc_ += value->size();
    stack_.push_back(std::move(value));
    return absl::OkStatus();
}

absl::Status VM::pop() {
    if (stack_.empty()) return invalid("can't pop empty stack");
    // std::cout << "POP\n";
    stack_.pop_back();
    return absl::OkStatus();
}

absl::Status VM::print() {
    if (stack_.empty()) return invalid("can't print empty stack");
    std::cout << stack_.back()->str() << std::endl;
    return absl::OkStatus();
}

absl::Status VM::step() {
    // std::cout << "pc = " << pc_ << std::endl;
    auto maybe_op = deserialize_opcode((*code_)[pc_++]);
    if (!maybe_op.ok()) return maybe_op.status();
    auto op = maybe_op.value();
    switch (op) {
        case Opcode::Push: return push();
        case Opcode::Pop: return pop();
        case Opcode::Print: return print();
    }
    return invalid(absl::StrFormat("invalid opcode: %d", op));
}

absl::Status VM::execute(const std::vector<char>& code) {
    pc_ = 0;
    code_ = &code;
    while (pc_ < code.size()) {
        auto result = step();
        if (!result.ok()) return result;
    }
    return absl::OkStatus();
}
