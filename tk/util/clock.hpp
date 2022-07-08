#pragma once

#include <chrono>
#include <limits>

namespace util {
template <typename T>
struct is_ratio : std::false_type {};

template <std::intmax_t Num, std::intmax_t Denom>
struct is_ratio<std::ratio<Num, Denom>> : std::true_type {};

/// helper class for std::chrono::steady_clock
class Clock {
 public:
  using clock = std::chrono::steady_clock;
  using time_point = decltype(clock::now());

  static inline time_point now() { return clock::now(); }

  template <typename Ratio = std::milli>
  static inline float duration(const time_point& start, const time_point& end) {
    static_assert(util::is_ratio<Ratio>::value, "Ratio is not specialization of std::ratio");
    return std::chrono::duration<float, Ratio>(end - start).count();
  }

  template <typename Ratio = std::milli>
  static inline float durationSince(const time_point& before) {
    static_assert(util::is_ratio<Ratio>::value, "Ratio is not specialization of std::ratio");
    return duration<Ratio>(before, now());
  }
};

}  // namespace util

