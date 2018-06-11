#pragma once

#include "typelist.hpp"
#include "utility.hpp"
#include <cassert>
#include <cstdint>
#include <type_traits>

namespace utility {

struct bad_variant_access : std::runtime_error
{
  bad_variant_access(const char* what)
    : std::runtime_error(what)
  {}
};

template<typename... Types>
struct alignas(typelist<Types...>::alignof_max) variant_storage
{
  constexpr variant_storage() {}
  template<typename T>
  constexpr variant_storage(T&& value)
  {
    reinterpret_cast<T&>(raw) = std::move(value);
  }
  template<typename T>
  constexpr variant_storage<Ts...>& operator=(T&& value)
  {
    reinterpret_cast<T&>(raw) = std::move(value);
    return *this;
  }
  template<typename T>
  constexpr operator T&()
  {
    return reinterpret_cast<T&>(raw);
  }
  template<typename T>
  constexpr operator const T&() const
  {
    return reinterpret_cast<const T&>(raw);
  }

  template<typename T>
  constexpr void destroy()
  {
    reinterpret_cast<T&>(raw).~T();
  }

private:
  std::uint8_t raw[typelist<Ts...>::sizeof_max];
};

template<typename T, typename... Ts>
struct variant
{
  using storage_type = variant_storage<T, Ts...>;

  using types = typelist<T, Ts...>;

  static constexpr size_t invalid_index = sizeof...(Ts) + 1; // one past the last type

  constexpr variant()
    : held_(invalid_index)
  {}

  template<typename I>
  constexpr variant(I&& Init)
  {
    /* if constexpr (types::template count_type<I>()) {
      if constexpr (std::is_lvalue_reference<I>::value) {
        union_ = static_cast<std::remove_reference_t<I>*>(&Init);
        held_ = types::template index_of<I>();
      } else {
        union_ = std::move(Init);
        held_ = types::template index_of<I>();
      }
    } else if constexpr (types::template count_type<std::remove_reference_t<I>>()) {
      union_ = Init;
      held_ = types::template index_of<std::remove_reference_t<I>>();
    } else if constexpr (types::template count_type<std::decay_t<I>>()) {
      static_cast<std::decay_t<I>&>(union_) = Init;
      held_ = types::template index_of<std::decay_t<I>>();
    } else if constexpr (types::template count_conversions<I>()) {
      using conv_type = decltype(types::template apply_first_conversion(Init));
      static_cast<conv_type&>(union_) = std::move(types::template apply_first_conversion(Init));
      held_ = types::template index_of<conv_type>();
    } else {
      static_assert(types::template count_conversions<I>(), "could not find suitable conversion in variant assignment");
    } */
  }

  constexpr size_t index() const { return held_; }

  constexpr bool empty() const { return held_ == invalid_index; }

  constexpr void clear()
  {
    if (!empty()) {
      union_ = value_type();
      held_ = invalid_index;
    }
  }

  template<typename C>
  constexpr bool is_type() const
  {
    return types::template index_of<C>() == held_;
  }

  template<typename P>
  constexpr variant<T, Ts...>& operator=(P&& rhs)
  {
    if constexpr (types::template count_type<P>()) {
      if (types::template index_of<P>() == held_) // dont destroy union_ if unneccesary
      {
        if constexpr (std::is_lvalue_reference<P>::value) {
          union_ = static_cast<std::remove_reference_t<P>*>(&rhs);
        } else {
          union_ = std::move(rhs);
        }
        return *this;
      }
      clear();
      if constexpr (std::is_lvalue_reference<P>::value) {
        union_ = static_cast<std::remove_reference_t<P>*>(&rhs);
        held_ = types::template index_of<P>();
      } else {
        union_ = std::move(rhs);
        held_ = types::template index_of<P>();
      }
    } else if constexpr (types::template count_type<std::remove_reference_t<P>>()) {
      if (types::template index_of<std::remove_reference_t<P>>() == held_) // dont destroy union_ if unneccesary
      {
        union_ = rhs;
        return *this;
      }
      clear();
      union_ = rhs;
      held_ = types::template index_of<std::remove_reference_t<P>>();
    } else if constexpr (types::template count_conversions<P>()) {
      clear();
      using conv_type = decltype(types::template apply_first_conversion(rhs));
      static_cast<conv_type&>(union_) = std::move(types::template apply_first_conversion(rhs));
      held_ = types::template index_of<conv_type>();
    } else {
      static_assert(types::template count_conversions<P>(), "could not find suitable conversion in variant assignment");
    }
    return *this;
  }

  template<typename G>
  constexpr const G& get() const
  {
    if (!empty()) {
      if (held_ == types::template index_of<G>()) {
        if constexpr (std::is_lvalue_reference<G>::value) {
          return *static_cast<std::remove_reference_t<G>*>(union_);
        } else {
          return union_;
        }
      } else
        throw bad_variant_access("variant does not hold the requested type");

    } else
      throw bad_variant_access("variant is empty");
  }

  template<size_t Index>
  constexpr decltype(auto) get() const
  {
    return get<typename types::template type_at<Index>>();
  }

private:
  value_type union_;
  size_t held_;
};
}