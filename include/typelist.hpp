#pragma once

#include "utility.hpp"

namespace utility
{

template <typename ... Ts>
struct typelist { enum{ size = sizeof...(Ts) }; };

template <template <typename...> typename ApplyTo, typename TL>
struct tl_apply;

template <template <typename...> typename ApplyTo, template <typename...> typename TL, typename ... TL_deduce>
struct tl_apply<ApplyTo, TL<TL_deduce...>>
{
	typedef ApplyTo<TL_deduce...> type;
};

template <template <typename...> typename ApplyTo, typename TL, typename ... Post>
struct tl_apply_before;

template <template <typename...> typename ApplyTo, template <typename...> typename TL, typename ... TL_deduce, typename ... Post>
struct tl_apply_before<ApplyTo, TL<TL_deduce...>, Post...>
{
	typedef ApplyTo<TL_deduce..., Post...> type;
};

template <template <typename...> typename ApplyTo, typename TL, typename ... Pre>
struct tl_apply_after;

template <template <typename...> typename ApplyTo, template <typename...> typename TL, typename ... TL_deduce, typename ... Pre>
struct tl_apply_after<ApplyTo, TL<TL_deduce...>, Pre...>
{
	typedef ApplyTo<Pre..., TL_deduce...> type;
};

template <typename F, typename TL>
struct tl_index_of;

template <typename F, template <typename...> typename TL, typename T, typename ... Ts>
struct tl_index_of<F, TL<T, Ts...>>
{
	static constexpr size_t index = ct_index_of<F, T, Ts...>::index;
};

template <size_t Index, typename TL>
struct tl_type_at;

template <size_t Index, template <typename...> typename TL, typename ... TL_deduce>
struct tl_type_at<Index, TL<TL_deduce...>>
{
	static_assert(Index < TL<TL_deduce...>::size, "typelist index out of bounds");
	typedef typename ct_select<Index, TL_deduce...>::type type;
};

template <typename TL, size_t Index = 0, typename TypeOfResult = typename tl_type_at<0, TL>::type, size_t SizeResult = sizeof(typename tl_type_at<0, TL>::type), bool IsMin = true, bool AtEnd = (Index == TL::size - 1)>
struct tl_min_size;

template <typename TL, size_t Index, typename TypeOfResult, size_t SizeResult, bool IsMin>
struct tl_min_size<TL, Index, TypeOfResult, SizeResult, IsMin, true>
{
	typedef TypeOfResult type;
	static constexpr size_t size = SizeResult;
	static constexpr size_t index = Index;
};

template <typename TL, size_t Index, typename TypeOfResult, size_t SizeResult>
struct tl_min_size<TL, Index, TypeOfResult, SizeResult, true, false>
{
	typedef typename tl_type_at<Index, TL>::type current_type;
	static constexpr size_t current_size = sizeof(typename tl_type_at<Index, TL>::type);
	static constexpr size_t next_size = sizeof(typename tl_type_at<Index + 1, TL>::type);
	static constexpr size_t size = tl_min_size<TL, Index + 1, current_type, current_size, (next_size < current_size), (Index + 1 == TL::size - 1)>::size;
};

template <typename TL, size_t Index, typename TypeOfResult, size_t SizeResult>
struct tl_min_size<TL, Index, TypeOfResult, SizeResult, false, false>
{
	typedef typename tl_type_at<Index, TL>::type current_type;
	static constexpr size_t current_size = sizeof(typename tl_type_at<Index, TL>::type);
	static constexpr size_t next_size = sizeof(typename tl_type_at<Index + 1, TL>::type);
	static constexpr size_t size = tl_min_size<TL, Index + 1, TypeOfResult, SizeResult, (next_size < current_size), (Index + 1 == TL::size - 1)>::size;
};

template <typename TL, size_t Index = 0, typename TypeOfResult = typename tl_type_at<0, TL>::type, size_t SizeResult = sizeof(typename tl_type_at<0, TL>::type), bool IsMax = true, bool AtEnd = (Index == TL::size - 1)>
struct tl_max_size;

template <typename TL, size_t Index, typename TypeOfResult, size_t SizeResult, bool IsMax>
struct tl_max_size<TL, Index, TypeOfResult, SizeResult, IsMax, true>
{
	typedef TypeOfResult type;
	static constexpr size_t size = SizeResult;
	static constexpr size_t index = Index;
};

template <typename TL, size_t Index, typename TypeOfResult, size_t SizeResult>
struct tl_max_size<TL, Index, TypeOfResult, SizeResult, true, false>
{
	typedef typename tl_type_at<Index, TL>::type current_type;
	static constexpr size_t current_size = sizeof(typename tl_type_at<Index, TL>::type);
	static constexpr size_t next_size = sizeof(typename tl_type_at<Index + 1, TL>::type);
	static constexpr size_t size = tl_max_size<TL, Index + 1, current_type, current_size, (next_size > current_size), (Index + 1 == TL::size - 1)>::size;
};

template <typename TL, size_t Index, typename TypeOfResult, size_t SizeResult>
struct tl_max_size<TL, Index, TypeOfResult, SizeResult, false, false>
{
	typedef typename tl_type_at<Index, TL>::type current_type;
	static constexpr size_t current_size = sizeof(typename tl_type_at<Index, TL>::type);
	static constexpr size_t next_size = sizeof(typename tl_type_at<Index + 1, TL>::type);
	static constexpr size_t size = tl_max_size<TL, Index + 1, TypeOfResult, SizeResult, (next_size > current_size), (Index + 1 == TL::size - 1)>::size;
};

template <typename TL, typename ... NewTypes>
struct tl_push_back;

template <template <typename...> typename TL, typename ... TL_deduce, typename ... NewTypes>
struct tl_push_back<TL<TL_deduce...>, NewTypes...>
{
	typedef typename tl_apply_before<typelist, TL<TL_deduce...>, NewTypes...>::type type;
};

template <typename TL, typename ... NewTypes>
struct tl_push_front;

template <template <typename...> typename TL, typename ... TL_deduce, typename ... NewTypes>
struct tl_push_front<TL<TL_deduce...>, NewTypes...>
{
	typedef typename tl_apply_after<typelist, TL<TL_deduce...>, NewTypes...>::type type;
};

template <typename TL>
struct tl_back
{
	typedef typename tl_type_at<TL::size - 1, TL>::type type;
};

template <typename TL>
struct tl_front
{
	typedef typename tl_type_at<0, TL>::type type;
};

template <typename TL, size_t Begin, size_t End, size_t Position = 0, bool AtEnd = false, bool AddType = (Begin == 0), typename ... Result>
struct tl_subrange;

template <typename TL, size_t Begin, size_t End, size_t Position, typename ... Result>
struct tl_subrange<TL, Begin, End, Position, true, true, Result...> // end
{
	typedef typelist<Result..., typename tl_type_at<Position, TL>::type> type;
};

template <typename TL, size_t Begin, size_t End, size_t Position, typename ... Result>
struct tl_subrange<TL, Begin, End, Position, true, false, Result...> // end
{
	typedef typelist<Result...> type;
};

template <typename TL, size_t Begin, size_t End, size_t Position, typename ... Result>
struct tl_subrange<TL, Begin, End, Position, false, true, Result...>
{
	static constexpr bool add_next_type = Position + 1 >= Begin && Position + 1 <= End;
	static constexpr bool next_is_last = Position + 1 == End;
	typedef typename tl_subrange<TL, Begin, End, Position + 1, next_is_last, add_next_type, Result..., typename tl_type_at<Position, TL>::type>::type type;
};

template <typename TL, size_t Begin, size_t End, size_t Position, typename ... Result>
struct tl_subrange<TL, Begin, End, Position, false, false, Result...>
{
	static constexpr bool add_next_type = Position + 1 >= Begin && Position + 1 <= End;
	static constexpr bool next_is_last = Position + 1 == End;
	typedef typename tl_subrange<TL, Begin, End, Position + 1, next_is_last, add_next_type, Result...>::type type;
};

template <typename TL, size_t Begin, size_t End, typename ... Result>
struct tl_subrange<TL, Begin, End, 0, false, true, Result...> // entry add type
{
	static constexpr bool Position = 0;
	static constexpr bool add_next_type = Begin <= 1;
	static constexpr bool next_is_last = End == 1;
	typedef typename tl_subrange<TL, Begin, End, Position + 1, next_is_last, add_next_type, typename tl_type_at<Position, TL>::type>::type type;
};

template <typename TL, size_t Begin, size_t End, typename ... Result>
struct tl_subrange<TL, Begin, End, 0, false, false, Result...> // entry without adding type
{
	static constexpr bool Position = 0;
	static constexpr bool add_next_type = Begin == 1;
	static constexpr bool next_is_last = End == 1;
	typedef typename tl_subrange<TL, Begin, End, Position + 1, next_is_last, add_next_type>::type type;
};

template <typename TL1, typename TL2>
struct tl_join;

template <template <typename> typename TL1, typename ... TL1_deduce, template <typename> typename TL2, typename ... TL2_deduce>
struct tl_join<TL1<TL1_deduce...>, TL2<TL2_deduce...>>
{
	typedef typelist<TL1_deduce..., TL2_deduce...> type;
};

template <typename TLDest, size_t Index, typename TLSource>
struct tl_insert
{
	typedef typename tl_subrange<TLDest, 0, Index - 1>::type first_partition;
	typedef typename tl_subrange<TLDest, Index, TLDest::size - 1>::type last_partition;
	typedef typename tl_join<first_partition, typename tl_join<TLSource, last_partition>::type>::type type;
};

template <typename TLDest, size_t Index, typename ... NewTypes>
struct tl_insert_t
{
	typedef typename tl_subrange<TLDest, 0, Index - 1>::type first_partition;
	typedef typename tl_subrange<TLDest, Index, TLDest::size - 1>::type last_partition;
	typedef typename tl_join<first_partition, typename tl_push_front<last_partition, NewTypes...>::type>::type type;
};

template <typename TL, size_t Index>
struct tl_remove
{
	typedef typename tl_subrange<TL, 0, Index - 1>::type first_partition;
	typedef typename tl_subrange<TL, Index + 1, TL::size - 1>::type last_partition;
	typedef typename tl_join<first_partition, last_partition>::type type;
};

template <typename TL, size_t Begin, size_t End>
struct tl_remove_subrange
{
	typedef typename tl_subrange<TL, 0, Begin - 1>::type first_partition;
	typedef typename tl_subrange<TL, End + 1, TL::size - 1>::type last_partition;
	typedef typename tl_join<first_partition, last_partition>::type type;
};

template <typename TL, size_t Size = 1>
struct tl_pop_back
{
	typedef typename tl_subrange<TL, 0, TL::size - 1 - Size>::type type;
};

template <typename TL, size_t Size = 1>
struct tl_pop_front
{
	typedef typename tl_subrange<TL, Size,  TL::size - 1>::type type;
};

template <template <typename> typename ApplyTo, typename TL>
struct tl_apply_callsign;

template <template <typename> typename ApplyTo, template <typename...> typename TL, typename R, typename ... Ps>
struct tl_apply_callsign<ApplyTo, TL<R, Ps...>>
{
	typedef ApplyTo<R(Ps...)> type;
};

template <typename TL>
struct tl_callsign; //< callsign means function signature

template <template <typename...> typename TL, typename R, typename ... Ps>
struct tl_callsign<TL<R, Ps...>>
{
	typedef R return_type;
	typedef typelist<Ps...> param_types;
	typedef R (*pointer_type)(Ps...);
};

template <typename F, typename TL>
struct tl_has_type;

template <typename F, template <typename> typename TL, typename T>
struct tl_has_type<F, TL<T>>
{
	static constexpr bool value = std::is_same<F, T>::value && sizeof(F) == sizeof(T);
};

template <typename F, template <typename...> typename TL, typename T, typename ... Ts>
struct tl_has_type<F, TL<T, Ts...>>
{
	static constexpr bool value = (std::is_same<F, T>::value && sizeof(F) == sizeof(T)) || tl_has_type<F, TL<Ts...>>::value;
};

template <typename F, typename TL>
struct tl_has_conversion;

template <typename F, template <typename> typename TL, typename T>
struct tl_has_conversion<F, TL<T>>
{
	static constexpr bool value = std::is_convertible<F, T>::value;
};

template <typename F, template <typename...> typename TL, typename T, typename ... Ts>
struct tl_has_conversion<F, TL<T, Ts...>>
{
	static constexpr bool value = std::is_convertible<F, T>::value || tl_has_conversion<F, TL<Ts...>>::value;
};

template <typename F, typename TL>
struct tl_find_conversion;

template <typename F, template <typename> typename TL, typename T>
struct tl_find_conversion<F, TL<T>>
{
	static constexpr bool valid_conversion = std::is_convertible<F, T>::value;
	static_assert(valid_conversion, "could not find suitable a suitable conversion for F in typelist");
	typedef T type;
};

template <typename F, template <typename...> typename TL, typename T, typename ... Ts>
struct tl_find_conversion<F, TL<T, Ts...>>
{
	static constexpr bool valid_conversion = std::is_convertible<F, T>::value;
	typedef typename ct_if_else</*If*/valid_conversion, /*Then*/T, /*Else*/typename tl_find_conversion<F, TL<Ts...>>::type>::type type;
};

}