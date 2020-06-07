[[maybe_unused]] char runtime_text[] = R"****(
/**
  * @file   Runtime.h 
  * @author sora
  * @date   2020/6/6
  */

#ifndef GEN_INTERFACE_V2_RUNTIME_RUNTIME_H_
#define GEN_INTERFACE_V2_RUNTIME_RUNTIME_H_
#include <variant>
#include <memory>

template<typename U, template<typename...> typename T>
struct is_template_of : std::false_type {};

template<typename U, template<typename...> typename T>
struct is_template_of<T<U>, T> : std::true_type {};

template<typename T>
constexpr bool is_shared_ptr_v = is_template_of<std::decay_t<T>, std::shared_ptr>::value;

template<typename T>
constexpr bool is_unique_ptr_v = is_template_of<std::decay_t<T>, std::unique_ptr>::value;

template<typename T>
class Proxy {
  template<typename U>
  friend
  class Proxy;
 private:
  using variant_type = std::variant<std::shared_ptr<T>, std::unique_ptr<T>, T *, std::pair<void *, void (*)(void *)>>;
  variant_type objPtr;
 public:
  ~Proxy() { if (objPtr.index() == 3) { std::get<3>(objPtr).second(std::get<3>(objPtr).first); }}
  T *get() {
    switch (objPtr.index()) {
      case 0: { return std::get<0>(objPtr).get(); }
      case 1: { return std::get<1>(objPtr).get(); }
      case 2: { return std::get<2>(objPtr); }
      case 3: { return reinterpret_cast<T *>(std::get<3>(objPtr).first); }
      default: throw std::runtime_error("invalid index");
    }
  }
  T *const operator->() const {
    return this->get();
  }

  T *const get() const {
    switch (objPtr.index()) {
      case 0: { return std::get<0>(objPtr).get(); }
      case 1: { return std::get<1>(objPtr).get(); }
      case 2: { return std::get<2>(objPtr); }
      case 3: { return reinterpret_cast<T *>(std::get<3>(objPtr).first); }
      default: throw std::runtime_error("invalid index");
    }
  }
  T *operator->() {
    return this->get();
  }
  Proxy() : objPtr(nullptr) {}
  Proxy(nullptr_t) : objPtr(nullptr) {}
  Proxy(T *t) : objPtr(t) {}
  Proxy(T &t) : objPtr(&t) {}
  Proxy(T &&t) : objPtr(std::make_unique<T>(std::move(t))) {}
  Proxy(const std::shared_ptr<T> &t) : objPtr(t) {}
  Proxy(std::shared_ptr<T> &&t) : objPtr(std::move(t)) {}
  Proxy(const std::unique_ptr<T> &t) : objPtr(t.get()) {}
  Proxy(std::unique_ptr<T> &&t) : objPtr(std::move(t)) {}

  template<typename U,
           typename E = std::enable_if_t<
               !std::is_same_v<std::decay_t<T>, std::decay_t<U>> && !is_shared_ptr_v<U> && !is_unique_ptr_v<U>>>
  Proxy(U *u) : objPtr(reinterpret_cast<T *>(u)) {}

  template<typename U,
           typename E = std::enable_if_t<
               !std::is_same_v<std::decay_t<T>, std::decay_t<U>> && !is_shared_ptr_v<U> && !is_unique_ptr_v<U>>>
  Proxy(U &u) : objPtr(reinterpret_cast<U *>(&u)) {}
  template<typename U,
           typename E = std::enable_if_t<
               !std::is_same_v<std::decay_t<T>, std::decay_t<U>> && !std::is_lvalue_reference_v<U> && !is_shared_ptr_v<U>
                   && !is_unique_ptr_v<U>>>
  Proxy(U &&u) : objPtr(std::pair{new U(std::move(u)), [](void *ptr) { delete ((U *) ptr); }}) {}
  template<typename U, typename E = std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::decay_t<U>>>>
  Proxy(const std::shared_ptr<U> &u) : objPtr(std::reinterpret_pointer_cast<T>(u)) {}
  template<typename U, typename E = std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::decay_t<U>>>>
  Proxy(std::shared_ptr<U> &&u) : objPtr(std::reinterpret_pointer_cast<T>(std::move(u))) {}
  template<typename U, typename E = std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::decay_t<U>>>>
  Proxy(const std::unique_ptr<U> &u): objPtr(reinterpret_cast<T*>(u.get())) {}
  template<typename U, typename E = std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::decay_t<U>>>>
  Proxy(std::unique_ptr<U> &&u) : objPtr(std::pair{u.release(), [](void *ptr) { delete ((U *) ptr); }}) {}

  Proxy(const Proxy &rhs) : objPtr(
      rhs.objPtr.index() == 0 ? variant_type(std::get<0>(rhs.objPtr)) : variant_type(rhs.get())) {}
  Proxy(Proxy &&rhs) : objPtr(std::move(rhs.objPtr)) {}
  Proxy &operator=(const Proxy &rhs) {
    if (&rhs == this) return *this;
    objPtr = rhs.objPtr.index() == 0 ? std::get<0>(rhs.objPtr) : rhs.get();
    return *this;
  }
  Proxy &operator=(Proxy &&rhs) {
    if (&rhs == this) return *this;
    objPtr = std::move(rhs.objPtr);
    rhs.objPtr = nullptr;
    return *this;
  }
  template<typename U, typename E = std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::decay_t<U>>>>
  Proxy(const Proxy<U> &u) {
    switch (u.objPtr.index()) {
      case 0: {
        objPtr = std::reinterpret_pointer_cast<T>(std::get<0>(u.objPtr));
        break;
      }
      case 1: {
        objPtr = reinterpret_cast<T *>(std::get<1>(u.objPtr).get());
        break;
      }
      case 2: {
        objPtr = reinterpret_cast<T *>(std::get<2>(u.objPtr));
        break;
      }
      case 3: {
        objPtr = reinterpret_cast<T *>(std::get<3>(u.objPtr).first);
        break;
      }
      default: throw std::runtime_error("invalid index");
    }
  }
  template<typename U, typename E = std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::decay_t<U>>>>
  Proxy(Proxy<U> &&u) {
    switch (u.objPtr.index()) {
      case 0: {
        objPtr = std::reinterpret_pointer_cast<T>(std::move(std::get<0>(u.objPtr)));
        break;
      }
      case 1: {
        objPtr = std::pair{std::get<1>(u.objPtr).release(), [](void *ptr) { delete ((U *) ptr); }};
        break;
      }
      case 2: {
        objPtr = reinterpret_cast<T *>(std::get<2>(u.objPtr));
        break;
      }
      case 3: {
        objPtr = std::move(std::get<3>(u.objPtr));
        u.objPtr = nullptr;
        break;
      }
      default: throw std::runtime_error("invalid index");
    }
  }
};

#endif //GEN_INTERFACE_V2_RUNTIME_RUNTIME_H_
)****";