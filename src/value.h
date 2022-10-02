#ifndef VALUE_H_
#define VALUE_H_

#include <vector>

enum class Type {
    Bool = 1,
};

void serialize_value(bool value, std::vector<char>* buf);

#endif  // VALUE_H_
