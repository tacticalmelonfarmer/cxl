#include <cxl/string.hpp>

namespace generators {
// takes a single type as {Input} which should be a cxl::string<...>
template<typename... Input>
struct integer
{
  constexpr operator int() const { return cxl::stoi(Input{}...); }
};

// takes a single type as {Input} which should be a cxl::string<...>
template<typename... Input>
struct floating
{
  constexpr operator float() const { return cxl::stof(Input{}...); }
};

// takes a single type as {Input} which should be a cxl::string<...>
template<typename... Input>
struct quoted_string
{
  constexpr operator const char*() const { return (Input{}, ...); }
};
}