#ifndef UTILITY_HPP_GAURD
#define UTILITY_HPP_GAURD

#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace utility {

template<typename R, typename... ArgTs>
size_t
get_address(std::function<R(ArgTs...)> f)
{
  typedef R(fnType)(ArgTs...);
  fnType** fnPointer = f.template target<fnType*>();
  return (size_t)*fnPointer;
}

template<bool E>
struct ct_require;

template<>
struct ct_require<true>
{
};

template<bool E, typename T, typename F>
struct ct_if_else;

template<typename T, typename F>
struct ct_if_else<false, T, F>
{
  typedef F type;
};

template<typename T, typename F>
struct ct_if_else<true, T, F>
{
  typedef T type;
};

namespace ct {
template<bool Expression, typename Then, typename Else>
using if_else_t = typename ct_if_else<Expression, Then, Else>::type;
}

template<size_t Index, typename... Ts>
struct ct_select;

template<typename T, typename... Ts>
struct ct_select<0, T, Ts...>
{
  typedef T type;
};

template<size_t Index, typename T, typename... Ts>
struct ct_select<Index, T, Ts...>
{
  static_assert(Index < sizeof...(Ts) + 1, "parameter pack index out of bounds");
  typedef typename ct_select<Index - 1, Ts...>::type type;
};

namespace ct {
template<size_t AtIndex, typename... TypeList>
using select_t = typename ct_select<AtIndex, TypeList...>::type;
}

template<size_t Index, size_t End, size_t ResultSize, typename ResultType, bool AtEnd, typename T, typename... Ts>
struct ct_biggest_type_assist;

template<size_t Index, size_t End, size_t ResultSize, typename ResultType, typename T, typename... Ts>
struct ct_biggest_type_assist<Index, End, ResultSize, ResultType, true, T, Ts...>
{
  static constexpr size_t this_size = sizeof(T);
  static constexpr size_t size = (this_size > ResultSize) ? this_size : ResultSize;
  typedef typename ct_if_else<(this_size > ResultSize), T, ResultType>::type type;
};

template<size_t Index, size_t End, size_t ResultSize, typename ResultType, typename T, typename... Ts>
struct ct_biggest_type_assist<Index, End, ResultSize, ResultType, false, T, Ts...>
{
  static constexpr size_t this_size = sizeof(T);
  static constexpr size_t size =
    (this_size > ct_biggest_type_assist<Index + 1, End, ResultSize, ResultType, (Index + 1 == End), Ts...>::this_size)
      ?
      /*then*/ ct_biggest_type_assist<Index + 1, End, this_size, T, (Index + 1 == End), Ts...>::size
      :
      /*else*/ ct_biggest_type_assist<Index + 1, End, ResultSize, ResultType, (Index + 1 == End), Ts...>::size;
  typedef typename ct_if_else<
    (this_size > ct_biggest_type_assist<Index + 1, End, ResultSize, ResultType, (Index + 1 == End), Ts...>::this_size),
    /*then*/
    typename ct_biggest_type_assist<Index + 1, End, this_size, T, (Index + 1 == End), Ts...>::type,
    /*else*/ typename ct_biggest_type_assist<Index + 1, End, ResultSize, ResultType, (Index + 1 == End), Ts...>::type>::
    type type;
};

template<typename... Ts>
struct ct_biggest_type;

template<typename T>
struct ct_biggest_type<T>
{
  constexpr static size_t size = sizeof(T);
  typedef T type;
};

template<typename T, typename... Ts>
struct ct_biggest_type<T, Ts...>
{
  constexpr static size_t size =
    ct_biggest_type_assist<0, sizeof...(Ts), 0, void, (sizeof...(Ts) == 0), T, Ts...>::size;
  typedef typename ct_biggest_type_assist<0, sizeof...(Ts), 0, void, (sizeof...(Ts) == 0), T, Ts...>::type type;
};

namespace ct {
template<typename... TypeList>
constexpr size_t biggest_type_v = ct_biggest_type<TypeList...>::size;
template<typename... TypeList>
using biggest_type_t = typename ct_biggest_type<TypeList...>::type;
}

template<bool Matches, size_t Index, typename F, typename T, typename... Ts>
struct ct_index_of_assist;

template<size_t Index, typename F, typename T, typename... Ts>
struct ct_index_of_assist<true, Index, F, T, Ts...>
{
  static constexpr size_t index = Index;
};

template<size_t Index, typename F, typename T, typename... Ts>
struct ct_index_of_assist<false, Index, F, T, Ts...>
{
  static constexpr bool matches_next = std::is_same<F, typename ct_select<Index + 1, T, Ts...>::type>::value;
  static constexpr size_t index = ct_index_of_assist<matches_next, Index + 1, F, T, Ts...>::index;
};

