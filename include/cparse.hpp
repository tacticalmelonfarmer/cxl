#pragma once
#include "cstring.hpp"
#include "ctuple.hpp"
#include "typelist.hpp"
#include "utility.hpp"

namespace utility {

namespace parse {

using namespace literals;

enum class parse_status
{
  success = 1,
  failure = 0,
  incomplete = 2
};

template<typename Match, typename Remainder, parse_status Status, typename TreeTL>
struct parse_result
{
  constexpr auto match() const { return Match{}; }
  constexpr auto remainder() const { return Remainder{}; }
  constexpr bool status() const { return (bool)Status; }
  constexpr auto tree() const { return TreeTL{}; }
  template<typename String>
  constexpr auto set_match(const String) const
  {
    return parse_result<String, Remainder, Status, TreeTL>{};
  }
  template<typename String>
  constexpr auto set_remainder(const String) const
  {
    return parse_result<Match, String, Status, TreeTL>{};
  }
  template<typename Generator>
  constexpr auto generate(const Generator) const
  {
    return parse_result<Match, Remainder, Status, typelist<Generator>>{};
  }
  constexpr auto fail() const { return parse_result<Match, Remainder, parse_status::failure, TreeTL>{}; }
  constexpr auto succeed() const { return parse_result<Match, Remainder, parse_status::success, TreeTL>{}; }
};

template<typename M1, typename R1, auto P1, typename T1, typename M2, typename R2, auto P2, typename T2>
constexpr auto
operator+(const parse_result<M1, R1, P1, T1>, const parse_result<M2, R2, P2, T2>)
{
  return parse_result<decltype(M1{} + M2{}), R2, P2, decltype(T1{}.append(T2{}))>{};
}

/////////////////////////// forward parser decls.

template<typename TargetString>
struct one_string;

template<typename TargetString>
struct one_char;

template<typename Target>
struct before;

template<typename Target>
struct filter;

template<typename Target>
struct optional;

template<typename Target>
struct one_or_more;

template<typename Target>
struct zero_or_more;

template<typename Target, index_t I>
struct repeat;

template<typename Target, index_t I>
struct repeat_lower_bound;

template<typename Target, index_t I>
struct repeat_upper_bound;

template<typename Target, index_t Min, index_t Max>
struct repeat_fully_bound;

template<typename Target0, typename... Targets>
struct sequence;

template<typename Target0, typename... Targets>
struct one_of;

template<typename Target, template<typename...> typename Output>
struct generator;

/////////////////////////// parsers

template<typename TargetString>
struct one_string
{
  constexpr one_string() {}
  constexpr one_string(const TargetString) {}

  template<typename String>
  constexpr auto parse(const String) const
  {
    constexpr auto target = TargetString{};
    constexpr auto string = String{};
    if constexpr (target.size > string.size) {
      return parse_result<cstring<>, String, parse_status::failure, typelist<>>{};
    } else {
      constexpr index_t size_of_match = cmatch(target, string);
      if constexpr (size_of_match == 0)
        return parse_result<cstring<>, String, parse_status::failure, typelist<>>{};
      if constexpr (size_of_match == target.size) {
        if constexpr (string.begin() + target.size == string.end())
          return parse_result<TargetString, cstring<>, parse_status::success, typelist<>>{};
        else
          return parse_result<TargetString,
                              decltype(csubstr(string.begin() + target.size, string.end())),
                              parse_status::success,
                              typelist<>>{};
      } else
        return parse_result<cstring<>, String, parse_status::failure, typelist<>>{};
    }
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<one_string<TargetString>>{}; }
  constexpr auto operator~() const { return optional<one_string<TargetString>>{}; }
  constexpr auto operator+() const { return one_or_more<one_string<TargetString>>{}; }
  constexpr auto operator*() const { return zero_or_more<one_string<TargetString>>{}; }

