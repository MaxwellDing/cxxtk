#ifndef OBJECT_H_
#define OBJECT_H_

#include <atomic>
#include <iostream>
#include <typeinfo>

template <class T> class Printer;
template <class T>
class Object {
 public:
  Object() { ++construct_; };
  Object(const Object&) { ++copy_construct_; };
  Object(Object&&) { ++move_construct_; };
  Object& operator=(const Object&) { ++copy_assign_; return *this; };
  Object& operator=(Object&&) { ++move_assign_; return *this; };
  static void Clear() {
    construct_.store(0);
    copy_construct_.store(0);
    move_construct_.store(0);
    copy_assign_.store(0);
    move_assign_.store(0);
  }
  static void Print() {
    std::cout << "--- Summary of [" << typeid(T).name() << "], size = " << sizeof(T) << '\n';
    std::cout << "  Contruct time: "      << construct_ << "\n";
    std::cout << "  Copy contruct time: " << copy_construct_ << "\n";
    std::cout << "  Move contruct time: " << move_construct_ << "\n";
    std::cout << "  Copy assign time: "   << copy_assign_ << "\n";
    std::cout << "  Move assign time: "   << move_assign_ << "\n";
    std::cout << "--- Summary End ---"    << std::endl;
  }

 private:
  static std::atomic<uint64_t> construct_;
  static std::atomic<uint64_t> copy_construct_;
  static std::atomic<uint64_t> move_construct_;
  static std::atomic<uint64_t> copy_assign_;
  static std::atomic<uint64_t> move_assign_;
};

template <class T> std::atomic<uint64_t> Object<T>::construct_(0);
template <class T> std::atomic<uint64_t> Object<T>::copy_construct_(0);
template <class T> std::atomic<uint64_t> Object<T>::move_construct_(0);
template <class T> std::atomic<uint64_t> Object<T>::copy_assign_(0);
template <class T> std::atomic<uint64_t> Object<T>::move_assign_(0);

#endif  // OBJECT_H_
