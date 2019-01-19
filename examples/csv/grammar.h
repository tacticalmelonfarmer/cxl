#include "generators.h"
#include <cxl/parsers.hpp>
#include <cxl/string.hpp>

namespace grammar
{
using namespace cxl::literals;
using namespace cxl::parse;

// matches a whitespace character
constexpr auto skip = one_char(STR(" \n\t"));

// returns a parser matching {Target} followed by one-or-more skip, preceeded by zero-or-more skip
template <typename Target>
constexpr auto token(Target)
{
  return *skip & Target{} & *skip;
}

// returns a parser matching a list of token( {Target} ) seperated by {Delimiter}
template <typename Target, typename Delimiter>
constexpr auto token_list(Target, Delimiter)
{
  return token(Target{}) & *(token(Delimiter{}) & token(Target{}));
}

constexpr auto digit = one_char(STR("0123456789"));
constexpr auto sign = one_char(STR("+-"));
constexpr auto integer = (~sign & +digit).generate(generators::integer<>{});
constexpr auto floating = (~sign & +digit & one_char(STR(".")) & *digit).generate(generators::floating<>{});
constexpr auto quote = one_char(STR("'"));
constexpr auto string = (quote & (*(!one_char(STR("'")))).generate(generators::quoted_string<>{}) & quote);
constexpr auto value = floating | integer | string;
constexpr auto value_list = token_list(value, one_char(STR(",")));

constexpr auto hello_world_i_break_msvc = sequence(skip, skip);
} // namespace grammar