  template<index_t I>
  constexpr auto operator[](const std::integral_constant<index_t, I>) const
  {
    return repeat<one_string<TargetString>, I>{};
  }
};

template<typename TargetString>
struct one_char
{
  constexpr one_char() {}
  constexpr one_char(const TargetString) {}

  template<typename String, index_t... Indices>
  constexpr auto parse_impl(const String, const index_range<Indices...>) const
  {
    constexpr auto target_string = TargetString{};
    constexpr auto string = String{};
    if constexpr ((false || ... || (string[0] == target_string[Indices])))
      return parse_result<cstring<string[0]>,
                          decltype(csubstr(++string.begin(), string.end())),
                          parse_status::success,
                          typelist<>>{};
    else
      return parse_result<cstring<>, String, parse_status::failure, typelist<>>{};
  }

  template<typename String>
  constexpr auto parse(const String) const
  {
    constexpr auto target_string = TargetString{};
    constexpr auto string = String{};
    return parse_impl(string, make_index_range<0, target_string.size() - 1>());
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<one_char<TargetString>>{}; }
  constexpr auto operator~() const { return optional<one_char<TargetString>>{}; }
  constexpr auto operator+() const { return one_or_more<one_char<TargetString>>{}; }
  constexpr auto operator*() const { return zero_or_more<one_char<TargetString>>{}; }

  template<index_t I>
  constexpr auto operator[](const std::integral_constant<index_t, I>) const
  {
    return repeat<one_char<TargetString>, I>{};
  }
};

template<typename Target>
struct before
{
  constexpr before() {}
  constexpr before(const Target) {}

  template<typename String>
  constexpr auto parse(const String) const
  {
    constexpr auto string = String{};
    constexpr auto result = Target{}.parse(string);
    if constexpr (result.status())
      return parse_result<cstring<>, String, parse_status::success, typelist<>>{};
    else
      return parse_result<decltype(result.match()), String, parse_status::failure, typelist<>>{};
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator~() const { return optional<before<Target>>{}; }
  constexpr auto operator+() const { return one_or_more<before<Target>>{}; }
  constexpr auto operator*() const { return zero_or_more<before<Target>>{}; }

  template<index_t I>
  constexpr auto operator[](const std::integral_constant<index_t, I>) const
  {
    return repeat<before<Target>, I>{};
  }
};

template<template<char...> typename String, char... Chars>
before(const String<Chars...>&)->before<one_string<String<Chars...>>>;

template<typename Target>
struct filter
{
  constexpr filter() {}
  constexpr filter(const Target) {}

  template<typename String>
  constexpr auto parse(const String) const
  {
    constexpr auto string = String{};
    constexpr auto result = Target{}.parse(string);
    if constexpr (result.status())
      return parse_result<decltype(result.match()), String, parse_status::failure, typelist<>>{};
    else
      return parse_result<decltype(substr(string.begin(), string.begin())),
                          decltype(substr(++string.begin(), string.end())),
                          parse_status::success,
                          typelist<>>{};
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator~() const { return optional<filter<Target>>{}; }
  constexpr auto operator+() const { return one_or_more<filter<Target>>{}; }
  constexpr auto operator*() const { return zero_or_more<filter<Target>>{}; }

  template<index_t I>
  constexpr auto operator[](const std::integral_constant<index_t, I>) const
  {
    return repeat<filter<Target>, I>{};
  }
};

template<template<char...> typename String, char... Chars>
filter(const String<Chars...>)->filter<one_string<String<Chars...>>>;

template<typename Target>
struct optional
{
  constexpr optional() {}
  constexpr optional(const Target) {}

  template<typename String>
  constexpr auto parse(const String) const
  {
    constexpr auto result = Target{}.parse(String{});
    if constexpr (result.status())
      return result;
    else
      return parse_result<cstring<>, String, parse_status::success, typelist<>>{};
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<optional<Target>>{}; }
  constexpr auto operator+() const { return one_or_more<optional<Target>>{}; }
  constexpr auto operator*() const { return zero_or_more<optional<Target>>{}; }

