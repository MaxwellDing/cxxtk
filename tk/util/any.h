#ifndef CXXTK_ANY_H_
#define CXXTK_ANY_H_

#include <functional>
#include <typeinfo>

#include "log/logger.h"

namespace cxxtk {

class Any {
 public:
  Any() = default;
  explicit Any(const Any& other) {
    type_ = other.type_;
    data_ = other.clone_(other.data_);
    deleter_ = other.deleter_;
    clone_ = other.clone_;
  }

  template <typename T>
  void Set(const T& v) {
    if (type_ != invalid_type_) {
      FAIL_UNLESS(type_ == typeid(T).hash_code());
    } else {
      type_ = typeid(T).hash_code();
      deleter_ = [&](void** data) {
        delete static_cast<T*>(*data);
        *data = nullptr;
      };
      clone_ = [&](void* data) {
        T* res = new T;
        FAIL_UNLESS(data);
        *res = *static_cast<T*>(data);
        return res;
      };
    }
    data_ = new T;
    *GetMutable<T>() = v;
  }

  template <typename T>
  const T& Get() const {
    FAIL_UNLESS(data_);
    FAIL_UNLESS(type_ == typeid(T).hash_code());
    return *static_cast<T*>(data_);
  }

  template <typename T>
  T* GetMutable() {
    FAIL_UNLESS(data_);
    FAIL_UNLESS(type_ == typeid(T).hash_code());
    return static_cast<T*>(data_);
  }

  bool Valid() const { return (data_ != nullptr); }

  ~Any() {
    if (Valid()) {
      deleter_(&data_);
    }
  }

 private:
  static const size_t invalid_type_;
  size_t type_ = invalid_type_;
  void* data_ = nullptr;
  std::function<void(void**)> deleter_;
  std::function<void*(void*)> clone_;
};  // class Any

}  // namespace cxxtk

#endif

