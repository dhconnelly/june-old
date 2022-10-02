#include "ast.h"

std::string print_literal(bool value) {
    return absl::StrFormat("Literal<bool>(value=%s)", value ? "true" : "false");
}
