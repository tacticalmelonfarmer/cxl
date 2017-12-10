#include <assert.h>
#include <cstring.hpp>
#include <iostream>

int
main()
{
  using namespace utility::cexpr::literals;
  constexpr auto str1 = "hello"_cs;
  std::cout << str1.data << std::endl;

  constexpr auto str2 = str1 + ", world!"_cs;
  std::cout << str2.data << std::endl;

  constexpr auto sub1 = utility::cexpr::substr<0, 2>(str1);
  std::cout << sub1.data << std::endl;

  return 0;
}