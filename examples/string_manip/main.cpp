#include <cxl/integral.hpp>
#include <cxl/string.hpp>
#include <type_traits>
int
main()
{
  using namespace cxl::literals;

  constexpr auto string = STR("Hello, World!");

  // sub-string from iterators
  constexpr auto word1 = cxl::substr(string.begin(), string.begin() + 5_i);
  static_assert(word1.size() == 5);

  // sub-string from string and position+length
  constexpr auto word2 = cxl::substr(string, 7_i, 5_i);
  static_assert(word2.size() == 5);
}