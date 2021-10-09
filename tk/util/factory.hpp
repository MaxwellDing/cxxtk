#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace util {

template <typename...>
using void_t = void;

template <typename T, typename = void>
struct is_std_hashable : std::false_type {};

template <typename T>
struct is_std_hashable<T, void_t<decltype(std::declval<std::hash<T>>()(std::declval<T>()))>> : std::true_type {};

template <typename T>
static constexpr bool is_std_hashable_v = is_std_hashable<T>::value;

/*
 * Factory for any Type creator.
 *
 * Usage:
 *
 * struct Foo;
 * // Register a creator.
 * Factory<Foo>::Global().Register("bar", [] -> std::unique_ptr<Foo> { ... });
 * // Retrive a creator.
 * auto foo_instance = Factory<Foo>::Global().Create("bar");
 */
template <typename KeyType, typename ObjectType, typename ObjectTypePtr = std::unique_ptr<ObjectType>>
class FactoryBase {
 public:
  static_assert(is_std_hashable_v<KeyType>, "KeyType of Factory is not hashable");
  using key_t = KeyType;
  using obj_t = ObjectType;
  using obj_ptr_t = ObjectTypePtr;
  using creator_t = std::function<obj_ptr_t()>;

  static FactoryBase& Global() {
    // FIXME(dmh): local static life cycle
    static FactoryBase x;
    return x;
  }

  void Register(const key_t& key, creator_t&& creator) {
    if (creators_.find(key) != creators_.end()) throw std::invalid_argument("duplicated object type");
    creators_.emplace(key, std::move(creator));
  }

  obj_ptr_t Create(const key_t& key) const {
    auto it = creators_.find(key);
    if (it == creators_.cend()) return nullptr;
    return obj_ptr_t(it->second());
  }

  std::vector<key_t> Keys() const {
    std::vector<key_t> keys;
    keys.reserve(creators_.size());
    for (const auto& it : creators_) {
      keys.emplace_back(it.first);
    }
    return keys;
  }

 protected:
  std::unordered_map<key_t, creator_t> creators_;
};

template <typename ObjectType, typename ObjectTypePtr = std::unique_ptr<ObjectType>>
using Factory = FactoryBase<std::string, ObjectType, ObjectTypePtr>;

/* A helper function to help run a lambda at the start.
 */
template <typename Type>
class Registor {
 public:
  explicit Registor(std::function<void()>&& functor) { functor(); }

  // Touch will do nothing.
  int Touch() { return 0; }
};

}  // namespace util

