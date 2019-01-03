#include <cxl/aggregate.hpp>
#include <type_traits>
#include <iostream>

struct coord2d
{
    double x, y;
};
struct coord3d
{
    double x, y, z;
};

template <typename T>
void print_coord(T &&coord)
{
    using ::std::get;
    if constexpr (cxl::aggregate_arity_v<T> == 2)
    {
        auto d = cxl::destructure(::std::forward<T>(coord));
        std::cout << "2D: " << get<0>(d) << get<1>(d) << "\n";
    }
    else if constexpr (cxl::aggregate_arity_v<T> == 3)
    {
        auto d = cxl::destructure(::std::forward<T>(coord));
        std::cout << "3D: " << get<0>(d) << get<1>(d) << get<2>(d) << "\n";
    }
}

int main()
{
    print_coord(coord2d{0.0, 1.0});
    print_coord(coord3d{0.0, 1.0, 2.0});
}