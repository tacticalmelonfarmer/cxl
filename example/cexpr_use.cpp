#include <assert.h>
#include <cexpr.hpp>
#include <iostream>

using namespace utility::cexpr;

int
main()
{
  constexpr auto str1 = cstring("hello");
  constexpr auto str2 = str1 + cstring(", world!");
  constexpr cstring str3 = "howdy";
  static_assert(str3[4] == 'y', "fail");

  constexpr carray arr1 = { 1, 2, 3, 4, 5 };
  constexpr auto end = *(arr1.end() - 3);
  constexpr auto tup = ctuple(0, 3.14, 'c', cstring("hello"));
  constexpr auto tupstr = tup.get<3>();

  constexpr auto if_pass =
    if_call(one_of_strings(cstring("hi"), cstring("hello")), []() { return "pass"; }, []() { return "fail"; });
  std::cout << if_pass.parse(str1)() << std::endl;
  std::cout << if_pass.parse(str3)() << std::endl;

  constexpr auto parse1 = one_of_strings(cstring("hi"), cstring("hello"));
  constexpr auto parse2 = one_of_strings(cstring("hi"), cstring("yo"));
  constexpr auto parse_parse = one_of_parsers(parse1, parse2).parse(cstring("yo"));
  constexpr auto seq_parse = seq_strings(str1, str2, str3).parse(str1 + str2 + str3);
  return parse_parse;
}