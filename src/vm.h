#ifndef VM_H_
#define VM_H_

#include "absl/status/status.h"

class VM {
   public:
    absl::Status execute(const std::vector<char>& buf);
};

#endif  // VM_H_
