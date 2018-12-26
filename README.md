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

methods for empty typelists include:
* `.append(Typelist)`
  * expands a typelist into this, the only valid operation for an empty typelist

methods for non-empty typelists include:
* `.subrange(Begin, End)`
  * returns a range of types indicated by two inclusive compile-time indices
* `.insert(Typelist)`
  * expands another typelist into this typelist at given index
* `.append(Typelist)`
  * expands a typelist into the end of this typelist
* `.prepend(Typelist)`
  * expands a typelist into the beginning of this typelist
* `.erase(Index)`
  * returns a typelist, with type at given index removed
* `.erase(Begin, End)`
  * returns a typelist, with types between given indices removed
* `.erase(BeginIter, EndIter)`
  * returns a typelist, with types between given iterators removed
* `.applied_emplacer()`
  * applies types to a templated class and returns an emplacer for that class
* `.template index_of<T>()`
  * for a given type, returns index of first occurence
* `.type_emplacer(Index)`
  * returns a proxy constructor for a type at given index
* `.operator[](Index)`
  * equivalent to `type_emplacer`, enables `cxl::iterator` compatibility
* `.front()`
  * returns a proxy constructor for the first type
* `.back()`
  * returns a proxy constructor for the last type

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

Built-in parser classes include:
* `one_string`
  * `(constructor)(TargetString)`: takes a string as target
  * `.parse(StringToParse)`: parses a string and matches target; consumes on success
  * `.generate(GeneratorTemplate)`: returns a new parser matching this one in behaviour, but producing user defined types into the parse tree on success
* `one_char`
  * `(constructor)(TargetString)`: takes a string as target
  * `.parse(StringToParse)`: parses first character in a string and matches any character in target; consumes on success
  * `.generate(GeneratorTemplate)`: returns a new parser matching this one in behaviour, but producing user defined types into the parse tree on success
* `before`
  * `(constructor)(TargetParser)`: takes another parser as target
  * `.parse(StringToParse)`: parses a string and matches target; does **not** consume on success
  * `.generate(GeneratorTemplate)`: returns a new parser matching this one in behaviour, but producing user defined types into the parse tree on success
* `filter`
  * `(constructor)(TargetParser)`: takes another parser as target
  * `.parse(StringToParse)`: parses a string and matches anything but target; consumes one character on success
  * `.generate(GeneratorTemplate)`: returns a new parser matching this one in behaviour, but producing user defined types into the parse tree on success