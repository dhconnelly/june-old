#ifndef SCANNER_H_
#define SCANNER_H_

#include <string>
#include <vector>

#include "absl/status/statusor.h"
#include "token.h"

class Scanner {
   public:
    // |text| must outlive the constructed Scanner
    Scanner(std::string_view text) {}
};

absl::StatusOr<std::vector<Token>> scan(std::string_view text);

#endif  // SCANNER_H_
