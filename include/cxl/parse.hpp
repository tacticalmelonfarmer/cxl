#pragma once
#include "string.hpp"
#include "typelist.hpp"
#include "utility.hpp"

namespace cxl {

namespace parse {

enum class parse_status
{
  success = 1,
  failure = 0
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

template<typename TargetParser>
struct before;

template<typename TargetParser>
struct filter;

template<typename TargetParser>
struct optional;

template<typename TargetParser>
struct one_or_more;

template<typename TargetParser>
struct zero_or_more;

template<typename TargetParser, index_t I>
struct repeat;

template<typename TargetParser, index_t I>
struct repeat_minimum;

template<typename TargetParser, index_t I>
struct repeat_maximum;

template<typename TargetParser, index_t Min, index_t Max>
struct repeat_range;

template<typename InitTargetParser, typename... TargetParsers>
struct sequence;

template<typename InitTargetParser, typename... TargetParsers>
struct one_of;

template<typename TargetParser, template<typename...> typename Output>
struct generator;

/////////////////////////// parsers

template<typename TargetString>
struct one_string
{
  constexpr one_string() {}
  constexpr one_string(const TargetString) {}

  template<typename InputString>
  constexpr auto parse(const InputString) const
  {
    constexpr auto target_string = TargetString{};
    constexpr auto input_string = InputString{};
    if constexpr (target_string.size() > input_string.size()) {
      return parse_result<string<>, InputString, parse_status::failure, typelist<>>{};
    } else {
      constexpr index_t size_of_match = match(target_string, input_string);
      if constexpr (size_of_match == 0)
        return parse_result<string<>, InputString, parse_status::failure, typelist<>>{};
      if constexpr (size_of_match == target_string.size()) {
        if constexpr (input_string.begin() + target_string.size() == input_string.end())
          return parse_result<TargetString, string<>, parse_status::success, typelist<>>{};
        else
          return parse_result<TargetString,
                              decltype(substr(input_string.begin() + target_string.size(), input_string.end())),
                              parse_status::success,
                              typelist<>>{};
      } else
        return parse_result<string<>, InputString, parse_status::failure, typelist<>>{};
    }
  }

