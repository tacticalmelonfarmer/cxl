#include "grammar.h"

int
main()
{
  constexpr auto match_result = value_list_p.parse(STR("1, 2, 3, 42,0, 11 , 91 ,'hi'")).tree();
  constexpr auto fifth_element = match_result[4_i]();
  return fifth_element; // :)
}