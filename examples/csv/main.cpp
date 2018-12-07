#include "grammar.h"
#include <cxl/integral.hpp>

#include <iostream>

int
main()
{
  using namespace cxl::literals;
  constexpr auto match_result = grammar::value_list.parse(STR("1, 2, 3, 42,0, 11 , 91 ,'hi'")).tree();
  constexpr auto fifth_element = match_result[4_i]();
  std::cout << match_result[7_i]() << "\n";
  return fifth_element; // :)
}