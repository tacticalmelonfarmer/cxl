#include <cassert>
#include <cintegral.hpp>
#include <cstdint>
#include <type_traits>
using namespace utility::literals;
using namespace std;
int
main()
{
  constexpr auto ct_index = 42_idx;
  static_assert(ct_index == integral_constant<size_t, 42>{}, "cintegral failure");
  constexpr auto ct_int8 = 42_i8;
  static_assert(ct_int8 == integral_constant<int8_t, 42>{}, "cintegral failure");
  constexpr auto ct_int16 = 42_i16;
  static_assert(ct_int16 == integral_constant<int16_t, 42>{}, "cintegral failure");
  constexpr auto ct_int32 = 42_i32;
  static_assert(ct_int32 == integral_constant<int32_t, 42>{}, "cintegral failure");
  constexpr auto ct_int64 = 42_i64;
  static_assert(ct_int64 == integral_constant<int64_t, 42>{}, "cintegral failure");
  constexpr auto ct_uint8 = 42_u8;
  static_assert(ct_uint8 == integral_constant<uint8_t, 42>{}, "cintegral failure");
  constexpr auto ct_uint16 = 42_u16;
  static_assert(ct_uint16 == integral_constant<uint16_t, 42>{}, "cintegral failure");
  constexpr auto ct_uint32 = 42_u32;
  static_assert(ct_uint32 == integral_constant<uint32_t, 42>{}, "cintegral failure");
  constexpr auto ct_uint64 = 42_u64;
  static_assert(ct_uint64 == integral_constant<uint64_t, 42>{}, "cintegral failure");
}