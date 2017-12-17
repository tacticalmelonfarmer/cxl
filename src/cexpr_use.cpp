#include <assert.h>
#include <cparse.hpp>
#include <cstring.hpp>
#include <iostream>
#include <type_traits>

int
main()
{
  using namespace utility::cexpr::literals;
  using utility::cexpr::strlen;
  using utility::cexpr::substr;

  constexpr auto str1 = "hello"_cs;
  std::cout << str1 << " len: " << strlen(str1) << std::endl;

  constexpr auto str2 = str1 + ", world!"_cs;
  std::cout << str2 << " len: " << strlen(str2.begin(), str2.end()) << std::endl;

  constexpr auto sub1 = substr<0, 2>(str1);
  constexpr auto sub2 = substr(str2.begin() + 3_ci, str2.end());
  std::cout << sub1 << sub2 << " len: " << strlen(sub1 + sub2) << std::endl;

  constexpr auto one_parser = "hi"_one;
  std::cout << one_parser.parse("hi"_cs).string() << std::endl; // ""

  constexpr auto star_parser = "hi"_star;
  std::cout << star_parser.parse("hihiho"_cs).string() << std::endl; // "ho"
  std::cout << star_parser.parse("hello"_cs).string() << std::endl;  // "hello"

  constexpr auto plus_parser = "hi"_plus;
  std::cout << plus_parser.parse("hihihiho"_cs).string() << std::endl; // "ho"

  constexpr auto seq_parser = "hi"_one && "hi"_one;
  std::cout << seq_parser.parse("hiho"_cs).string() << std::endl; // "hiho"
  std::cout << seq_parser.parse("hihi"_cs).string() << std::endl; // ""

  constexpr auto or_parser = "hello"_one || "goodbye"_one;
  std::cout << or_parser.parse("hello!"_cs).string() << std::endl;
  std::cout << or_parser.parse("goodbye!"_cs).string() << std::endl;

  return 0;
}