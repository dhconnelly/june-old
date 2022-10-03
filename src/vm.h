#ifndef VM_H_
#define VM_H_

#include <type_traits>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "value.h"

class VM final {
public:
    absl::Status execute(const std::vector<char>& code);
    void set_log(bool log) { log_ = log; }

private:
    absl::Status invalid(std::string_view message) const;
    absl::Status type_error(Type want, Type got) const;
    absl::Status check_type(const Value& value, Type want) const;
    absl::Status precondition_failed(std::string_view message) const;
    void log(std::string_view message) const;

    // executive the next instruction
    absl::Status step();

    template <typename T>
    absl::StatusOr<std::unique_ptr<T>> downcast(std::unique_ptr<Value> value) {
        Value* base = value.get();
        T* derived = dynamic_cast<T*>(base);
        if (derived == nullptr) return type_error(T::static_typ, base->typ());
        return std::unique_ptr<T>(dynamic_cast<T*>(value.release()));
    }

    // pop the next value from the stack
    template <typename T>
    absl::StatusOr<std::unique_ptr<T>> pop_stack() {
        if (stack_.empty()) return invalid("can't pop empty stack");
        auto value = std::move(stack_.back());
        stack_.pop_back();
        if constexpr (std::is_base_of<T, Value>()) return value;
        else return downcast<T>(std::move(value));
    }

    // read the next static value from code
    template <typename T>
    absl::StatusOr<std::unique_ptr<T>> read_typed_static() {
        auto value = Value::deserialize(*code_, pc_);
        if (value.ok()) pc_ += (*value)->size();
        if constexpr (std::is_base_of<T, Value>()) return value;
        else return downcast<T>(std::move(value.value()));
    }
    template <typename T>
    absl::StatusOr<std::unique_ptr<T>> read_static() {
        auto value = T::deserialize(*code_, pc_);
        if (value.ok()) pc_ += (*value)->value_size();
        return value;
    }

    // opcode handlers
    absl::Status push();
    absl::Status pop();
    absl::Status print();
    absl::Status jmp();
    absl::Status jmp_if_not();

    bool log_ = false;
    int instr_pc_ = 0;
    int pc_ = 0;
    const std::vector<char>* code_ = nullptr;

    // TODO: keep the values directly in the stack, not via pointers
    std::vector<std::unique_ptr<Value>> stack_;
};

#endif  // VM_H_
