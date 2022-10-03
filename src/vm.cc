#include "vm.h"

#include <cassert>

#include "absl/strings/str_format.h"
#include "instr.h"

absl::Status VM::invalid(std::string_view message) const {
    return absl::InvalidArgumentError(
        absl::StrFormat("[pc=%d] vm: %s", instr_pc_, message));
}

absl::Status VM::type_error(Type want, Type got) const {
    return invalid(absl::StrFormat("type error: want %s, got %s",
                                   to_string(want), to_string(got)));
}

absl::Status VM::check_type(const Value& value, Type want) const {
    return value.typ() == want ? absl::OkStatus()
                               : type_error(want, value.typ());
}

absl::Status VM::precondition_failed(std::string_view message) const {
    return absl::FailedPreconditionError(
        absl::StrFormat("[pc=%d] vm: %s", instr_pc_, message));
}

absl::Status VM::push() {
    auto value = read_typed_static<Value>();
    if (!value.ok()) return value.status();
    log("PUSH " + value.value()->str());
    stack_.push_back(std::move(value.value()));
    return absl::OkStatus();
}

absl::Status VM::pop() {
    auto val = pop_stack<Value>();
    if (val.ok()) log("POP");
    return val.status();
}

absl::Status VM::print() {
    if (stack_.empty()) return invalid("can't print empty stack");
    log("PRINT");
    absl::PrintF("%s\n", stack_.back()->str());
    return absl::OkStatus();
}

absl::Status VM::jmp() {
    auto dest = read_static<IntValue>();
    if (!dest.ok()) return dest.status();
    log("JMP " + dest.value()->str());
    pc_ = dest.value()->value();
    return absl::OkStatus();
}

absl::Status VM::jmp_if_not() {
    auto cond = pop_stack<BoolValue>();
    if (!cond.ok()) return cond.status();
    auto dest = read_static<IntValue>();
    if (!dest.ok()) return dest.status();
    log("JMP_IF_NOT " + dest.value()->str());
    if (cond.value()->value()) return absl::OkStatus();
    pc_ = dest.value()->value();
    return absl::OkStatus();
}

absl::Status VM::step() {
    instr_pc_ = pc_;
    auto op = deserialize_opcode((*code_)[pc_++]);
    if (!op.ok()) return invalid(op.status().message());
    switch (op.value()) {
        case Opcode::Push: return push();
        case Opcode::Pop: return pop();
        case Opcode::Print: return print();
        case Opcode::Jmp: return jmp();
        case Opcode::JmpIfNot: return jmp_if_not();
    }
    return invalid(absl::StrFormat("unsupported opcode: %d", op.value()));
}

void VM::log(std::string_view msg) const {
    if (log_) absl::PrintF("%4d\t%s\n", instr_pc_, msg);
}

absl::Status VM::execute(const std::vector<char>& code) {
    pc_ = 0;
    code_ = &code;
    while (pc_ < code.size()) {
        if (auto status = step(); !status.ok()) return status;
    }
    return absl::OkStatus();
}
