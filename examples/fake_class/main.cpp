#include <cxl/aggregate.hpp>
#include <cxl/lambda.hpp>
#include <tuple>

struct agg
{
  int x, y, z;
  cxl::lambda::wrap<int(int)> method;
};

int
main() 
{
  agg my_agg{ 0, 1, 2, [](auto i) { return i - 1; } };
  auto my_tup = cxl::destructure(my_agg);
  return std::get<3>(my_tup)(std::get<1>(my_tup));
}