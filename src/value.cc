#include "value.h"

#include "absl/strings/str_format.h"

void BoolValue::serialize_value(std::vector<char>* buf) const {
    buf->push_back(static_cast<char>(value_));
}

void IntValue::serialize_value(std::vector<char>* buf) const {
    unsigned int x = value_;
    buf->push_back(static_cast<char>(x & 0xFF));
    buf->push_back(static_cast<char>((x >> 8) & 0xFF));
    buf->push_back(static_cast<char>((x >> 16) & 0xFF));
    buf->push_back(static_cast<char>((x >> 24) & 0xFF));
    int pos = buf->size();
    absl::PrintF("%x %x %x %x\n", (*buf)[pos - 4], (*buf)[pos - 3],
                 (*buf)[pos - 2], (*buf)[pos - 1]);
}

absl::StatusOr<std::unique_ptr<BoolValue>> BoolValue::deserialize(
    const std::vector<char>& buf, int at) {
    return std::make_unique<BoolValue>(static_cast<bool>(buf[at]));
}

absl::StatusOr<std::unique_ptr<IntValue>> IntValue::deserialize(
    const std::vector<char>& buf, int at) {
    if (at + 4 > buf.size()) {
        return absl::InvalidArgumentError("can't parse int: not enough bytes");
    }
    unsigned int x0 = static_cast<unsigned char>(buf[at++]);
    unsigned int x1 = static_cast<unsigned char>(buf[at++]);
    unsigned int x2 = static_cast<unsigned char>(buf[at++]);
    unsigned int x3 = static_cast<unsigned char>(buf[at++]);
    int int_value = static_cast<int>(x0 + (x1 << 8) + (x2 << 16) + (x3 << 24));
    return std::make_unique<IntValue>(static_cast<int>(int_value));
}

absl::StatusOr<std::unique_ptr<Value>> Value::deserialize(
    const std::vector<char>& buf, int at) {
    char typ = buf[at++];
    switch (static_cast<Type>(typ)) {
        case Type::Bool: return BoolValue::deserialize(buf, at);
        case Type::Int: return IntValue::deserialize(buf, at);
    }
    return absl::InvalidArgumentError(absl::StrFormat("bad type: %d", typ));
}
