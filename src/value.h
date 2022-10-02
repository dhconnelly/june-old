#ifndef VALUE_H_
#define VALUE_H_

#include <memory>
#include <vector>

#include "absl/status/statusor.h"

enum class Type {
    Bool = 1,
};

class Value {
   public:
    virtual ~Value() {}
    virtual void serialize(std::vector<char>* buf) const = 0;
    virtual Type typ() const = 0;
    int size() const { return 1 + value_size(); }
    static absl::StatusOr<std::unique_ptr<Value>> deserialize(
        const std::vector<char>& buf, int at);
    virtual std::string str() const = 0;

   protected:
    virtual int value_size() const = 0;
};

class BoolValue final : public Value {
   public:
    BoolValue(bool value) : value_(value) {}
    void serialize(std::vector<char>* buf) const override;
    int value_size() const override { return 1; }
    Type typ() const override { return Type::Bool; }
    std::string str() const override { return value_ ? "true" : "false"; }

   private:
    bool value_;
};

#endif  // VALUE_H_
