#pragma once
#include <cstdlib>
#include <vector>

namespace tk {
namespace util {

class ObjPtr
{
public:
    typedef void (*del_t)(void*);
    ObjPtr(void* p, del_t d) : ptr_(p), del_(d) {}
    ~ObjPtr() { if (del_ && ptr_) del_(ptr_); }
    ObjPtr(const ObjPtr&) = delete;
    ObjPtr& operator=(const ObjPtr&) = delete;
    ObjPtr(ObjPtr&& rv) { swap(rv); }
    ObjPtr& operator=(ObjPtr&& rhs) { swap(rhs); return *this; }
    void swap(ObjPtr& o) { std::swap(ptr_, o.ptr_); std::swap(del_, o.del_); }

private:
    void* ptr_{nullptr};
    del_t del_{nullptr};
};

template<typename T>
void destructDel(void* o) { reinterpret_cast<T*>(o)->~T(); }
template<typename T>
void deleteDel(void* o) { delete reinterpret_cast<T*>(o); }

class ObjectPool
{
public:
    // default size: 4K
    explicit ObjectPool(size_t bytes = 4096) : capacity_(bytes)
    {
        buf_ = reinterpret_cast<char*>(malloc(bytes));
    }
    template <typename T>
    T* requireObj()
    {
        static_assert(std::is_default_constructible<T>::value, "Object is not default constructable");
        T* ptr;
        if (ofs_ + sizeof(T) <= capacity_)
        {
            // construct without malloc
            ptr = new(buf_ + ofs_) T;
            ofs_ += sizeof(T);
            obj_q_.emplace_back(ptr, &destructDel<T>);
        }
        else
        {
            ptr = new T;
            obj_q_.emplace_back(ptr, &deleteDel<T>);
        }
        return ptr;
    }
    void releaseAll()
    {
        obj_q_.clear();
        ofs_ = 0;
    }
    ~ObjectPool()
    {
        releaseAll();
        free(buf_);
        buf_ = nullptr;
        capacity_ = 0;
    }

private:
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;

    char* buf_{nullptr};
    size_t ofs_{0};
    size_t capacity_{0};
    std::vector<ObjPtr> obj_q_;
};

}}  // tk::util::