  template<index_t I>
  constexpr auto operator[](const std::integral_constant<index_t, I>) const
  {
    return repeat<optional<Target>, I>{};
  }
};

template<template<char...> typename String, char... Chars>
optional(const String<Chars...>)->optional<one_string<String<Chars...>>>;

template<typename Target>
struct one_or_more
{
  constexpr one_or_more() {}
  constexpr one_or_more(const Target) {}

  template<index_t Count = 0, typename String>
  constexpr auto parse(const String) const
  {
    constexpr Target target;
    constexpr String string;
    constexpr auto result = target.parse(string);
    if constexpr (Count == 0) {
      if constexpr (result.status())
        return result + parse<Count + 1>(csubstr(string.begin() + result.match().size(), string.end()));
      else
        return result;
    } else {
      if constexpr (result.status())
        return result + parse<Count + 1>(csubstr(string.begin() + result.match().size(), string.end()));
      else
        return result.succeed();
    }
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<one_or_more<Target>>{}; }
  constexpr auto operator~() const { return optional<one_or_more<Target>>{}; }
};

template<template<char...> typename String, char... Chars>
one_or_more(const String<Chars...>)->one_or_more<one_string<String<Chars...>>>;

template<typename Target>
struct zero_or_more
{
  constexpr zero_or_more() {}
  constexpr zero_or_more(const Target) {}

  template<index_t Count = 0, typename String>
  constexpr auto parse(const String) const
  {
    constexpr Target target;
    constexpr String string;
    constexpr auto result = target.parse(string);

    if constexpr (result.status())
      return result + parse<Count + 1>(csubstr(string.begin() + result.match().size(), string.end()));
    else
      return result.succeed();
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<zero_or_more<Target>>{}; }
};

template<template<char...> typename String, char... Chars>
zero_or_more(const String<Chars...>)->zero_or_more<one_string<String<Chars...>>>;

//////////////////////////////////////////////////////////////

template<index_t U, typename T>
struct identity
{
  using type = T;
};

template<typename Target, index_t I>
struct repeat
{
  constexpr repeat() {}
  constexpr repeat(const Target, const std::integral_constant<index_t, I>) {}

  template<index_t... Indices>
  constexpr auto make_sequence(const index_range<Indices...>) const
  {
    return sequence<typename identity<Indices, Target>::type...>();
  }

  template<typename String>
  constexpr auto parse(const String) const
  {
    return make_sequence(make_index_range<0, I - 1>()).parse(String{});
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<repeat<Target, I>>{}; }
  constexpr auto operator~() const { return optional<repeat<Target, I>>{}; }
  constexpr auto operator++(int) const { return repeat_lower_bound<Target, I>{}; }
  constexpr auto operator--(int) const { return repeat_upper_bound<Target, I>{}; }
  template<index_t J>
  constexpr auto operator[](const std::integral_constant<index_t, J>) const
  {
    return repeat_fully_bound<Target, I, J>{};
  }
};

template<template<char...> typename String, char... Chars, index_t I>
repeat(const String<Chars...>, const std::integral_constant<index_t, I>)->repeat<one_string<String<Chars...>>, I>;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

template<typename Target, index_t I>
struct repeat_lower_bound
{
  constexpr repeat_lower_bound() {}
  constexpr repeat_lower_bound(const Target, const std::integral_constant<index_t, I>) {}

  template<index_t... Indices>
  constexpr auto make_sequence(const index_range<Indices...>) const
  {
    constexpr auto new_targets =
      typelist<typename identity<Indices, Target>::type...>{}.append(typelist<decltype(+Target{})>{});
    return new_targets.template apply<sequence>();
  }

