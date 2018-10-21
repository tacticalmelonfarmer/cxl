#include <cparse.hpp>
#include <cstring.hpp>
#include <utility.hpp>

using namespace utility::literals;
using namespace utility::parse;
using utility::typelist;

template<typename... Input>
struct integer
{
  constexpr auto operator()() const { return utility::cstoi(Input{}...); }
};

template<typename... Input>
struct floating
{
  constexpr auto operator()() const { return utility::cstof(Input{}...); }
};

template<typename... Input>
struct number_list
{
  const typelist<Input...> input_list = {};
  constexpr auto add() const { return (Input{}() + ...); }
};

constexpr auto skip_p = *one_char(STR(" \n\t"));
template<typename Target>
constexpr auto
token(const Target)
{
  return skip_p & Target{} & skip_p;
}
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
constexpr auto number_p = floating_p | integer_p;
constexpr auto number_list_p = token_list(number_p, one_char(STR(","))).generate(number_list<>{});

int
main()
{

  constexpr auto result = number_list_p.parse(STR("1,2 ,3.14, 42"));
  constexpr auto test = result.tree().largest_size();
  return 0;
}
