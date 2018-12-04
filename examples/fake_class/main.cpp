#include <cxl/aggregate.hpp>

struct agg
{
  int x, y, z;
};

int
main()
{
  agg my_agg{ 0, 1, 2 };
  return std::get<0>(cxl::destructure(my_agg));
}