  template<index_t Count = 0, typename MatchedSoFar = cstring<>, typename String>
  constexpr auto parse(const String) const
  {
    return make_sequence(make_index_range<0, I - 2>()).parse(String{});
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<repeat_lower_bound<Target, I>>{}; }
  constexpr auto operator~() const { return optional<repeat_lower_bound<Target, I>>{}; }
};

template<template<char...> typename String, char... Chars, index_t I>
repeat_lower_bound(const String<Chars...>, const std::integral_constant<index_t, I>)
  ->repeat_lower_bound<one_string<String<Chars...>>, I>;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

template<typename Target, index_t I>
struct repeat_upper_bound
{
  constexpr repeat_upper_bound() {}
  constexpr repeat_upper_bound(const Target, const std::integral_constant<index_t, I>) {}

  template<typename String>
  constexpr auto parse(const String) const
  {
    constexpr auto string = String{};
    constexpr auto base = Target{}.parse(String{});
    constexpr auto result = zero_or_more<Target>{}.parse(String{});
    if constexpr (base.status()) {
      if constexpr (result.match().size <= (I * base.match().size))
        return result;
      else
        return parse_result<decltype(result.match()), String, parse_status::failure, typelist<>>{};
    } else
      return parse_result<cstring<>, String, parse_status::success, typelist<>>{};
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<repeat_upper_bound<Target, I>>{}; }
  constexpr auto operator~() const { return optional<repeat_upper_bound<Target, I>>{}; }
};

template<template<char...> typename String, char... Chars, index_t I>
repeat_upper_bound(const String<Chars...>, const std::integral_constant<index_t, I>)
  ->repeat_upper_bound<one_string<String<Chars...>>, I>;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

template<typename Target, index_t Min, index_t Max>
struct repeat_fully_bound
{
  constexpr repeat_fully_bound() {}
  constexpr repeat_fully_bound(const Target,
                               const std::integral_constant<index_t, Min>,
                               const std::integral_constant<index_t, Max>)
  {}

  template<typename String>
  constexpr auto parse(const String) const
  {
    constexpr auto string = String{};
    constexpr auto base = Target{}.parse(String{});
    constexpr auto result = zero_or_more<Target>{}.parse(String{});
    if constexpr (base.status()) {
      if constexpr (result.match().size >= (Min * base.match().size) &&
                    result.match().size <= (Max * base.match().size))
        return result;
      else
        return parse_result<decltype(result.match()), String, parse_status::failure, typelist<>>{};
    } else
      return parse_result<cstring<>, String, parse_status::success, typelist<>>{};
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator~() const { return optional<repeat_fully_bound<Target, Min, Max>>{}; }
  constexpr auto operator!() const { return filter<repeat_fully_bound<Target, Min, Max>>{}; }
};

template<template<char...> typename String, char... Chars, index_t Min, index_t Max>
repeat_fully_bound(const String<Chars...>,
                   const std::integral_constant<index_t, Min>,
                   const std::integral_constant<index_t, Max>)
  ->repeat_fully_bound<one_string<String<Chars...>>, Min, Max>;

//////////////////////////////////////////////////////////////

template<typename Target0, typename... Targets>
struct sequence
{
  constexpr sequence() {}
  constexpr sequence(const Target0, const Targets...) {}

  template<typename String,
           index_t End = sizeof...(Targets),
           index_t CurrentIndex = 0,
           typename T0 = Target0,
           typename... Ts>
  constexpr auto parse(const String) const
  {
    if constexpr (CurrentIndex == 0) {
      constexpr String string;
      constexpr Target0 target;
      constexpr auto result = target.parse(string);
      if constexpr (result.status())
        return result + parse<decltype(result.remainder()), End, CurrentIndex + 1, Targets...>(result.remainder());
      else
        return result;
    } else if constexpr (CurrentIndex > 0 && CurrentIndex < End) {
      constexpr String string;
      constexpr T0 target;
      constexpr auto result = target.parse(string);
      if constexpr (result.status())
        return result + parse<decltype(result.remainder()), End, CurrentIndex + 1, Ts...>(result.remainder());
      else
        return result;
    } else {
      constexpr String string;
      constexpr T0 target;
      constexpr auto result = target.parse(string);
      return result;
    }
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<sequence<Target0, Targets...>>{}; }
  constexpr auto operator~() const { return optional<sequence<Target0, Targets...>>{}; }
  constexpr auto operator+() const { return one_or_more<sequence<Target0, Targets...>>{}; }
  constexpr auto operator*() const { return zero_or_more<sequence<Target0, Targets...>>{}; }

