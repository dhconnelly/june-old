#ifndef VM_H_
#define VM_H_

#include "absl/status/status.h"
#include "value.h"

class VM final {
public:
    absl::Status execute(const std::vector<char>& code);
    void set_log(bool log) { log_ = log; }

private:
    absl::Status invalid(std::string_view message) const;
    void log(std::string_view message);

    // executive the next instruction
    absl::Status step();

    // opcode handlers
    absl::Status push();
    absl::Status pop();
    absl::Status print();

    bool log_ = false;
    int pc_ = 0;
    const std::vector<char>* code_ = nullptr;
    std::vector<std::unique_ptr<Value>> stack_;
};

#endif  // VM_H_
