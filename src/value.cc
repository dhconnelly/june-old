#include "value.h"

void serialize_value(bool value, std::vector<char>* buf) {
    buf->push_back(static_cast<char>(Type::Bool));
    buf->push_back(static_cast<char>(value));
}
