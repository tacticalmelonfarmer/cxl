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
  return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22);
}
if constexpr (arity == 23) {
  auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23] = pod;
  return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23);
}
if constexpr (arity == 24) {
  auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24] = pod;
  return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24);
}
if constexpr (arity == 25) {
  auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25] = pod;
  return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25);
}
if constexpr (arity == 26) {
  auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26] = pod;
  return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26);
}
if constexpr (arity == 27) {
  auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27] = pod;
  return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27);
}
if constexpr (arity == 28) {
  auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28] = pod;
  return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28);
}
if constexpr (arity == 29) {
  auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29] = pod;
  return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29);
}
if constexpr (arity == 30) {
  auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30] = pod;
  return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30);
}
if constexpr (arity == 31) {
  auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31] = pod;
  return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31);
}
if constexpr (arity == 32) {
  auto&& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32] = pod;
  return make_tuple<T&&>(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32);
}
