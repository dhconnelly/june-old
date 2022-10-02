#include "instr.h"

void serialize_opcode(Opcode op, std::vector<char>* buf) {
    buf->push_back(static_cast<char>(op));
}
