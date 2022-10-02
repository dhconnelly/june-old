#ifndef AST_H_
#define AST_H_

#include <string>

#include "absl/strings/str_format.h"

class Expr {
   public:
    virtual ~Expr() {}
    virtual int line() const = 0;
    virtual std::string str() const = 0;
};

std::string print_literal(bool value);

template <typename T>
class Literal : public Expr {
   public:
    Literal(int line, T value) : line_(line), value_(value) {}
    ~Literal() override {}

    const T value() const { return value_; }
    int line() const override { return line_; }
    std::string str() const override { return print_literal(value_); }

   private:
    int line_;
    T value_;
};

#endif  // AST_H_
