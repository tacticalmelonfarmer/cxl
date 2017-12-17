#include <assert.h>
#include <chrono>
#include <iostream>
#include <stack_variant.hpp>
#include <variant>

struct object
{};

using utility::stack_variant;

using svar_t = stack_variant<int, float, object, long&, long>;
using stdvar_t = stack_variant<int, float, object, long&, long>;

int
main()
{
  std::chrono::steady_clock::time_point begin, end;

  ////////////////////////////////////////////////////////////////////////////////////////////
  {
    begin = std::chrono::steady_clock::now();
    svar_t a(21);
    assert(a.is_type<int>());
    assert(a.get<int>() == 21);

    a = 3.14f;
    assert(a.is_type<float>());
    assert(a.get<float>() == 3.14f);

    a.clear();
    assert(a.empty());

    svar_t b;
    assert(b.empty());

    const svar_t c = object{};
    object o = c.get<object>();

    long l = 23;
    svar_t d = l;
    assert(d.is_type<long&>());
    assert(d.get<long&>() == 23);

    svar_t e = 23l;
    assert(e.is_type<long>());
    assert(e.get<long>() == 23l);
    end = std::chrono::steady_clock::now();
  }
  std::cout << "stack: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()
            << ", size = " << sizeof(svar_t) << std::endl;
  ////////////////////////////////////////////////////////////////////////////////////////////
  {
    begin = std::chrono::steady_clock::now();
    stdvar_t a(21);
    assert(a.is_type<int>());
    assert(a.get<int>() == 21);

    a = 3.14f;
    assert(a.is_type<float>());
    assert(a.get<float>() == 3.14f);

    a.clear();
    assert(a.empty());

    stdvar_t b;
    assert(b.empty());

    const stdvar_t c = object{};
    object o = c.get<object>();

    long l = 23;
    stdvar_t d = l;
    assert(d.is_type<long&>());
    assert(d.get<long&>() == 23);

    stdvar_t e = 23l;
    assert(e.is_type<long>());
    assert(e.get<long>() == 23l);
    end = std::chrono::steady_clock::now();
  }
  std::cout << "standard: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()
            << ", size = " << sizeof(stdvar_t) << std::endl;
  return 0;
}