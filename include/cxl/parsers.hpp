#pragma once

#include "integral.hpp"
#include "parsed.hpp"
#include "string.hpp"
#include "typelist.hpp"
#include "utility.hpp"

namespace cxl
{
namespace parse
{

// this parser matches one instance of a given compile-time string
template <typename TargetString>
struct one_string;

// this parser matches one instance of a given integral constant character
template <typename TargetString>
struct one_char;

// this parser preforms a lookahead match, if succesful the state of the current parsing run is unchanged
// otherwise the parser stop with a failure status
template <typename TargetParser>
struct before;

// this parser tries to match the previously matched symbols, against a new parser
// this can be very usefull for combining parsers in a non-sequential non-disjunctive manner
template <typename TargetParser>
struct after; // NOT YET IMPLEMENTED

// this parser matches anything that is not matched by { TargetParser }, essentially a logical NOT
template <typename TargetParser>
struct filter;

// this parser is self explanatory
template <typename TargetParser>
struct optional;

// this parser matches one or more consecutive occurences of a parser
template <typename TargetParser>
struct one_or_more;

// this parser matches zero or more consecutive occurences of a parser
template <typename TargetParser>
struct zero_or_more;

// this parser matches { TargetParser }, count { I } times
template <typename TargetParser, index_t I>
struct repeat;

// this parser matches { TargetParser }, a minimum of { I } times
template <typename TargetParser, index_t I>
struct repeat_minimum;

// this parser matches { TargetParser }, zero or a maximum of { I } times
template <typename TargetParser, index_t I>
struct repeat_maximum;

// this is somewhat self explanatory, given all the previous parsers
template <typename TargetParser, index_t Min, index_t Max>
struct repeat_range;

// this parser encapsulates an ordered sequence of pre-defined parsers
template <typename InitTargetParser, typename... TargetParsers>
struct sequence;

// this parser is a logical (disjunctive / OR) of an oredered sequence of given parsers
template <typename InitTargetParser, typename... TargetParsers>
struct one_of;

// you know, don't act like you don't
struct anything;

// this is where the magic happens
// give it a parser and an output template and you've got yourself an AST
template <typename TargetParser, template <typename...> typename Output>
struct generator;

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename TargetString>
struct one_string
{
  using parser_tag = tag_t;
  constexpr one_string() {}
  constexpr one_string(TargetString) {}

  template <typename InputString>
  constexpr auto parse(InputString) const
  {
    constexpr auto target_string = TargetString{};
    constexpr auto input_string = InputString{};
    if constexpr (target_string.size() > input_string.size())
    {
      return parsed<string<>, InputString, status::failure>{};
    }
    else
    {
      constexpr index_t size_of_match = strmatch(target_string, input_string);
      if constexpr (size_of_match == 0)
        return parsed<string<>, InputString, status::failure>{};
      if constexpr (size_of_match == target_string.size())
      {
        if constexpr (input_string.begin() + target_string.size() == input_string.end())
          return parsed<TargetString, string<>, status::success>{};
        else
          return parsed<TargetString,
                        decltype(substr(input_string.begin() + target_string.size(), input_string.end())),
                        status::success>{};
      }
      else
        return parsed<string<>, InputString, status::failure>{};
    }
  }

  template <template <typename...> typename Output>
  constexpr auto generate(Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<one_string<TargetString>>{}; }
  constexpr auto operator~() const { return optional<one_string<TargetString>>{}; }
  constexpr auto operator+() const { return one_or_more<one_string<TargetString>>{}; }
  constexpr auto operator*() const { return zero_or_more<one_string<TargetString>>{}; }

  template <index_t I>
  constexpr auto operator[](::std::integral_constant<index_t, I>) const
  {
    return repeat<one_string<TargetString>, I>{};
  }
};

template <typename TargetString>
struct one_char
{
  using parser_tag = tag_t;
  constexpr one_char() {}
  constexpr one_char(TargetString) {}

  template <typename InputString, index_t... Indices>
  constexpr auto parse_impl(InputString, index_range<Indices...>) const
  {
    constexpr auto target_string = TargetString{};
    constexpr auto input_string = InputString{};
    if constexpr ((false || ... || (input_string[0] == target_string[Indices])))
      return parsed<string<input_string[0]>,
                    decltype(substr(++input_string.begin(), input_string.end())),
                    status::success>{};
    else
      return parsed<string<>, InputString, status::failure>{};
  }

