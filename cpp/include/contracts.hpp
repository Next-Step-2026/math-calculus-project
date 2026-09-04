#pragma once

#include <cassert>
#include <stdexcept>
#include <string>

namespace contracts {

class PreconditionError : public std::invalid_argument {
    using std::invalid_argument::invalid_argument;
};

class PostconditionError : public std::logic_error {
    using std::logic_error::logic_error;
};

class InvariantError : public std::logic_error {
    using std::logic_error::logic_error;
};

// Precondition: Caller obligation
inline void Expects(bool condition, const std::string& message = "Precondition failed") {
    if (!condition) {
        throw PreconditionError(message);
    }
}

// Postcondition: Supplier guarantee
inline void Ensures(bool condition, const std::string& message = "Postcondition failed") {
    if (!condition) {
        throw PostconditionError(message);
    }
}

// Invariant: Object/State integrity guarantee
inline void Invariant(bool condition, const std::string& message = "Invariant failed") {
    if (!condition) {
        throw InvariantError(message);
    }
}

} // namespace contracts

#if defined(__ESBMC__) || defined(__ESBMC)
  // ESBMC intrinsic: __ESBMC_assume restricts symbolic state space
  #define CONTRACT_ASSUME(cond) __ESBMC_assume(cond)
  #define CONTRACT_ASSERT(cond) assert(cond)
#else
  // Standard runtime verification
  #define CONTRACT_ASSUME(cond) assert(cond)
  #define CONTRACT_ASSERT(cond) assert(cond)
#endif
