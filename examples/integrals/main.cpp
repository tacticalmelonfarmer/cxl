#include <cxl/integral.hpp>
#include <type_traits>

int main()
{
    using namespace cxl::literals;
    constexpr auto index = 1_i;
    static_assert(index == 1);
    constexpr auto int8 = 2_i8;
    static_assert(int8 == 2);
    constexpr auto int16 = 3_i16;
    static_assert(int16 == 3);
    constexpr auto int32 = 4_i32;
    static_assert(int32 == 4);
    constexpr auto int64 = 5_i64;
    static_assert(int64 == 5);
    constexpr auto uint8 = 6_u8;
    static_assert(uint8 == 6);
    constexpr auto uint16 = 7_u16;
    static_assert(uint16 == 7);
    constexpr auto uint32 = 8_u32;
    static_assert(uint32 == 8);
    constexpr auto uint64 = 9_u64;
    static_assert(uint64 == 9);
}