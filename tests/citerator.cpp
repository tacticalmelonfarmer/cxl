#include <citerator.hpp>
#include <cstring.hpp>
#include <type_traits>

using namespace utility;
using namespace literals;
using namespace std;

int
main()
{
  constexpr auto test_string = STR("Hello, World!");
  static_assert(test_string == cstring<'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!'>{}, "fail");
  constexpr auto begin = test_string.begin();
  static_assert(*begin == 'H', "fail");
  constexpr auto end = test_string.end();
  static_assert(*end == '\0', "fail");
  constexpr auto iter_to_comma = begin + 5_idx;
  static_assert(*iter_to_comma == ',', "fail");
  constexpr auto iter_to_exclamation = --end;
  static_assert(*iter_to_exclamation == '!', "fail");
}