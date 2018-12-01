# Constant eXpression Library (abbreviated **cxl**)

## Integral
*include/cxl/integral.hpp* contains templated user-defined 
literal operators that allow you to easily convert literals 
to `std::integral_constant<...>`s.
* literal operator `_i` returns an `index_t` integral constant
* literal operator `_i8` returns an `int8_t` integral constant
* literal operator `_i16` returns an `int16_t` integral constant
* literal operator `_i32` returns an `int32_t` integral constant
* literal operator `_i64` returns an `int64_t` integral constant
* literal operator `_u8` returns an `uint8_t` integral constant
* literal operator `_u16` returns an `uint16_t` integral constant
* literal operator `_u32` returns an `uint32_t` integral constant
* literal operator `_u64` returns an `uint64_t` integral constant

## Iterator
*include/cxl/iterator.hpp* contains an adaptable constexpr iterator class
`cxl::iterator<...>` which can support any constexpr class with the following methods:
* `constexpr auto begin() const`
* `constexpr auto end() const`
* `template<typename T, T value>`
  
  `constexpr auto operator[](std::integral_constant<T,value>) const`

## String
*include/cxl/string.hpp* contains a string class `cxl::string<...>` with 
a completely constexpr compatible interface. It also supports the
`cxl::iterator<...>`s. Elements can be accesed through 
an iterator, operator[] or converting to const char*. String can 
be created through a macro `STR("string")`, because C++17 
does not support templated user-defined string literals. The long way would 
be `cxl::string<'s','t','r','i','n','g'>{}`

## Typelist
*include/cxl/typelist.hpp* contains the typelist sublibrary 
which has a few transform functions and different 
ways to apply/expand a typelist into a user template. 
It is compatible with the iterator sublibrary.

transforms for empty typelists `typelist<>` include:
* append
  * splices a typlist onto the end, this case also the beginning
* apply
  * applies the types to a given template, which is empty argument list
* applied_emplacer
  * applies types to a template and returns an emplacer for that template

transforms/accessors for non-empty typelists include:
* subrange
  * returns a range of types indicated by two inclusive indices
* splice
  * expands another typelist into this typelist at given index
* append
  * expands a typelist into the end of this typelist
* prepend
  * expands a typelist into the beginning of this typelist
* apply
  * expands this typelist into a given user-template
* applied_emplacer
  * applies types to a template and returns an emplacer for that template
* index_of
  * for a given type, returns index of first occurence
* type_at
  * returns a default constructed type indicated by a given index
* operator[]
  * shortcut for type_at, also allows iterating
* type_emplacer
  * similar to type_at, but returns a proxy constructor for non default constructible types
* front
  * returns a default constructed instance of the first type
* back
  * returns a default constructed instance of the last type

# Parse
*include/cxl/parse.hpp* contains the parsing sublibrary. 
There are a handful of predefined fundamental parsers, which operate on `cxl::string<...>`s. 
These parsers can be combined in many 
different ways to create more complex parsers.
When a parse operation is performed using the parser will return a 
`cxl::parse_result<...>` that can tell you if the parse was a success, how far 
the parser matched, what remains to be parsed and a generated typelist.
subparsers can generate custom types, however if they do not, the generated type 
will be a `cxl::string<...>` filled with what the subparser matched. 
custom generated types are just a templated class taking a parameter pack.
see example csv parser-generator in **example/csv**

For example, here is a default generating parser:
```c++
using namespace cxl::parse;
constexpr auto parser = one_string(STR("abc"));
constexpr auto result = parser.parse(STR("abc"));

```
in this case `result.tree()` would return a `typelist<string<'a','b','c'>>`

But if we have a custom generated type:
```c++
template<typename... T>
struct synth{};

using namespace cxl::parse;
constexpr auto parser = one_string(STR("abc")).generate(synth<>{});
constexpr auto result = parser.parse(STR("abc"));

```

now `result.tree()` will return a `typelist<synth<string<'a','b','c'>>>`. 
The rest is up to you...