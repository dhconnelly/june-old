#include "value.h"

#include "absl/strings/str_format.h"

void BoolValue::serialize(std::vector<char>* buf) const {
    buf->push_back(static_cast<char>(Type::Bool));
    buf->push_back(static_cast<char>(value_));
}

absl::StatusOr<std::unique_ptr<Value>> Value::deserialize(
    const std::vector<char>& buf, int at) {
    char typ = buf[at++];
    switch (typ) {
        case 1: return std::make_unique<BoolValue>(static_cast<bool>(buf[at]));
    }
    return absl::InvalidArgumentError(absl::StrFormat("bad type: %d", typ));
}