  template <typename InputString>
  constexpr auto parse(InputString) const
  {
    constexpr auto target_string = TargetString{};
    constexpr auto input_string = InputString{};
    return parse_impl(input_string, make_index_range<0, target_string.size() - 1>());
  }

  template <template <typename...> typename Output>
  constexpr auto generate(Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<one_char<TargetString>>{}; }
  constexpr auto operator~() const { return optional<one_char<TargetString>>{}; }
  constexpr auto operator+() const { return one_or_more<one_char<TargetString>>{}; }
  constexpr auto operator*() const { return zero_or_more<one_char<TargetString>>{}; }

  template <index_t I>
  constexpr auto operator[](::std::integral_constant<index_t, I>) const
  {
    return repeat<one_char<TargetString>, I>{};
  }
};

template <typename TargetParser>
struct before
{
  using parser_tag = tag_t;
  constexpr before() {}
  constexpr before(TargetParser) {}

  template <typename InputString>
  constexpr auto parse(InputString) const
  {
    constexpr auto input_string = InputString{};
    constexpr auto result = TargetParser{}.parse(input_string);
    if constexpr (result.status())
      return parsed<string<>, InputString, status::success>{};
    else
      return parsed<decltype(result.match()), InputString, status::failure>{};
  }

  template <template <typename...> typename Output>
  constexpr auto generate(Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator~() const { return optional<before<TargetParser>>{}; }
  constexpr auto operator+() const { return one_or_more<before<TargetParser>>{}; }
  constexpr auto operator*() const { return zero_or_more<before<TargetParser>>{}; }

  template <index_t I>
  constexpr auto operator[](::std::integral_constant<index_t, I>) const
  {
    return repeat<before<TargetParser>, I>{};
  }
};

template <char... Chars>
before(string<Chars...> &)->before<one_string<string<Chars...>>>;

template <typename TargetParser>
struct filter
{
  using parser_tag = tag_t;
  constexpr filter() {}
  constexpr filter(TargetParser) {}

  template <typename InputString>
  constexpr auto parse(InputString) const
  {
    constexpr auto input_string = InputString{};
    constexpr auto target_result = TargetParser{}.parse(input_string);
    if constexpr (target_result.status())
      return parsed<string<>, InputString, status::failure>{};
    else
      return parsed<string<input_string[0]>,
                    decltype(substr(++input_string.begin(), input_string.end())),
                    status::success>{};
  }

  template <template <typename...> typename Output>
  constexpr auto generate(Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator~() const { return optional<filter<TargetParser>>{}; }
  constexpr auto operator+() const { return one_or_more<filter<TargetParser>>{}; }
  constexpr auto operator*() const { return zero_or_more<filter<TargetParser>>{}; }

  template <index_t I>
  constexpr auto operator[](::std::integral_constant<index_t, I>) const
  {
    return repeat<filter<TargetParser>, I>{};
  }
};

template <char... Chars>
filter(string<Chars...>)->filter<one_string<string<Chars...>>>;

template <typename TargetParser>
struct optional
{
  using parser_tag = tag_t;
  constexpr optional() {}
  constexpr optional(TargetParser) {}

  template <typename InputString>
  constexpr auto parse(InputString) const
  {
    constexpr auto result = TargetParser{}.parse(InputString{});
    if constexpr (result.status())
      return result;
    else
      return parsed<string<>, InputString, status::success>{};
  }

  template <template <typename...> typename Output>
  constexpr auto generate(Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<optional<TargetParser>>{}; }
  constexpr auto operator+() const { return one_or_more<optional<TargetParser>>{}; }
  constexpr auto operator*() const { return zero_or_more<optional<TargetParser>>{}; }

  template <index_t I>
  constexpr auto operator[](::std::integral_constant<index_t, I>) const
  {
    return repeat<optional<TargetParser>, I>{};
  }
};

template <char... Chars>
optional(string<Chars...>)->optional<one_string<string<Chars...>>>;

template <typename TargetParser>
struct one_or_more
{
  using parser_tag = tag_t;
  constexpr one_or_more() {}
  constexpr one_or_more(TargetParser) {}

