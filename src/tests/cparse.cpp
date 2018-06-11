#include <cparse.hpp>
#include <cstring.hpp>
#include <utility.hpp>

using namespace utility;
using namespace parse;

template<typename Input, typename TreeTL>
struct integer
{
  constexpr operator int() const { return utility::cstoi(Input{}); }
};

int
main()
{
  constexpr auto digits = CSTRING("0123456789");
}
