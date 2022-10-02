#ifndef INSTR_H_
#define INSTR_H_

#include <vector>

enum class Opcode {
    // [Push Value]
    Push = 1,
    // [Pop]
    Pop = 2,
};

void serialize_opcode(Opcode op, std::vector<char>* buf);

#endif  // INSTR_H_
