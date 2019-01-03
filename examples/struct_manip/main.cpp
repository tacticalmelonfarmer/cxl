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
    auto d = cxl::destructure(::std::forward<T>(coord));
    using D = decltype(d);
    if constexpr (std::tuple_size_v<D> == 2)
    {
        std::cout << "2D: " << get<0>(d) << get<1>(d) << "\n";
    }
    else if constexpr (std::tuple_size_v<D> == 3)
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