  template <index_t Count = 0, typename InputString>
  constexpr auto parse(InputString) const
  {
    constexpr TargetParser target_parser;
    constexpr InputString input_string;
    constexpr auto result = target_parser.parse(input_string);
    if constexpr (Count == 0)
    {
      if constexpr (result.status())
        return result + parse<Count + 1>(substr(input_string.begin() + result.match().size(), input_string.end()));
      else
        return result;
    }
    else
    {
      if constexpr (result.status())
        return result + parse<Count + 1>(substr(input_string.begin() + result.match().size(), input_string.end()));
      else
        return result.succeed();
    }
  }

  template <template <typename...> typename Output>
  constexpr auto generate(Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<one_or_more<TargetParser>>{}; }
  constexpr auto operator~() const { return optional<one_or_more<TargetParser>>{}; }
};

template <char... Chars>
one_or_more(string<Chars...>)->one_or_more<one_string<string<Chars...>>>;

template <typename TargetParser>
struct zero_or_more
{
  using parser_tag = tag_t;
  constexpr zero_or_more() {}
  constexpr zero_or_more(TargetParser) {}

  template <index_t Count = 0, typename InputString>
  constexpr auto parse(InputString) const
  {
    constexpr TargetParser target_parser;
    constexpr InputString input_string;
    constexpr auto result = target_parser.parse(input_string);

    if constexpr (result.status())
      return result + parse<Count + 1>(substr(input_string.begin() + result.match().size(), input_string.end()));
    else
      return result.succeed();
  }

  template <template <typename...> typename Output>
  constexpr auto generate(Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<zero_or_more<TargetParser>>{}; }
};

template <char... Chars>
zero_or_more(string<Chars...>)->zero_or_more<one_string<string<Chars...>>>;

//////////////////////////////////////////////////////////////

template <index_t U, typename T>
struct identity
{
  using type = T;
};

template <typename TargetParser, index_t I>
struct repeat
{
  using parser_tag = tag_t;
  constexpr repeat() {}
  constexpr repeat(TargetParser, ::std::integral_constant<index_t, I>) {}

  template <index_t... Indices>
  constexpr auto make_sequence(index_range<Indices...>) const
  {
    return sequence<decltype((Indices, TargetParser{}))...>{};
  }

  template <typename InputString>
  constexpr auto parse(InputString) const
  {
    return make_sequence(make_index_range<0, I - 1>()).parse(InputString{});
  }

  template <template <typename...> typename Output>
  constexpr auto generate(Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<repeat<TargetParser, I>>{}; }
  constexpr auto operator~() const { return optional<repeat<TargetParser, I>>{}; }
  constexpr auto operator++(int) const { return repeat_minimum<TargetParser, I>{}; }
  constexpr auto operator--(int) const { return repeat_maximum<TargetParser, I>{}; }
  template <index_t J>
  constexpr auto operator[](::std::integral_constant<index_t, J>) const
  {
    return repeat_range<TargetParser, I, J>{};
  }
};

template <char... Chars, index_t I>
repeat(string<Chars...>, ::std::integral_constant<index_t, I>)->repeat<one_string<string<Chars...>>, I>;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

template <typename TargetParser, index_t I>
struct repeat_minimum
{
  using parser_tag = tag_t;
  constexpr repeat_minimum() {}
  constexpr repeat_minimum(TargetParser, ::std::integral_constant<index_t, I>) {}

  template <index_t... Indices>
  constexpr auto make_sequence(index_range<Indices...>) const
  {
    constexpr auto new_targets =
        typelist<decltype((Indices, TargetParser{}))...>{}.append(typelist<decltype(*TargetParser{})>{});
    return new_targets.template apply<sequence>();
  }

  template <index_t Count = 0, typename MatchedSoFar = string<>, typename InputString>
  constexpr auto parse(InputString) const
  {
    return make_sequence(make_index_range<0, I - 1>()).parse(InputString{});
  }

  template <template <typename...> typename Output>
  constexpr auto generate(Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<repeat_minimum<TargetParser, I>>{}; }
  constexpr auto operator~() const { return optional<repeat_minimum<TargetParser, I>>{}; }
};

template <char... Chars, index_t I>
repeat_minimum(string<Chars...>, ::std::integral_constant<index_t, I>)->repeat_minimum<one_string<string<Chars...>>, I>;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

template <typename TargetParser, index_t I>
struct repeat_maximum
{
  using parser_tag = tag_t;
  constexpr repeat_maximum() {}
  constexpr repeat_maximum(TargetParser, ::std::integral_constant<index_t, I>) {}

