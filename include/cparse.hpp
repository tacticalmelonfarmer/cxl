#pragma once
#include "cstring.hpp"
#include "ctuple.hpp"
#include "utility.hpp"
#include <type_traits>

namespace utility::cexpr {
using namespace utility::cexpr::literals;

template<typename T, typename... Gen>
struct parse_success : public T
{
  using generated_types = typelist<Gen...>;
  template<typename U>
  constexpr auto generate() const
  {
    return parse_success<T, Gen..., U>{};
  }
  constexpr T string() const { return T{}; }
  constexpr bool success() const { return true; }
};
template<typename T>
struct parse_failure : public T
{
  constexpr T string() const { return T{}; }
  constexpr bool success() const { return false; }
};

/////////////////////////// forward parser decls.

template<typename Target>
struct not_parser;

template<typename Target>
struct one_parser;

template<typename Target>
struct opt_parser;

template<typename Target>
struct plus_parser;

template<typename Target>
struct star_parser;

template<typename Target0, typename... Targets>
struct seq_parser;

template<typename Target0, typename... Targets>
struct or_parser;

/////////////////////////// parsers

template<typename Target>
struct not_parser
{
  constexpr not_parser() {}
  constexpr not_parser(const Target&) {}
  template<typename String>
  constexpr auto parse(const String&)
  {
    constexpr auto string = String{};
    constexpr auto result = Target{}.parse(string);
    if constexpr (result.success())
      return parse_failure<String>{};
    else
      return parse_success<decltype(substr(++string.begin(), string.end()))>{};
  }
  constexpr auto operator~() const { return opt_parser<not_parser<Target>>{}; }
  constexpr auto operator+() const { return plus_parser<not_parser<Target>>{}; }
  constexpr auto operator*() const { return star_parser<not_parser<Target>>{}; }
};

template<typename Target>
struct one_parser
{
  constexpr one_parser() {}
  constexpr one_parser(const Target&) {}
  template<typename String>
  constexpr auto parse(const String&) const
  {
    constexpr auto target = Target{};
    constexpr auto string = String{};
    static_assert(target.size <= string.size, "Parser input too small");
    constexpr size_t size_of_match = match_size<Target, String>{}.match(ct::make_index_range_t<0, target.size - 1>{});
    if constexpr (size_of_match == target.size) {
      if constexpr (string.begin() + target.size == string.end())
        return parse_success<cstring<>>{};
      else
        return parse_success<decltype(substr(string.begin() + target.size, string.end()))>{};
    } else
      return parse_failure<String>{};
  }
  constexpr auto operator!() const { return not_parser<one_parser<Target>>{}; }
  constexpr auto operator~() const { return opt_parser<one_parser<Target>>{}; }
  constexpr auto operator+() const { return plus_parser<one_parser<Target>>{}; }
  constexpr auto operator*() const { return star_parser<one_parser<Target>>{}; }
};

template<typename Target>
struct opt_parser
{
  constexpr opt_parser() {}
  constexpr opt_parser(const Target&) {}
  template<typename String>
  constexpr auto parse(const String&)
  {
    constexpr auto result = Target{}.parse(String{});
    if constexpr (result.success())
      return result;
    else
      return parse_success<String>{};
  }
  constexpr auto operator!() const { return not_parser<opt_parser<Target>>{}; }
  constexpr auto operator+() const { return plus_parser<opt_parser<Target>>{}; }
  constexpr auto operator*() const { return star_parser<opt_parser<Target>>{}; }
};

template<typename Target>
struct plus_parser
{
  constexpr plus_parser() {}
  constexpr plus_parser(const Target&) {}
  template<typename String, size_t Count = 0>
  constexpr auto parse(const String&) const
  {
    constexpr auto target = Target{};
    constexpr auto string = String{};
    constexpr auto next = target.parse(string);
    if constexpr (Count == 0) {
      if constexpr (next.success()) {
        if constexpr (next.string().size == 0)
          return next;
        else
          return parse(next.string());
      } else
        return parse_failure<String>{};
    } else {
      if constexpr (next.success()) {
        if constexpr (next.string().size == 0)
          return next;
        else
          return parse(next.string());
      } else
        return parse_success<String>{};
    }
  }
  constexpr auto operator!() const { return not_parser<plus_parser<Target>>{}; }
  constexpr auto operator~() const { return opt_parser<plus_parser<Target>>{}; }
};

template<template<char...> typename String, char... Chars>
plus_parser(String<Chars...>)->plus_parser<one_parser<String<Chars...>>>;

template<typename Target>
struct star_parser
{
  constexpr star_parser() {}
  constexpr star_parser(const Target&) {}
  template<typename String>
  constexpr auto parse(const String&) const
  {
    constexpr auto target = Target{};
    constexpr auto string = String{};
    constexpr auto next = target.parse(string);
    if constexpr (next.success()) {
      if constexpr (next.string().size == 0)
        return next;
      else
        return parse(next.string());
    } else
      return parse_success<String>{};
  }
  constexpr auto operator!() const { return not_parser<star_parser<Target>>{}; }
  constexpr auto operator~() const { return opt_parser<star_parser<Target>>{}; }
};

template<template<char...> typename String, char... Chars>
star_parser(String<Chars...>)->star_parser<one_parser<String<Chars...>>>;

template<typename Target0, typename... Targets>
struct seq_parser
{
  constexpr seq_parser() {}
  constexpr seq_parser(const Target0&, const Targets&...) {}
  template<typename FallbackString, typename String, size_t Index, typename UTarget0, typename... UTargets>
  constexpr auto parse_imp() const
  {
    constexpr auto target = UTarget0{};
    constexpr auto string = String{};
    if constexpr (sizeof...(UTargets) == 0) // last target
    {
      constexpr auto next = target.parse(string);
      if constexpr (next.success()) {
        return parse_success<cstring<>>{};
      } else {
        return parse_failure<FallbackString>{};
      }
    } else { // not last target
      constexpr auto next = target.parse(string);
      if constexpr (next.success()) {
        return parse_imp<FallbackString, decltype(next.string()), Index + 1, UTargets...>();
      } else {
        return parse_failure<FallbackString>{};
      }
    }
  }
  template<typename String>
  constexpr auto parse(const String&) const
  {
    return parse_imp<String, String, 0, Target0, Targets...>();
  }
  constexpr auto operator!() const { return not_parser<seq_parser<Target0, Targets...>>{}; }
  constexpr auto operator~() const { return opt_parser<seq_parser<Target0, Targets...>>{}; }
  constexpr auto operator+() const { return plus_parser<seq_parser<Target0, Targets...>>{}; }
  constexpr auto operator*() const { return star_parser<seq_parser<Target0, Targets...>>{}; }
};

template<typename Target0, typename... Targets>
struct or_parser
{
  constexpr or_parser() {}
  constexpr or_parser(const Target0&, const Targets&...) {}
  template<typename String, typename UTarget0, typename... UTargets>
  constexpr auto parse_imp() const
  {
    constexpr auto result = UTarget0{}.parse(String{});
    if constexpr (result.success())
      return result;
    else
      return parse_imp<String, UTargets...>();
  }
  template<typename String>
  constexpr auto parse(const String&) const
  {
    return parse_imp<String, Target0, Targets...>();
  }
  constexpr auto operator!() const { return not_parser<or_parser<Target0, Targets...>>{}; }
  constexpr auto operator~() const { return opt_parser<or_parser<Target0, Targets...>>{}; }
  constexpr auto operator+() const { return plus_parser<or_parser<Target0, Targets...>>{}; }
  constexpr auto operator*() const { return star_parser<or_parser<Target0, Targets...>>{}; }
};

namespace literals {
template<typename CharT, CharT... Chars>
constexpr auto operator"" _one() -> one_parser<cstring<Chars...>>
{
  return {};
}
template<typename CharT, CharT... Chars>
constexpr auto operator"" _star() -> star_parser<one_parser<cstring<Chars...>>>
{
  return {};
}
template<typename CharT, CharT... Chars>
constexpr auto operator"" _plus() -> plus_parser<one_parser<cstring<Chars...>>>
{
  return {};
}
template<typename CharT, CharT... Chars>
constexpr auto operator"" _opt() -> opt_parser<one_parser<cstring<Chars...>>>
{
  return {};
}
}

template<typename L, typename R>
constexpr auto
operator&&(const L&, const R&)
{
  return seq_parser<L, R>{};
}
template<typename... L, typename... R>
constexpr auto
operator&&(const seq_parser<L...>&, const seq_parser<R...>&)
{
  return seq_parser<L..., R...>{};
}
template<typename... L, typename R>
constexpr auto
operator&&(const seq_parser<L...>&, const R&)
{
  return seq_parser<L..., R>{};
}
template<typename L, typename... R>
constexpr auto
operator&&(const L&, const seq_parser<R...>&)
{
  return seq_parser<L, R...>{};
}

template<typename L, typename R>
constexpr auto
operator||(const L&, const R&)
{
  return or_parser<L, R>{};
}
template<typename... L, typename... R>
constexpr auto
operator||(const or_parser<L...>&, const or_parser<R...>&)
{
  return or_parser<L..., R...>{};
}
template<typename... L, typename R>
constexpr auto
operator||(const or_parser<L...>&, const R&)
{
  return or_parser<L..., R>{};
}
template<typename L, typename... R>
constexpr auto
operator||(const L&, const or_parser<R...>&)
{
  return or_parser<L, R...>{};
}
}