  template<index_t I>
  constexpr auto operator[](const std::integral_constant<index_t, I>) const
  {
    return repeat<sequence<Target0, Targets...>, I>{};
  }
};

template<typename Target0, typename... Targets>
struct one_of
{
  constexpr one_of() {}
  constexpr one_of(const Target0, const Targets...) {}

  template<typename String, typename UTarget0, typename... UTargets>
  constexpr auto parse_impl() const
  {
    constexpr auto result = UTarget0{}.parse(String{});
    if constexpr (result.status())
      return result;
    else if constexpr (sizeof...(UTargets) == 0) {
      if constexpr (result.status())
        return result;
      else
        return parse_result<decltype(result.match()), String, parse_status::failure, typelist<>>{};
    } else
      return parse_impl<String, UTargets...>();
  }

  template<typename String>
  constexpr auto parse(const String) const
  {
    return decltype(*this){}.template parse_impl<String, Target0, Targets...>();
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<one_of<Target0, Targets...>>{}; }
  constexpr auto operator~() const { return optional<one_of<Target0, Targets...>>{}; }
  constexpr auto operator+() const { return one_or_more<one_of<Target0, Targets...>>{}; }
  constexpr auto operator*() const { return zero_or_more<one_of<Target0, Targets...>>{}; }

  template<index_t I>
  constexpr auto operator[](const std::integral_constant<index_t, I>) const
  {
    return repeat<one_of<Target0, Targets...>, I>{};
  }
};

//////////////////////////////////////////////////////////////

template<typename Target, template<typename...> typename Output>
struct generator
{
  constexpr generator() {}
  constexpr generator(const Target, const Output<>) {}

  template<typename String>
  constexpr auto parse(const String) const
  {
    constexpr auto target = Target{};
    constexpr auto string = String{};
    constexpr auto result = target.parse(string);
    if constexpr (result.tree().size() == 0)
      return result.generate(Output<decltype(result.match())>{});
    else
      return result.generate(result.tree().template apply<Output>());
  }

  constexpr auto disable() const { return Target{}; }

  constexpr auto operator!() const { return filter<generator<Target, Output>>{}; }
  constexpr auto operator~() const { return optional<generator<Target, Output>>{}; }
  constexpr auto operator+() const { return one_or_more<generator<Target, Output>>{}; }
  constexpr auto operator*() const { return zero_or_more<generator<Target, Output>>{}; }

  template<index_t I>
  constexpr auto operator[](const std::integral_constant<index_t, I>) const
  {
    return repeat<generator<Target, Output>, I>{};
  }
};

//////////////////////////////////////////////////////////////////////////////////

template<typename L, typename R>
constexpr auto operator&(const L, const R)
{
  return sequence<L, R>{};
}
template<typename... L, typename... R>
constexpr auto operator&(const sequence<L...>, const sequence<R...>)
{
  return sequence<L..., R...>{};
}
template<typename... L, typename R>
constexpr auto operator&(const sequence<L...>, const R)
{
  return sequence<L..., R>{};
}
template<typename L, typename... R>
constexpr auto operator&(const L, const sequence<R...>)
{
  return sequence<L, R...>{};
}

template<typename L, typename R>
constexpr auto
operator|(const L, const R)
{
  return one_of<L, R>{};
}
template<typename... L, typename... R>
constexpr auto
operator|(const one_of<L...>, const one_of<R...>)
{
  return one_of<L..., R...>{};
}
template<typename... L, typename R>
constexpr auto
operator|(const one_of<L...>, const R)
{
  return one_of<L..., R>{};
}
template<typename L, typename... R>
constexpr auto
operator|(const L, const one_of<R...>)
{
  return one_of<L, R...>{};
}
}
}