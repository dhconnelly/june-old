#ifndef SCANNER_H_
#define SCANNER_H_

#include <optional>
#include <string>
#include <vector>

#include "absl/status/statusor.h"
#include "token.h"

class Scanner {
   public:
    // |text| must outlive the constructed Scanner
    Scanner(std::string_view text) : text_(text) {}
    absl::StatusOr<std::optional<Token>> next();
    bool at_end() const { return pos_ >= text_.size(); }

   private:
    absl::Status invalid(std::string_view message) const;
    Token token(TokenType typ) const;

    std::optional<char> peek() const;
    std::optional<char> advance();

    int start_ = 0;
    int pos_ = 0;
    int line_ = 1;
    std::string_view text_;
};

absl::StatusOr<std::vector<Token>> scan(std::string_view text);

#endif  // SCANNER_H_