  template<typename BeginIter, typename EndIter>
  constexpr auto parse(const BeginIter, const EndIter) const
  {
    constexpr auto begin = BeginIter{};
    constexpr auto end = EndIter{};
    // implement cxl::distance for iterators
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

  template<typename InputString, index_t... Indices>
  constexpr auto parse_impl(const InputString, const index_range<Indices...>) const
  {
    constexpr auto target_string = TargetString{};
    constexpr auto input_string = InputString{};
    if constexpr ((false || ... || (input_string[0] == target_string[Indices])))
      return parse_result<string<input_string[0]>,
                          decltype(substr(++input_string.begin(), input_string.end())),
                          parse_status::success,
                          typelist<>>{};
    else
      return parse_result<string<>, InputString, parse_status::failure, typelist<>>{};
  }

  template<typename InputString>
  constexpr auto parse(const InputString) const
  {
    constexpr auto target_string = TargetString{};
    constexpr auto input_string = InputString{};
    return parse_impl(input_string, make_index_range<0, target_string.size() - 1>());
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

template<typename TargetParser>
struct before
{
  constexpr before() {}
  constexpr before(const TargetParser) {}

  template<typename InputString>
  constexpr auto parse(const InputString) const
  {
    constexpr auto input_string = InputString{};
    constexpr auto result = TargetParser{}.parse(input_string);
    if constexpr (result.status())
      return parse_result<string<>, InputString, parse_status::success, typelist<>>{};
    else
      return parse_result<decltype(result.match()), InputString, parse_status::failure, typelist<>>{};
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator~() const { return optional<before<TargetParser>>{}; }
  constexpr auto operator+() const { return one_or_more<before<TargetParser>>{}; }
  constexpr auto operator*() const { return zero_or_more<before<TargetParser>>{}; }

  template<index_t I>
  constexpr auto operator[](const std::integral_constant<index_t, I>) const
  {
    return repeat<before<TargetParser>, I>{};
  }
};

template<char... Chars>
before(const string<Chars...>&)->before<one_string<string<Chars...>>>;

template<typename TargetParser>
struct filter
{
  constexpr filter() {}
  constexpr filter(const TargetParser) {}

  template<typename InputString>
  constexpr auto parse(const InputString) const
  {
    constexpr auto input_string = InputString{};
    constexpr auto result = TargetParser{}.parse(input_string);
    if constexpr (result.status())
      return parse_result<decltype(result.match()), InputString, parse_status::failure, typelist<>>{};
    else
      return parse_result<decltype(substr(input_string.begin(), input_string.begin())),
                          decltype(substr(++input_string.begin(), input_string.end())),
                          parse_status::success,
                          typelist<>>{};
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator~() const { return optional<filter<TargetParser>>{}; }
  constexpr auto operator+() const { return one_or_more<filter<TargetParser>>{}; }
  constexpr auto operator*() const { return zero_or_more<filter<TargetParser>>{}; }

  template<index_t I>
  constexpr auto operator[](const std::integral_constant<index_t, I>) const
  {
    return repeat<filter<TargetParser>, I>{};
  }
};

template<char... Chars>
filter(const string<Chars...>)->filter<one_string<string<Chars...>>>;

template<typename TargetParser>
struct optional
{
  constexpr optional() {}
  constexpr optional(const TargetParser) {}

  template<typename InputString>
  constexpr auto parse(const InputString) const
  {
    constexpr auto result = TargetParser{}.parse(InputString{});
    if constexpr (result.status())
      return result;
    else
      return parse_result<string<>, InputString, parse_status::success, typelist<>>{};
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<optional<TargetParser>>{}; }
  constexpr auto operator+() const { return one_or_more<optional<TargetParser>>{}; }
  constexpr auto operator*() const { return zero_or_more<optional<TargetParser>>{}; }

  template<index_t I>
  constexpr auto operator[](const std::integral_constant<index_t, I>) const
  {
    return repeat<optional<TargetParser>, I>{};
  }
};

template<char... Chars>
optional(const string<Chars...>)->optional<one_string<string<Chars...>>>;

template<typename TargetParser>
struct one_or_more
{
  constexpr one_or_more() {}
  constexpr one_or_more(const TargetParser) {}

  template<index_t Count = 0, typename InputString>
  constexpr auto parse(const InputString) const
  {
    constexpr TargetParser target_parser;
    constexpr InputString input_string;
    constexpr auto result = target_parser.parse(input_string);
    if constexpr (Count == 0) {
      if constexpr (result.status())
        return result + parse<Count + 1>(substr(input_string.begin() + result.match().size(), input_string.end()));
      else
        return result;
    } else {
      if constexpr (result.status())
        return result + parse<Count + 1>(substr(input_string.begin() + result.match().size(), input_string.end()));
      else
        return result.succeed();
    }
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<one_or_more<TargetParser>>{}; }
  constexpr auto operator~() const { return optional<one_or_more<TargetParser>>{}; }
};

template<char... Chars>
one_or_more(const string<Chars...>)->one_or_more<one_string<string<Chars...>>>;

template<typename TargetParser>
struct zero_or_more
{
  constexpr zero_or_more() {}
  constexpr zero_or_more(const TargetParser) {}

  template<index_t Count = 0, typename InputString>
  constexpr auto parse(const InputString) const
  {
    constexpr TargetParser target_parser;
    constexpr InputString input_string;
    constexpr auto result = target_parser.parse(input_string);

    if constexpr (result.status())
      return result + parse<Count + 1>(substr(input_string.begin() + result.match().size(), input_string.end()));
    else
      return result.succeed();
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<zero_or_more<TargetParser>>{}; }
};

template<char... Chars>
zero_or_more(const string<Chars...>)->zero_or_more<one_string<string<Chars...>>>;

//////////////////////////////////////////////////////////////

template<index_t U, typename T>
struct identity
{
  using type = T;
};

template<typename TargetParser, index_t I>
struct repeat
{
  constexpr repeat() {}
  constexpr repeat(const TargetParser, const std::integral_constant<index_t, I>) {}

  template<index_t... Indices>
  constexpr auto make_sequence(const index_range<Indices...>) const
  {
    return sequence<decltype((Indices, TargetParser{}))...>{};
  }

  template<typename InputString>
  constexpr auto parse(const InputString) const
  {
    return make_sequence(make_index_range<0, I - 1>()).parse(InputString{});
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<repeat<TargetParser, I>>{}; }
  constexpr auto operator~() const { return optional<repeat<TargetParser, I>>{}; }
  constexpr auto operator++(int) const { return repeat_minimum<TargetParser, I>{}; }
  constexpr auto operator--(int) const { return repeat_maximum<TargetParser, I>{}; }
  template<index_t J>
  constexpr auto operator[](const std::integral_constant<index_t, J>) const
  {
    return repeat_range<TargetParser, I, J>{};
  }
};

template<char... Chars, index_t I>
repeat(const string<Chars...>, const std::integral_constant<index_t, I>)->repeat<one_string<string<Chars...>>, I>;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

template<typename TargetParser, index_t I>
struct repeat_minimum
{
  constexpr repeat_minimum() {}
  constexpr repeat_minimum(const TargetParser, const std::integral_constant<index_t, I>) {}

  template<index_t... Indices>
  constexpr auto make_sequence(const index_range<Indices...>) const
  {
    constexpr auto new_targets =
      typelist<decltype((Indices, TargetParser{}))...>{}.append(typelist<decltype(*TargetParser{})>{});
    return new_targets.template apply<sequence>();
  }

  template<index_t Count = 0, typename MatchedSoFar = string<>, typename InputString>
  constexpr auto parse(const InputString) const
  {
    return make_sequence(make_index_range<0, I - 1>()).parse(InputString{});
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<repeat_minimum<TargetParser, I>>{}; }
  constexpr auto operator~() const { return optional<repeat_minimum<TargetParser, I>>{}; }
};

template<char... Chars, index_t I>
repeat_minimum(const string<Chars...>, const std::integral_constant<index_t, I>)
  ->repeat_minimum<one_string<string<Chars...>>, I>;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

template<typename TargetParser, index_t I>
struct repeat_maximum
{
  constexpr repeat_maximum() {}
  constexpr repeat_maximum(const TargetParser, const std::integral_constant<index_t, I>) {}

  template<typename InputString>
  constexpr auto parse(const InputString) const
  {
    constexpr auto input_string = InputString{};
    constexpr auto base = TargetParser{}.parse(input_string);
    constexpr auto result = zero_or_more<TargetParser>{}.parse(input_string);
    if constexpr (base.status()) {
      if constexpr (result.match().size() <= (I * base.match().size()))
        return result;
      else
        return parse_result<decltype(result.match()), InputString, parse_status::failure, typelist<>>{};
    } else
      return parse_result<string<>, InputString, parse_status::success, typelist<>>{};
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<repeat_maximum<TargetParser, I>>{}; }
  constexpr auto operator~() const { return optional<repeat_maximum<TargetParser, I>>{}; }
};

template<char... Chars, index_t I>
repeat_maximum(const string<Chars...>, const std::integral_constant<index_t, I>)
  ->repeat_maximum<one_string<string<Chars...>>, I>;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

template<typename TargetParser, index_t Min, index_t Max>
struct repeat_range
{
  constexpr repeat_range() {}
  constexpr repeat_range(const TargetParser,
                         const std::integral_constant<index_t, Min>,
                         const std::integral_constant<index_t, Max>)
  {}

  template<typename InputString>
  constexpr auto parse(const InputString) const
  {
    constexpr auto input_string = InputString{};
    constexpr auto base = TargetParser{}.parse(input_string);
    constexpr auto result = zero_or_more<TargetParser>{}.parse(input_string);
    if constexpr (base.status()) {
      if constexpr (result.match().size() >= (Min * base.match().size()) &&
                    result.match().size() <= (Max * base.match().size()))
        return result;
      else
        return parse_result<decltype(result.match()), InputString, parse_status::failure, typelist<>>{};
    } else
      return parse_result<string<>, InputString, parse_status::success, typelist<>>{};
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator~() const { return optional<repeat_range<TargetParser, Min, Max>>{}; }
  constexpr auto operator!() const { return filter<repeat_range<TargetParser, Min, Max>>{}; }
};

template<char... Chars, index_t Min, index_t Max>
repeat_range(const string<Chars...>,
             const std::integral_constant<index_t, Min>,
             const std::integral_constant<index_t, Max>)
  ->repeat_range<one_string<string<Chars...>>, Min, Max>;

//////////////////////////////////////////////////////////////

template<typename InitTargetParser, typename... TargetParsers>
struct sequence
{
  constexpr sequence() {}
  constexpr sequence(const InitTargetParser, const TargetParsers...) {}

  template<typename InputString,
           index_t End = sizeof...(TargetParsers),
           index_t CurrentIndex = 0,
           typename CurrentTargetParser = InitTargetParser,
           typename... NextTargetParsers>
  constexpr auto parse(const InputString) const
  {
    if constexpr (CurrentIndex == 0) {
      constexpr InputString input_string;
      constexpr CurrentTargetParser target_parser;
      constexpr auto result = target_parser.parse(input_string);
      if constexpr (result.status())
        return result +
               parse<decltype(result.remainder()), End, CurrentIndex + 1, TargetParsers...>(result.remainder());
      else
        return result;
    } else if constexpr (CurrentIndex > 0 && CurrentIndex < End) {
      constexpr InputString input_string;
      constexpr CurrentTargetParser target_parser;
      constexpr auto result = target_parser.parse(input_string);
      if constexpr (result.status())
        return result +
               parse<decltype(result.remainder()), End, CurrentIndex + 1, NextTargetParsers...>(result.remainder());
      else
        return result;
    } else {
      constexpr InputString input_string;
      constexpr CurrentTargetParser target_parser;
      constexpr auto result = target_parser.parse(input_string);
      return result;
    }
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<sequence<InitTargetParser, TargetParsers...>>{}; }
  constexpr auto operator~() const { return optional<sequence<InitTargetParser, TargetParsers...>>{}; }
  constexpr auto operator+() const { return one_or_more<sequence<InitTargetParser, TargetParsers...>>{}; }
  constexpr auto operator*() const { return zero_or_more<sequence<InitTargetParser, TargetParsers...>>{}; }

  template<index_t I>
  constexpr auto operator[](const std::integral_constant<index_t, I>) const
  {
    return repeat<sequence<InitTargetParser, TargetParsers...>, I>{};
  }
};

template<typename InitTargetParser, typename... TargetParsers>
struct one_of
{
  constexpr one_of() {}
  constexpr one_of(const InitTargetParser, const TargetParsers...) {}

  template<typename InputString, typename CurrentTargetParser, typename... NextTargetParsers>
  constexpr auto parse_impl() const
  {
    constexpr auto result = CurrentTargetParser{}.parse(InputString{});
    if constexpr (result.status())
      return result;
    else if constexpr (sizeof...(NextTargetParsers) == 0) {
      if constexpr (result.status())
        return result;
      else
        return parse_result<decltype(result.match()), InputString, parse_status::failure, typelist<>>{};
    } else
      return parse_impl<InputString, NextTargetParsers...>();
  }

  template<typename InputString>
  constexpr auto parse(const InputString) const
  {
    return decltype(*this){}.template parse_impl<InputString, InitTargetParser, TargetParsers...>();
  }

  template<template<typename...> typename Output>
  constexpr auto generate(const Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<one_of<InitTargetParser, TargetParsers...>>{}; }
  constexpr auto operator~() const { return optional<one_of<InitTargetParser, TargetParsers...>>{}; }
  constexpr auto operator+() const { return one_or_more<one_of<InitTargetParser, TargetParsers...>>{}; }
  constexpr auto operator*() const { return zero_or_more<one_of<InitTargetParser, TargetParsers...>>{}; }

  template<index_t I>
  constexpr auto operator[](const std::integral_constant<index_t, I>) const
  {
    return repeat<one_of<InitTargetParser, TargetParsers...>, I>{};
  }
};

//////////////////////////////////////////////////////////////

template<typename TargetParser, template<typename...> typename Output>
struct generator
{
  constexpr generator() {}
  constexpr generator(const TargetParser, const Output<>) {}

  template<typename InputString>
  constexpr auto parse(const InputString) const
  {
    constexpr auto target_parser = TargetParser{};
    constexpr auto input_string = InputString{};
    constexpr auto result = target_parser.parse(input_string);
    if constexpr (result.tree().size() == 0)
      return result.generate(Output<decltype(result.match())>{});
    else
      return result.generate(result.tree().template apply<Output>());
  }

  constexpr auto disable() const { return TargetParser{}; }

  constexpr auto operator!() const { return filter<generator<TargetParser, Output>>{}; }
  constexpr auto operator~() const { return optional<generator<TargetParser, Output>>{}; }
  constexpr auto operator+() const { return one_or_more<generator<TargetParser, Output>>{}; }
  constexpr auto operator*() const { return zero_or_more<generator<TargetParser, Output>>{}; }

  template<index_t I>
  constexpr auto operator[](const std::integral_constant<index_t, I>) const
  {
    return repeat<generator<TargetParser, Output>, I>{};
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