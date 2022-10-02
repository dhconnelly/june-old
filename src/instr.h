#ifndef INSTR_H_
#define INSTR_H_

#include <utility>
#include <vector>

#include "absl/status/statusor.h"

enum class Opcode {
    // [Push Value]
    Push = 1,
    // [Pop]
    Pop = 2,
};

void serialize_opcode(Opcode op, std::vector<char>* buf);
absl::StatusOr<Opcode> deserialize_opcode(char ch);

#endif  // INSTR_H_