template<size_t Index, typename F, typename T>
struct ct_index_of_assist<true, Index, F, T>
{
  static constexpr size_t index = Index;
};

template<size_t Index, typename F, typename T>
struct ct_index_of_assist<false, Index, F, T>
{
  static_assert(std::is_same<F, T>::value, "no match found in parameter pack");
};

template<typename F, typename T, typename... Ts>
struct ct_index_of
{
  static constexpr bool matches_next = std::is_same<F, T>::value;
  static constexpr size_t index = ct_index_of_assist<matches_next, 0, F, T, Ts...>::index;
};

namespace ct {
template<typename TypeToFind, typename... TypeList>
using index_of_v = typename ct_index_of<TypeToFind, TypeList...>::index;
}

template<size_t... Indices>
struct ct_index_range
{
  enum
  {
    size = sizeof...(Indices)
  };
};

template<size_t Begin,
         size_t End,
         size_t Position = 0,
         bool AddIndex = Begin == 0,
         bool AtEnd = Position == End,
         size_t... Result>
struct ct_make_index_range;

template<size_t Begin, size_t End, size_t Position, size_t... Result>
struct ct_make_index_range<Begin, End, Position, /*AddIndex=*/false, /*AtEnd=*/true, Result...>
{
  typedef ct_index_range<Result...> type;
};

template<size_t Begin, size_t End, size_t Position, size_t... Result>
struct ct_make_index_range<Begin, End, Position, /*AddIndex=*/true, /*AtEnd=*/true, Result...>
{
  typedef ct_index_range<Result..., Position> type;
};

template<size_t Begin, size_t End, size_t Position, size_t... Result>
struct ct_make_index_range<Begin, End, Position, /*AddIndex=*/false, /*AtEnd=*/false, Result...>
{
  static constexpr size_t add_next_index = Position + 1 >= Begin && Position + 1 <= End;
  static constexpr size_t next_is_last = Position + 1 == End;
  typedef typename ct_make_index_range<Begin, End, Position + 1, add_next_index, next_is_last, Result...>::type type;
};

template<size_t Begin, size_t End, size_t Position, size_t... Result>
struct ct_make_index_range<Begin, End, Position, /*AddIndex=*/true, /*AtEnd=*/false, Result...>
{
  static constexpr size_t add_next_index = Position + 1 >= Begin && Position + 1 <= End;
  static constexpr size_t next_is_last = Position + 1 == End;
  typedef
    typename ct_make_index_range<Begin, End, Position + 1, add_next_index, next_is_last, Result..., Position>::type
      type;
};

template<size_t Begin, size_t End, size_t... Result>
struct ct_make_index_range<Begin,
                           End,
                           /*Position=*/0,
                           /*AddIndex=*/false,
                           /*AtEnd=*/false,
                           Result...>
{
  static_assert(Begin < End, "make_index_range: begin index is greater than end index");
  static constexpr size_t add_next_index = 1 >= Begin && 1 <= End;
  static constexpr size_t next_is_last = 1 == End;
  typedef typename ct_make_index_range<Begin, End, 1, add_next_index, next_is_last, Result...>::type type;
};

template<size_t Begin, size_t End, size_t... Result>
struct ct_make_index_range<Begin,
                           End,
                           /*Position=*/0,
                           /*AddIndex=*/true,
                           /*AtEnd=*/false,
                           Result...>
{
  static_assert(Begin < End, "make_index_range: begin index is greater than end index");
  static constexpr size_t add_next_index = 1 >= Begin && 1 <= End;
  static constexpr size_t next_is_last = 1 == End;
  typedef typename ct_make_index_range<Begin, End, 1, add_next_index, next_is_last, 0>::type type;
};

namespace ct {
template<size_t... Indices>
using index_range = ct_index_range<Indices...>;
template<size_t BeginIndex, size_t EndIndex>
using make_index_range_t = typename ct_make_index_range<BeginIndex, EndIndex>::type;
}

template<typename F, template<class...> typename T, size_t... Indices, class... TL>
decltype(auto)
apply_impl(F&& functional, T<TL...>&& tuple, ct_index_range<Indices...>&& indices)
{
  return functional(std::get<Indices>(tuple)...);
}

template<typename F, template<class...> typename T, class T0, class... TL>
decltype(auto)
apply(F functional, T<T0, TL...> tuple)
{
  return apply_impl(std::forward<F>(functional),
                    std::forward<T<T0, TL...>>(tuple),
                    typename ct_make_index_range<0, sizeof...(TL)>::type());
}

template<typename F, template<class...> typename T>
decltype(auto)
apply(F functional, T<> tuple)
{
  return std::forward<F>(functional)();
}
}

#endif