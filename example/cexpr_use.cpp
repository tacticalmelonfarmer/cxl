#include <assert.h>
#include <cexpr.hpp>

using namespace utility::cexpr;

int
main()
{
  constexpr cstring str1 = "hi";
  constexpr cstring str2 = "hello";
  constexpr cstring str3 = "yo";

  constexpr auto parse1 = one_of_strings(str1, str2);
  constexpr auto parse2 = one_of_strings(str1, str3);
  constexpr auto one_of_p = one_of_parsers(parse1, parse2);

  constexpr auto seqstr = seq_strings(str1, str3).parse(cstring("hiyo"));

  constexpr auto seq_p = seq_parsers(one_of_p, one_of_p).parse(str1, str2);

  return one_of_p.parse(cstring("yo"));
}