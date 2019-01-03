#pragma once

#include <type_traits>

#include "typelist.hpp"

namespace cxl {
namespace parse {

enum class status
{
  success = 1,
  failure = 0
};

template<typename Match, typename Remainder, status Status, typename ParseTree = typelist<>>
struct parsed
{
  constexpr auto match() const { return Match{}; }
  constexpr auto remainder() const { return Remainder{}; }
  constexpr bool status() const { return (bool)Status; }
  constexpr auto tree() const { return ParseTree{}; }
  template<typename String>
  constexpr auto set_match(const String) const
  {
    return parsed<String, Remainder, Status, ParseTree>{};
  }
  template<typename String>
  constexpr auto set_remainder(const String) const
  {
    return parsed<Match, String, Status, ParseTree>{};
  }
  template<typename Generator>
  constexpr auto generate(const Generator) const
  {
    return parsed<Match, Remainder, Status, decltype(ParseTree{}.append(typelist<Generator>{}))>{};
  }
  constexpr auto fail() const { return parsed<Match, Remainder, status::failure, ParseTree>{}; }
  constexpr auto succeed() const { return parsed<Match, Remainder, status::success, ParseTree>{}; }
};

template<typename M1, typename R1, auto P1, typename T1, typename M2, typename R2, auto P2, typename T2>
constexpr auto
operator+(const parsed<M1, R1, P1, T1>, const parsed<M2, R2, P2, T2>)
{
  return parsed<decltype(M1{} + M2{}), R2, P2, decltype(T1{}.append(T2{}))>{};
}
}
}