  template <typename InputString>
  constexpr auto parse(InputString) const
  {
    constexpr auto input_string = InputString{};
    constexpr auto base = TargetParser{}.parse(input_string);
    constexpr auto result = zero_or_more<TargetParser>{}.parse(input_string);
    if constexpr (base.status())
    {
      if constexpr (result.match().size() <= (I * base.match().size()))
        return result;
      else
        return parsed<decltype(result.match()), InputString, status::failure>{};
    }
    else
      return parsed<string<>, InputString, status::success>{};
  }

  template <template <typename...> typename Output>
  constexpr auto generate(Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<repeat_maximum<TargetParser, I>>{}; }
  constexpr auto operator~() const { return optional<repeat_maximum<TargetParser, I>>{}; }
};

template <char... Chars, index_t I>
repeat_maximum(string<Chars...>, ::std::integral_constant<index_t, I>)->repeat_maximum<one_string<string<Chars...>>, I>;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

template <typename TargetParser, index_t Min, index_t Max>
struct repeat_range
{
  using parser_tag = tag_t;
  constexpr repeat_range() {}
  constexpr repeat_range(TargetParser, ::std::integral_constant<index_t, Min>, ::std::integral_constant<index_t, Max>) {}

  template <typename InputString>
  constexpr auto parse(InputString) const
  {
    constexpr auto input_string = InputString{};
    constexpr auto base = TargetParser{}.parse(input_string);
    constexpr auto result = zero_or_more<TargetParser>{}.parse(input_string);
    if constexpr (base.status())
    {
      if constexpr (result.match().size() >= (Min * base.match().size()) &&
                    result.match().size() <= (Max * base.match().size()))
        return result;
      else
        return parsed<decltype(result.match()), InputString, status::failure>{};
    }
    else
      return parsed<string<>, InputString, status::success>{};
  }

  template <template <typename...> typename Output>
  constexpr auto generate(Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator~() const { return optional<repeat_range<TargetParser, Min, Max>>{}; }
  constexpr auto operator!() const { return filter<repeat_range<TargetParser, Min, Max>>{}; }
};

template <char... Chars, index_t Min, index_t Max>
repeat_range(string<Chars...>, ::std::integral_constant<index_t, Min>, ::std::integral_constant<index_t, Max>)
    ->repeat_range<one_string<string<Chars...>>, Min, Max>;

//////////////////////////////////////////////////////////////

template <typename InitTargetParser, typename... TargetParsers>
struct sequence
{
  using parser_tag = tag_t;
  constexpr sequence() {}
  constexpr sequence(InitTargetParser, TargetParsers...) {}

  template <typename InputString,
            index_t End = sizeof...(TargetParsers),
            index_t CurrentIndex = 0,
            typename CurrentTargetParser = InitTargetParser,
            typename... NextTargetParsers>
  constexpr auto parse(InputString) const
  {
    if constexpr (CurrentIndex == 0)
    {
      constexpr InputString input_string;
      constexpr CurrentTargetParser target_parser;
      constexpr auto result = target_parser.parse(input_string);
      if constexpr (result.status())
        return result +
               parse<decltype(result.remainder()), End, CurrentIndex + 1, TargetParsers...>(result.remainder());
      else
        return result;
    }
    else if constexpr (CurrentIndex > 0 && CurrentIndex < End)
    {
      constexpr InputString input_string;
      constexpr CurrentTargetParser target_parser;
      constexpr auto result = target_parser.parse(input_string);
      if constexpr (result.status())
        return result +
               parse<decltype(result.remainder()), End, CurrentIndex + 1, NextTargetParsers...>(result.remainder());
      else
        return result;
    }
    else
    {
      constexpr InputString input_string;
      constexpr CurrentTargetParser target_parser;
      constexpr auto result = target_parser.parse(input_string);
      return result;
    }
  }

  template <template <typename...> typename Output>
  constexpr auto generate(Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<sequence<InitTargetParser, TargetParsers...>>{}; }
  constexpr auto operator~() const { return optional<sequence<InitTargetParser, TargetParsers...>>{}; }
  constexpr auto operator+() const { return one_or_more<sequence<InitTargetParser, TargetParsers...>>{}; }
  constexpr auto operator*() const { return zero_or_more<sequence<InitTargetParser, TargetParsers...>>{}; }

