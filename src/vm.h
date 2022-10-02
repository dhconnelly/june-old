#ifndef VM_H_
#define VM_H_

#include "absl/status/status.h"
#include "value.h"

class VM final {
   public:
    absl::Status execute(const std::vector<char>& code);

   private:
    absl::Status invalid(std::string_view message) const;

    // executive the next instruction
    absl::Status step();

    // opcode handlers
    absl::Status push();
    absl::Status pop();
    absl::Status print();

    int pc_ = 0;
    const std::vector<char>* code_ = nullptr;
    std::vector<std::unique_ptr<Value>> stack_;
};

#endif  // VM_H_
