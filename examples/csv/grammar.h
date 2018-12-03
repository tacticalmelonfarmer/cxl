#include <cxl/parse.hpp>
#include <cxl/string.hpp>

using namespace cxl::literals;
using namespace cxl::parse;

constexpr auto skip_p = *one_char(STR(" \n\t"));
template<typename Target>
constexpr auto
token(const Target)
{
  return skip_p & Target{} & skip_p;
}

template<typename... Input>
struct integer
{
  constexpr operator int() const { return cxl::stoi(Input{}...); }
};

template<typename... Input>
struct floating
{
  constexpr operator float() const { return cxl::stof(Input{}...); }
};

template<typename... Input>
struct quoted_string
{
  constexpr operator const char*() const { return (Input{}, ...); }
};

template<typename Target, typename Delimiter>
constexpr auto
token_list(const Target, const Delimiter)
{
  return token(Target{}) & *(token(Delimiter{}) & token(Target{}));
}

constexpr auto digit_p = one_char(STR("0123456789"));
constexpr auto sign_p = one_char(STR("+-"));
constexpr auto integer_p = (~sign_p & +digit_p).generate(integer<>{});
constexpr auto floating_p = (~sign_p & +digit_p & one_char(STR(".")) & *digit_p).generate(floating<>{});
constexpr auto quote_p = one_char(STR("'"));
constexpr auto string_p = (quote_p & (*(!one_char(STR("'")))).generate(quoted_string<>{}) & quote_p);
constexpr auto value_p = floating_p | integer_p | string_p;
constexpr auto value_list_p = token_list(value_p, one_char(STR(",")));
