#include "generators.h"
#include <cxl/parse.hpp>
#include <cxl/string.hpp>

namespace grammar {
using namespace cxl::literals;
using namespace cxl::parse;

// matches whitespace characters
constexpr auto skip = *one_char(STR(" \n\t"));

// returns a parser matching {Target} surrounded by one-or-more whitespace characters
template<typename Target>
constexpr auto
token(const Target)
{
  return skip & Target{} & skip;
}

// returns a parser matching {Target} surrounded by whitespace characters
template<typename Target, typename Delimiter>
constexpr auto
token_list(const Target, const Delimiter)
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
}