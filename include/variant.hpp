#ifndef VARIANT_HPP_GAURD
#define VARIANT_HPP_GAURD

#include "typelist.hpp"
#include "utility.hpp"
#include <exception>
#include <type_traits>

namespace utility {

struct bad_variant_access : std::runtime_error
{
  bad_variant_access(const char* what)
    : std::runtime_error(what)
  {}
};

template<typename T>
void
delete_type(void* HeldPtr)
{
  if constexpr (std::is_lvalue_reference<T>::value)
    delete static_cast<std::remove_reference_t<T>*>(HeldPtr);
  else
    delete static_cast<T*>(HeldPtr);
}

template<typename T, typename... Ts>
struct variant_deleter
{
  static constexpr auto size = sizeof...(Ts) + 1;
  static constexpr void (*deleters[size])(void*) = { &delete_type<T>, &delete_type<Ts>... };

  static inline void release(size_t HeldIndex, void* HeldPtr) { deleters[HeldIndex](HeldPtr); }
};

template<typename T, typename... Ts>
struct variant
{
  typedef typelist<T, Ts...> types;
  typedef variant_deleter<T, Ts...> deleter_type;
  static constexpr size_t invalid_index = sizeof...(Ts) + 1; // one past the last type

  variant()
    : union_(nullptr)
    , held_(invalid_index)
  {}
  template<typename I>
  variant(I&& Init)
  {
    if constexpr (tl_has_type<I, types>::value) {
      if constexpr (std::is_lvalue_reference<I>::value) {
        union_ = new std::remove_reference_t<I>*(&Init);
        held_ = tl_index_of<I, types>::index;
      } else {
        union_ = new I(std::move(Init));
        held_ = tl_index_of<I, types>::index;
      }
    } else if constexpr (tl_has_conversion<I, types>::value) {
      union_ = new typename tl_find_conversion<I, types>::type(std::move(Init));
      held_ = tl_index_of<typename tl_find_conversion<I, types>::type, types>::index;
    } else {
      static_assert(tl_has_type<I, types>::value || tl_has_conversion<I, types>::value,
                    "could not find suitable conversion in variant assignment");
    }
  }
  ~variant()
  {
    if (!empty())
      deleter_type::release(held_, union_);
  }

  bool empty() const { return held_ == invalid_index; }

  void clear()
  {
    if (!empty()) {
      deleter_type::release(held_, union_);
      held_ = invalid_index;
    }
  }

  template<typename C>
  bool is_type() const
  {
    return tl_index_of<C, types>::index == held_;
  }

  template<typename P>
  variant<T, Ts...>& operator=(P&& rhs)
  {
    if constexpr (tl_has_type<P, types>::value) {
      if (tl_index_of<P, types>::index == held_) // dont destroy union_ if unneccesary
      {
        if constexpr (std::is_lvalue_reference<P>::value) {
          *static_cast<std::remove_reference_t<P>**>(union_) = &rhs;
        } else {
          *static_cast<P*>(union_) = std::move(rhs);
          return *this;
        }
      }
      clear();
      if constexpr (std::is_lvalue_reference<P>::value) {
        union_ = new std::remove_reference_t<P>*(&rhs);
        held_ = tl_index_of<P, types>::index;
      } else {
        union_ = new P(std::move(rhs));
        held_ = tl_index_of<P, types>::index;
      }
    } else if constexpr (tl_has_conversion<P, types>::value) {
      clear();
      union_ = new typename tl_find_conversion<P, types>::type(std::move(rhs));
      held_ = tl_index_of<typename tl_find_conversion<P, types>::type, types>::index;
    } else {
      static_assert(tl_has_type<P, types>::value || tl_has_conversion<P, types>::value,
                    "could not find suitable conversion in variant assignment");
    }
    return *this;
  }

  template<typename G>
  G& get()
  {
    if (!empty()) {
      if (held_ == tl_index_of<G, types>::index) {
        if constexpr (std::is_lvalue_reference<G>::value)
          return **static_cast<std::remove_reference_t<G>**>(union_);
        else
          return *static_cast<G*>(union_);
      } else
        throw bad_variant_access("variant does not hold the requested type");

    } else
      throw bad_variant_access("variant is empty");
  }

  template<typename G>
  const G& get() const
  {
    if (!empty()) {
      if (held_ == tl_index_of<G, types>::index) {
        if constexpr (std::is_lvalue_reference<G>::value)
          return **static_cast<std::remove_reference_t<G>**>(union_);
        else
          return *static_cast<G*>(union_);
      } else
        throw bad_variant_access("variant does not hold the requested type");

    } else
      throw bad_variant_access("variant is empty");
  }

private:
  void* union_;
  size_t held_;
};
}

#endif