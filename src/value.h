#ifndef VALUE_H_
#define VALUE_H_

#include <memory>
#include <vector>

#include "absl/status/statusor.h"

enum class Type {
    Bool = 1,
    Int = 2,
};

constexpr const char* to_string(Type typ) {
    switch (typ) {
        case Type::Bool: return "Bool";
        case Type::Int: return "Int";
    }
}

class Value {
public:
    virtual ~Value() {}
    virtual void serialize_value(std::vector<char>* buf) const = 0;
    virtual Type typ() const = 0;
    int size() const { return 1 + value_size(); }
    virtual std::string str() const = 0;
    virtual int value_size() const = 0;
    virtual std::unique_ptr<Value> clone() const = 0;

    void serialize(std::vector<char>* buf) const {
        buf->push_back(static_cast<char>(typ()));
        serialize_value(buf);
    }

    static absl::StatusOr<std::unique_ptr<Value>> deserialize(
        const std::vector<char>& buf, int at);
};

class BoolValue final : public Value {
public:
    BoolValue(bool value) : value_(value) {}
    void serialize_value(std::vector<char>* buf) const override;
    int value_size() const override { return 1; }
    Type typ() const override { return Type::Bool; }
    std::string str() const override { return value_ ? "true" : "false"; }
    bool value() const { return value_; }
    std::unique_ptr<Value> clone() const override {
        return std::make_unique<BoolValue>(value_);
    }

    static constexpr Type static_typ = Type::Bool;

    static absl::StatusOr<std::unique_ptr<BoolValue>> deserialize(
        const std::vector<char>& buf, int at);

private:
    bool value_;
};

class IntValue final : public Value {
public:
    IntValue(int value) : value_(value) {}
    void serialize_value(std::vector<char>* buf) const override;
    void serialize_value(std::vector<char>* buf, int at) const;
    int value_size() const override { return 4; }
    Type typ() const override { return Type::Int; }
    std::string str() const override { return std::to_string(value_); }
    int value() const { return value_; }
    std::unique_ptr<Value> clone() const override {
        return std::make_unique<IntValue>(value_);
    }

    static constexpr Type static_typ = Type::Int;

    static absl::StatusOr<std::unique_ptr<IntValue>> deserialize(
        const std::vector<char>& buf, int at);

private:
    int value_;
};
#endif  // VALUE_H_
