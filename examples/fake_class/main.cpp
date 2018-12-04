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
  agg my_agg{ 0, 1, 2, [](auto i) { return i + 1; } };
  return std::get<0>(cxl::destructure(my_agg));
}