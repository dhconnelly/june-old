#ifndef SCANNER_H_
#define SCANNER_H_

#include <optional>
#include <string>
#include <vector>

#include "absl/status/statusor.h"
#include "token.h"

absl::StatusOr<std::vector<Token>> scan(std::string_view text);

#endif  // SCANNER_H_
