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
    log("PUSH");
    auto value = read_typed_static<Value>();
    if (!value.ok()) return value.status();
    log(absl::StrFormat(": [%s]", (*value)->str()));
    stack_.push_back(*std::move(value));
    return absl::OkStatus();
}

absl::Status VM::pop() {
    log("POP");
    auto val = pop_stack<Value>();
    log(absl::StrFormat("-> [%s]", (*val)->str()));
    return val.status();
}

absl::Status VM::print() {
    log("PRINT");
    if (stack_.empty()) return invalid("can't print empty stack");
    absl::PrintF("%s\n", stack_.back()->str());
    return absl::OkStatus();
}

absl::Status VM::jmp() {
    log("JMP");
    auto dest = read_static<IntValue>();
    if (!dest.ok()) return dest.status();
    log(absl::StrFormat(": [%s]", (*dest)->str()));
    pc_ = (*dest)->value();
    return absl::OkStatus();
}

absl::Status VM::jmp_if_not() {
    log("JMP_IF_NOT");
    auto dest = read_static<IntValue>();
    if (!dest.ok()) return dest.status();
    log(absl::StrFormat(": [%s]", (*dest)->str()));
    auto cond = pop_stack<BoolValue>();
    if (!cond.ok()) return cond.status();
    log(absl::StrFormat("-> [%s]", (*cond)->str()));
    if ((*cond)->value()) return absl::OkStatus();
    pc_ = (*dest)->value();
    return absl::OkStatus();
}

absl::Status VM::swap() {
    log("SWAP");
    auto val1 = pop_stack<Value>();
    if (!val1.ok()) return val1.status();
    log(absl::StrFormat("-> [%s]", (*val1)->str()));
    auto val2 = pop_stack<Value>();
    if (!val2.ok()) return val2.status();
    log(absl::StrFormat("-> [%s]", (*val2)->str()));
    push_stack(*std::move(val1));
    push_stack(*std::move(val2));
    return absl::OkStatus();
}

absl::Status VM::get() {
    log("GET");
    auto n = read_static<IntValue>();
    if (!n.ok()) return n.status();
    auto val = stack_get((*n)->value());
    if (!val.has_value()) {
        return absl::FailedPreconditionError("stack offset out of bounds");
    }
    push_stack(*std::move(val));
    return absl::OkStatus();
}

absl::Status VM::step() {
    log(absl::StrFormat("< stack: %d >", stack_size()));
    instr_pc_ = pc_;
    auto op = deserialize_opcode((*code_)[pc_++]);
    if (!op.ok()) return invalid(op.status().message());
    switch (*op) {
        case Opcode::Push: return push();
        case Opcode::Pop: return pop();
        case Opcode::Print: return print();
        case Opcode::Jmp: return jmp();
        case Opcode::JmpIfNot: return jmp_if_not();
        case Opcode::Swap: return swap();
        case Opcode::Get: return get();
    }
    return invalid(absl::StrFormat("unsupported opcode: %d", *op));
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