  template <index_t I>
  constexpr auto operator[](::std::integral_constant<index_t, I>) const
  {
    return repeat<sequence<InitTargetParser, TargetParsers...>, I>{};
  }
};

template <typename InitTargetParser, typename... TargetParsers>
struct one_of
{
  using parser_tag = tag_t;
  constexpr one_of() {}
  constexpr one_of(InitTargetParser, TargetParsers...) {}

  template <typename InputString, typename CurrentTargetParser, typename... NextTargetParsers>
  constexpr auto parse_impl() const
  {
    constexpr auto result = CurrentTargetParser{}.parse(InputString{});
    if constexpr (result.status())
      return result;
    else if constexpr (sizeof...(NextTargetParsers) == 0)
    {
      if constexpr (result.status())
        return result;
      else
        return parsed<decltype(result.match()), InputString, status::failure>{};
    }
    else
      return parse_impl<InputString, NextTargetParsers...>();
  }

  template <typename InputString>
  constexpr auto parse(InputString) const
  {
    return decltype(*this){}.template parse_impl<InputString, InitTargetParser, TargetParsers...>();
  }

  template <template <typename...> typename Output>
  constexpr auto generate(Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator!() const { return filter<one_of<InitTargetParser, TargetParsers...>>{}; }
  constexpr auto operator~() const { return optional<one_of<InitTargetParser, TargetParsers...>>{}; }
  constexpr auto operator+() const { return one_or_more<one_of<InitTargetParser, TargetParsers...>>{}; }
  constexpr auto operator*() const { return zero_or_more<one_of<InitTargetParser, TargetParsers...>>{}; }

  template <index_t I>
  constexpr auto operator[](::std::integral_constant<index_t, I>) const
  {
    return repeat<one_of<InitTargetParser, TargetParsers...>, I>{};
  }
};

struct anything
{
  using parser_tag = tag_t;
  constexpr anything() {}

  template <typename InputString>
  constexpr auto parse(InputString) const
  {
    constexpr auto input_string = InputString{};
    static_assert(input_string.size() > 0, "cxl::string<...> not big enough to hold anything");
    return parsed<string<input_string[0]>,
                  decltype(substr(++input_string.begin(), input_string.end())),
                  status::success>{};
  }

  template <template <typename...> typename Output>
  constexpr auto generate(Output<>) const
  {
    return generator(decltype(*this){}, Output<>{});
  }

  constexpr auto operator~() const { return optional<anything>{}; }
  constexpr auto operator+() const { return one_or_more<anything>{}; }
  constexpr auto operator*() const { return zero_or_more<anything>{}; }

  template <index_t I>
  constexpr auto operator[](::std::integral_constant<index_t, I>) const
  {
    return repeat<anything, I>{};
  }
};

//////////////////////////////////////////////////////////////

template <typename TargetParser, template <typename...> typename Output>
struct generator
{
  using parser_tag = tag_t;
  constexpr generator() {}
  constexpr generator(TargetParser, Output<>) {}

  template <typename InputString>
  constexpr auto parse(InputString) const
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

  template <index_t I>
  constexpr auto operator[](::std::integral_constant<index_t, I>) const
  {
    return repeat<generator<TargetParser, Output>, I>{};
  }
};

//////////////////////////////////////////////////////////////////////////////////

template <typename L, typename R>
constexpr auto operator&(L, R)
{
  return sequence<L, R>{};
}
template <typename... L, typename... R>
constexpr auto operator&(sequence<L...>, sequence<R...>)
{
  return sequence<L..., R...>{};
}
template <typename... L, typename R>
constexpr auto operator&(sequence<L...>, R)
{
  return sequence<L..., R>{};
}
template <typename L, typename... R>
constexpr auto operator&(L, sequence<R...>)
{
  return sequence<L, R...>{};
}
template <typename L, typename R>
constexpr auto operator|(L, R)
{
  return one_of<L, R>{};
}
template <typename... L, typename... R>
constexpr auto
operator|(one_of<L...>, one_of<R...>)
{
  return one_of<L..., R...>{};
}
template <typename... L, typename R>
constexpr auto
operator|(one_of<L...>, R)
{
  return one_of<L..., R>{};
}
template <typename L, typename... R>
constexpr auto
operator|(L, one_of<R...>)
{
  return one_of<L, R...>{};
}
} // namespace parse
} // namespace cxl