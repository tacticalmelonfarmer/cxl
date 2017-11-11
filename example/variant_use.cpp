#include <variant.hpp>
#include <assert.h>

struct object{};

using utility::variant;
using var_t = variant<int, float, object, long&, long>;

int main()
{
    var_t a(21);
    assert(a.is_type<int>());

    a = 3.14f;
    assert(a.is_type<float>());

    a.clear();
    assert(a.empty());

    var_t b;
    assert(b.empty());

    const var_t c = object{};
    object o = c.get<object>();

    long l = 23;
    var_t d = l;
    assert(d.is_type<long&>());

    var_t e = 23l;
    assert(e.is_type<long>());
}