#pragma once
#include <cstddef>
#include <type_traits>
#include <utility>
#include <functional>
#include <tuple>

namespace utility
{

template <typename T>
using uncvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename T>
inline const uncvref_t<T> lit(T v)
{ return static_cast<const uncvref_t<T>>(v); }

template <typename R, typename ... ArgTs>
size_t get_address(std::function<R(ArgTs...)> f) {
    typedef R (fnType)(ArgTs...);
    fnType ** fnPointer = f.template target<fnType*>();
    return (size_t) *fnPointer;
}

template <bool E>
struct ct_require;

template <>
struct ct_require<true>
{};

template <bool E, typename T, typename F>
struct ct_if_else;

template <typename T, typename F>
struct ct_if_else<false, T, F>
{
	typedef F type;
};

template <typename T, typename F>
struct ct_if_else<true, T, F>
{
	typedef T type;
};

template <size_t Index, typename ... Ts>
struct ct_select;

template <typename T, typename ... Ts>
struct ct_select<0, T, Ts...>
{
	typedef T type;
};

template <size_t Index, typename T, typename ... Ts>
struct ct_select<Index, T, Ts...>
{
	static_assert(Index < sizeof...(Ts) + 1, "parameter pack index out of bounds");
	typedef typename ct_select<Index - 1, Ts...>::type type;
};

template <bool Matches, size_t Index, typename F, typename T, typename ... Ts>
struct ct_index_of_assist;

template <size_t Index, typename F, typename T, typename ... Ts>
struct ct_index_of_assist<true, Index, F, T, Ts...>
{
	static constexpr size_t index = Index;
};

template <size_t Index, typename F, typename T, typename ... Ts>
struct ct_index_of_assist<false, Index, F, T, Ts...>
{
	static constexpr bool matches_next = std::is_same<F, typename ct_select<Index + 1, T, Ts...>::type>::value;
	static constexpr size_t index = ct_index_of_assist<matches_next, Index + 1, F, T, Ts...>::index;
};

template <size_t Index, typename F, typename T>
struct ct_index_of_assist<true, Index, F, T>
{
	static constexpr size_t index = Index;
};

template <size_t Index, typename F, typename T>
struct ct_index_of_assist<false, Index, F, T>
{
	static_assert(std::is_same<F, T>::value, "no match found in parameter pack");
};

template <typename F, typename T, typename ... Ts>
struct ct_index_of
{
	static constexpr bool matches_next = std::is_same<F, T>::value;
	static constexpr size_t index = ct_index_of_assist<matches_next, 0, F, T, Ts...>::index;
};

template <size_t ... Indices>
struct ct_index_range { enum{ size = sizeof...(Indices) }; };

template <size_t Begin, size_t End, size_t Position = 0, bool AddIndex = Begin == 0, bool AtEnd = Position == End, size_t ... Result>
struct ct_make_index_range;

template <size_t Begin, size_t End, size_t Position, size_t ... Result>
struct ct_make_index_range<Begin, End, Position, /*AddIndex=*/false, /*AtEnd=*/true, Result...>
{
	typedef ct_index_range<Result...> type;
};

template <size_t Begin, size_t End, size_t Position, size_t ... Result>
struct ct_make_index_range<Begin, End, Position, /*AddIndex=*/true, /*AtEnd=*/true, Result...>
{
	typedef ct_index_range<Result..., Position> type;
};

template <size_t Begin, size_t End, size_t Position, size_t ... Result>
struct ct_make_index_range<Begin, End, Position, /*AddIndex=*/false, /*AtEnd=*/false, Result...>
{
	static constexpr size_t add_next_index = Position + 1 >= Begin && Position + 1 <= End;
	static constexpr size_t next_is_last = Position + 1 == End;
	typedef typename ct_make_index_range<Begin, End, Position + 1, add_next_index, next_is_last, Result...>::type type;
};

template <size_t Begin, size_t End, size_t Position, size_t ... Result>
struct ct_make_index_range<Begin, End, Position, /*AddIndex=*/true, /*AtEnd=*/false, Result...>
{
	static constexpr size_t add_next_index = Position + 1 >= Begin && Position + 1 <= End;
	static constexpr size_t next_is_last = Position + 1 == End;
	typedef typename ct_make_index_range<Begin, End, Position + 1, add_next_index, next_is_last, Result..., Position>::type type;
};

template <size_t Begin, size_t End, size_t ... Result>
struct ct_make_index_range<Begin, End, /*Position=*/0, /*AddIndex=*/false, /*AtEnd=*/false, Result...>
{
	static constexpr size_t add_next_index = 1 >= Begin && 1 <= End;
	static constexpr size_t next_is_last = 1 == End;
	typedef typename ct_make_index_range<Begin, End, 1, add_next_index, next_is_last, Result...>::type type;
};

template <size_t Begin, size_t End, size_t ... Result>
struct ct_make_index_range<Begin, End, /*Position=*/0, /*AddIndex=*/true, /*AtEnd=*/false, Result...>
{
	static constexpr size_t add_next_index = 1 >= Begin && 1 <= End;
	static constexpr size_t next_is_last = 1 == End;
	typedef typename ct_make_index_range<Begin, End, 1, add_next_index, next_is_last, 0>::type type;
};

}
