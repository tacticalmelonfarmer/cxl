#include "grammar.h"

int main()
{
  using namespace cxl::literals;
  constexpr auto match_result = grammar::value_list.parse(STR("3363, 'Cm`RjacaPa', 691, 7901, 6386, 3106, '+W@Y.%>l^ ', 9755, ' |16x.NKT2', 'pw^klBA=qE', '[s^i^ ^YE0', 169, 4718, 'gqk_Cj#F]x', 5567, 1138, ']qD~`wj=3|', ': h*w= C!A', 'A32cG/u@ ;', 3230, 1759, 5834, 'VwOLW:ytV8', 'Kdq=M:TtUu', 7014, 571, 't5wb+w(63:', 'ws9ymfOf <', 'BS9]`IDk7=', 8258, 'kwsR-=MPCl', 1827, 2604, 7697, 'LhqeG>TL@<', 5004, 3082, 2276, 's*9-8ZMef('")).tree();
}