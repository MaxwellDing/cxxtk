#pragma once

#include <type_traits>
#include <utility>

#include "tk/util/noncopy.hpp"

namespace tk {
namespace util {

// A helper function to help run a lambda at the start.
class Register {
 public:
  template <typename F>
  explicit Register(F&& func) { func(); }
};

template <typename F>
class FinalAction : public Noncopy {
 public:
  explicit FinalAction(F func)
    : func_(std::move(func)), enable_{true} {}
  ~FinalAction() {
    if (enable_) func_();
  }
  FinalAction(FinalAction&& rv) : func_(std::move(rv.func_)), enable_(true) {
    rv.enable_ = false;
  }
  FinalAction& operator=(FinalAction&& rhs) = delete;

  void disable() { enable_ = false; }

 private:
  F func_;
  bool enable_;
};

template <typename F>
FinalAction<F> finally(F const& f) {
  return FinalAction<F>(f);
}

template <typename F>
FinalAction<F> finally(F&& f) {
  return FinalAction<F>(std::forward<F>(f));
}

}}  // namespace tk::util::

