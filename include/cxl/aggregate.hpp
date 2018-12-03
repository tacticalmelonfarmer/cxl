#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace cxl {

using std::size_t;

inline namespace detail {

template<typename T>
struct size_greater
{
  static constexpr auto size = sizeof(T);
};

template<typename T, typename U>
constexpr auto
operator>(const size_greater<T>, const size_greater<U>)
{
  (sizeof(T) >= sizeof(U)) ? size_greater<T>{} : size_greater<U>{};
}

template<typename... Types>
struct max_size
{
  static constexpr auto value = (size_greater<Types>{} > ...).size;
};

struct unknown_t
{
  template<typename T>
  operator T() const;
};

template<size_t N = 0>
using unknown = const unknown_t;

template<typename T, size_t... I>
constexpr auto
is_brace_constructible_(std::index_sequence<I...>, T*) -> decltype(T{ unknown<I>{}... }, std::true_type{})
{
  return {};
}

template<size_t... I>
constexpr std::false_type
is_brace_constructible_(std::index_sequence<I...>, ...)
{
  return {};
}

template<typename T, size_t N>
constexpr auto
is_brace_constructible() -> decltype(is_brace_constructible_(std::make_index_sequence<N>{}, static_cast<T*>(nullptr)))
{
  return {};
}

template<size_t N>
using size = std::integral_constant<size_t, N>;

template<typename A, typename... Ts>
constexpr auto
make_tuple(Ts&&... Values)
{
  if constexpr (std::is_rvalue_reference_v<A>)
    return std::make_tuple(std::forward<Ts>(Values)...);
  else if constexpr (std::is_lvalue_reference_v<A>)
    return std::forward_as_tuple(std::forward<Ts>(Values)...);
}
}

template<typename T, size_t Count = 0>
constexpr auto
aggregate_arity()
{
  if constexpr (std::is_aggregate<T>{} && is_brace_constructible<T, Count>() && !is_brace_constructible<T, Count + 1>())
    return size<Count>{};
  else
    return aggregate_arity<T, Count + 1>();
}

template<typename T>
constexpr auto
destructure(T&& pod)
{
  constexpr auto arity = aggregate_arity<std::remove_reference_t<T>>();
  if constexpr (arity == 0)
    return std::tuple<>{};

  if constexpr (arity == 1) {
    auto&& [v1] = pod;
    return make_tuple<T&&>(v1);
  }
  if constexpr (arity == 2) {
    auto&& [v1, v2] = pod;
    return make_tuple<T&&>(v1, v2);
  }
  if constexpr (arity == 3) {
    auto&& [v1, v2, v3] = pod;
    return make_tuple<T&&>(v1, v2, v3);
  }
  if constexpr (arity == 4) {
    auto&& [v1, v2, v3, v4] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4);
  }
  if constexpr (arity == 5) {
    auto&& [v1, v2, v3, v4, v5] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4, v5);
  }
  if constexpr (arity == 6) {
    auto&& [v1, v2, v3, v4, v5, v6] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4, v5, v6);
  }
  if constexpr (arity == 7) {
    auto&& [v1, v2, v3, v4, v5, v6, v7] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7);
  }
  if constexpr (arity == 8) {
    auto&& [v1, v2, v3, v4, v5, v6, v7, v8] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8);
  }
  if constexpr (arity == 9) {
    auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9);
  }
  if constexpr (arity == 10) {
    auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10);
  }
  if constexpr (arity == 11) {
    auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11);
  }
  if constexpr (arity == 12) {
    auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12);
  }
  if constexpr (arity == 13) {
    auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13);
  }
  if constexpr (arity == 14) {
    auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14);
  }
  if constexpr (arity == 15) {
    auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15);
  }
  if constexpr (arity == 16) {
    auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16);
  }
  if constexpr (arity == 17) {
    auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17);
  }
  if constexpr (arity == 18) {
    auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18);
  }
  if constexpr (arity == 19) {
    auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19);
  }
  if constexpr (arity == 20) {
    auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20);
  }
  if constexpr (arity == 21) {
    auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21] = pod;
    return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21);
  }
  if constexpr (arity == 22) {
    auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22] = pod;
    return make_tuple<T&&>(
      v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22);
  }
  if constexpr (arity == 23) {
    auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23] = pod;
    return make_tuple<T&&>(
      v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23);
  }
  if constexpr (arity == 24) {
    auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24] =
      pod;
    return make_tuple<T&&>(
      v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24);
  }
}
}