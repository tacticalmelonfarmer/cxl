#include "grammar.h"

int
main()
{
  return number_list_p.parse(STR("1, 2, 3, 42,0, 11 , 91   ")).tree()[0_i].add();
}