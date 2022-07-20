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

#ifndef TK_UTIL_COW_PTR_H_
#define TK_UTIL_COW_PTR_H_

#include <memory>
#include <type_traits>

namespace tk {
namespace util {

/**
 * @brief Copy on write pointer
 *
 * @tparam T Type of stored element
 */
template <typename T>
class cow_ptr final {
 public:
  static constexpr bool is_nothrow_detach = std::is_nothrow_copy_constructible_v<T>;
  
  constexpr cow_ptr() noexcept : base_{nullptr} {}
  constexpr cow_ptr(std::nullptr_t) noexcept : cow_ptr() {}
  /**
   * @brief Construct a new cow_ptr object from raw pointer
   *
   * @param other Raw pointer
   */
  explicit cow_ptr(T* other) noexcept : base_(other) {}
  ~cow_ptr() = default;
  constexpr cow_ptr(const cow_ptr&) = default;
  constexpr cow_ptr(cow_ptr&&) = default;
  constexpr cow_ptr& operator=(cow_ptr&&) = default;

  /**
   * @brief Copy assign value
   *
   * @param other Other cow_ptr
   * @return cow_ptr& this cow_ptr
   */
  cow_ptr& operator=(const cow_ptr& other) noexcept {
    if (other.base_ != base_) {
      base_ = other.base_;
    }
    return *this;
  }

  /**
   * @brief Assign raw pointer
   *
   * @param other Raw pointer
   * @return cow_ptr& this cow_ptr
   */
  cow_ptr& operator=(T* other) noexcept {
    base_.reset(other);
    return *this;
  }

  /*-------------dereferencer -----------------*/
  /**
   * @brief Dereferences the stored pointer
   *
   * @return T& The result of dereferencing the stored pointer
   */
  T& operator*() noexcept(is_nothrow_detach) {
    detach();
    return *base_;
  }

  /**
   * @brief Dereferences the stored pointer
   *
   * @return const T& The result of dereferencing the stored pointer
   */
  const T& operator*() const noexcept {
    return *base_;
  }

  /**
   * @brief Dereferences the stored pointer
   *
   * @return T* The stored pointer
   */
  T* operator->() noexcept(is_nothrow_detach) {
    detach();
    return base_.get();
  }

  /**
   * @brief Dereferences the stored pointer
   *
   * @return const T* The stored pointer
   */
  const T* operator->() const noexcept {
    return base_.get();
  }

  /*--------------- getter ------------------*/
  /**
   * @brief Returns the stored pointer
   *
   * @return T* The stored pointer
   */
  T* get() noexcept(is_nothrow_detach) {
    detach();
    return base_.get();
  }

  /**
   * @brief Returns the stored pointer
   *
   * @return const T* The stored pointer
   */
  const T* get() const noexcept {
    return base_.get();
  }

  /**
   * @brief Returns the stored pointer
   *
   * @return const T* The stored pointer
   */
  const T* get_const() const noexcept {
    return base_.get();
  }

  /*--------------- comparator ----------------*/
  /**
   * @brief Compare two cow_ptr objects
   *
   * @param other Other cow_ptr
   * @return *this == other
   */
  bool operator==(const cow_ptr<T> other) const noexcept {
    return base_ == other.base_;
  }

  /**
   * @brief Compare two cow_ptr objects
   *
   * @param other Other cow_ptr
   * @return *this != other
   */
  bool operator!=(const cow_ptr<T> other) const noexcept {
    return base_ != other.base_;
  }

  /**
   * @brief Compare two cow_ptr objects
   *
   * @param other Other cow_ptr
   * @return *this < other
   */
  bool operator<(const cow_ptr<T> other) const noexcept {
    return base_ < other.base_;
  }

  /**
   * @brief Compare two cow_ptr objects
   *
   * @param other Other cow_ptr
   * @return *this > other
   */
  bool operator>(const cow_ptr<T> other) const noexcept {
    return base_ > other.base_;
  }


  /**
   * @brief Compare two cow_ptr objects
   *
   * @param other Other cow_ptr
   * @return *this <= other
   */
  bool operator<=(const cow_ptr<T> other) const noexcept {
    return !(base_ > other.base_);
  }

  /**
   * @brief Compare two cow_ptr objects
   *
   * @param other Other cow_ptr
   * @return *this >= other
   */
  bool operator>=(const cow_ptr<T> other) const noexcept {
    return !(base_ < other.base_);
  }

  /**
   * @brief Convert cow_ptr to bool
   *
   * @retval true If owns a ptr
   * @retval false Otherwise
   */
  operator bool() const noexcept {
    return bool(base_);
  }

  void swap(cow_ptr& r) noexcept {
    base_.swap(r.base_);
  }

  std::size_t hash() const noexcept {
    return std::hash<std::shared_ptr<T>>(base_);
  }

 private:
  void detach() noexcept(is_nothrow_detach) {
    if (base_ && !base_.unique()) {
      T* tmp = base_.get();
      base_.reset(new T(*tmp));
    }
  }

  std::shared_ptr<T> base_{nullptr};
};  // class cow_ptr

/**
 * @brief Create a cow_ptr in-place
 *
 * @tparam T Type of stored element in cow_ptr
 * @tparam Args Type of arguments to construct T
 * @param args Arguments to construct T
 * @return cow_ptr<T> A new cow_ptr
 */
template <typename T, typename... Args,
          typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
cow_ptr<T> make_cow(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
  return cow_ptr<T>(new T(std::forward<Args>(args)...));
}

}  // namespace util
}  // namespace tk

namespace std {
template <typename T>
void swap(tk::util::cow_ptr<T>& lhs, tk::util::cow_ptr<T>& rhs) noexcept {
  lhs.swap(rhs);
}

template <typename T>
struct hash<tk::util::cow_ptr<T>> {
  size_t operator()(const tk::util::cow_ptr<T>& cp) const noexcept {
    return cp.hash();
  }
};
}  // namespace std


#endif  // TK_UTIL_COW_PTR_H_