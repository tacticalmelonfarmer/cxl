#pragma once
#include "utility.hpp"

namespace utility::cexpr {

template<size_t Index>
struct cindex
{
  const size_t index = Index;

  constexpr operator size_t() const { return index; }
};

template<auto Value, auto Power>
struct cpow
{
  template<size_t Dummy, auto PassValue>
  struct identity
  {
    static constexpr auto value = PassValue;
  };
  template<size_t... Indices>
  static constexpr auto produce(const ct::index_range<Indices...>&)
  {
    return (static_cast<decltype(Value)>(1) * ... * identity<Indices, Value>::value);
  };
  static constexpr auto value = produce(ct::make_index_range_t<0, Power - 1>());
};

template<auto Value>
struct cpow<Value, 0>
{
  static constexpr auto value = (Value == 0) ? 0 : 1;
};

template<size_t Place, class... Ints>
constexpr int
combine_digits_base10(int int0, Ints... ints)
{
  if constexpr (Place == 0)
    return int0;
  else
    return combine_digits_base10<Place - 1>(cpow<10, Place>::value + int0, ints...);
}

constexpr int
parse_digit(char C)
{
  return (C >= '0' && C <= '9') ? C - '0' : throw std::out_of_range("only decimal digits are allowed");
}

namespace literals {
template<char... Digits>
constexpr auto operator"" _ci() -> cindex<combine_digits_base10<sizeof...(Digits) - 1>(parse_digit(Digits)...)>
{
  return {};
}
}
}