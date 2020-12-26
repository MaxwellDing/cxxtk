/*************************************************************************
 * Copyright (C) [2020] by MaxwellDing. All rights reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *************************************************************************/

#ifndef TK_UTIL_CONNECT_HH_
#define TK_UTIL_CONNECT_HH_

#include <functional>
#include <future>
#include <list>
#include <memory>

#include "util/crtp.hh"

#define emit
#define signals public
#define connect(sender, signal, slot) ((sender)->signal.Bind(slot))

namespace tk {
namespace util {
namespace detail {

enum class SignalPolicy {
  SYNC,
  ASYNC
};

template <class Derived>
class SlotBase : public crtp<Derived, SlotBase> {
 public:
  template <typename... RArgs>
  void Run(RArgs&&... args) {
    this->underlying().Exec(std::forward<RArgs>(args)...);
  }
  virtual ~SlotBase() = default;
};

template<SignalPolicy policy, typename... Args>
class Slot : public SlotBase<Slot<policy, Args...>> {
 public:
  using OnFunc = std::function<void(Args...)>;
  Slot(OnFunc&& func) noexcept : func_(std::forward<OnFunc>(func)) {}

  template <typename... RArgs, typename = std::enable_if_t<std::is_invocable_v<OnFunc, RArgs...>>>
  void Exec(RArgs&&... args) {
    func_(std::forward<RArgs>(args)...);
  }

 private:
  OnFunc func_;
};

template<typename... Args>
class Slot<SignalPolicy::ASYNC, Args...> : public SlotBase<Slot<SignalPolicy::ASYNC, Args...>> {
 public:
  using OnFunc = std::function<void(Args...)>;
  Slot(OnFunc&& func) noexcept : func_(std::forward<OnFunc>(func)) {}

  template <typename... RArgs, typename = std::enable_if_t<std::is_invocable_v<OnFunc, RArgs...>>>
  void Exec(RArgs&&... args) {
    if (ret_.valid()) ret_.get();
    ret_ = std::async(std::launch::async, func_, std::forward<RArgs>(args)...);
  }

 private:
  OnFunc func_;
  std::future<void> ret_;
};

template<SignalPolicy policy, typename... Args>
class Signal {
 public:
  using SlotPtr = std::shared_ptr<SlotBase<Slot<policy, Args...>>>;

  template <typename Callable, typename = std::enable_if_t<std::is_invocable_v<Callable, Args...>>>
  void Bind(Callable&& func) {
    slots_.emplace_back(new Slot<policy, Args...>(std::forward<Callable>(func)));
  }

  template <typename... RArgs, typename = std::enable_if_t<(std::is_convertible_v<RArgs, Args> && ...)>>
  void operator()(RArgs&&... args) {
    for (auto& iter : slots_) {
      iter->Run(std::forward<RArgs>(args)...);
    }
  }

 private:
  std::list<SlotPtr> slots_;
};

}  // namespace detail

template<typename... Args>
using SyncSignal = detail::Signal<detail::SignalPolicy::SYNC, Args...>;

template<typename... Args>
using AsyncSignal = detail::Signal<detail::SignalPolicy::ASYNC, Args...>;

}  // namespace util
}  // namespace tk

#endif  // TK_UTIL_CONNECT_HH_

