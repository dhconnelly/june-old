#include "instr.h"

#include "absl/strings/str_format.h"

void serialize_opcode(Opcode op, std::vector<char>* buf) {
    buf->push_back(static_cast<char>(op));
}

absl::StatusOr<Opcode> deserialize_opcode(char op) {
    switch (op) {
        case 1: return Opcode::Push;
        case 2: return Opcode::Pop;
    }
    return absl::InvalidArgumentError(absl::StrFormat("bad opcode: %d", op));
}
