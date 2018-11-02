/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright 2011-2018 Dominik Charousset                                     *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#pragma once

#include <utility>

#include "caf/detail/int_list.hpp"
#include "caf/detail/type_list.hpp"

namespace caf {
namespace detail {

// this utterly useless function works around a bug in Clang that causes
// the compiler to reject the trailing return type of apply_args because
// "get" is not defined (it's found during ADL)
template<long Pos, class... Ts>
typename tl_at<type_list<Ts...>, Pos>::type get(const type_list<Ts...>&);

template <class F, long... Is, class Tuple>
auto apply_args(F& f, detail::int_list<Is...>, Tuple& tup)
-> decltype(f(get<Is>(tup)...)) {
  return f(get<Is>(tup)...);
}

template <class F, long... Is, class Tuple>
auto apply_moved_args(F& f, detail::int_list<Is...>, Tuple& tup)
-> decltype(f(std::move(get<Is>(tup))...)) {
  return f(std::move(get<Is>(tup))...);
}

template <class F, class Tuple, class... Ts>
auto apply_args_prefixed(F& f, detail::int_list<>, Tuple&, Ts&&... xs)
-> decltype(f(std::forward<Ts>(xs)...)) {
  return f(std::forward<Ts>(xs)...);
}

template <class F, long... Is, class Tuple, class... Ts>
auto apply_args_prefixed(F& f, detail::int_list<Is...>, Tuple& tup, Ts&&... xs)
-> decltype(f(std::forward<Ts>(xs)..., get<Is>(tup)...)) {
  return f(std::forward<Ts>(xs)..., get<Is>(tup)...);
}

template <class F, class Tuple, class... Ts>
auto apply_moved_args_prefixed(F& f, detail::int_list<>, Tuple&, Ts&&... xs)
-> decltype(f(std::forward<Ts>(xs)...)) {
  return f(std::forward<Ts>(xs)...);
}

template <class F, long... Is, class Tuple, class... Ts>
auto apply_moved_args_prefixed(F& f, detail::int_list<Is...>, Tuple& tup, Ts&&... xs)
-> decltype(f(std::forward<Ts>(xs)..., std::move(get<Is>(tup))...)) {
  return f(std::forward<Ts>(xs)..., std::move(get<Is>(tup))...);
}

template <class F, long... Is, class Tuple, class... Ts>
auto apply_args_suffxied(F& f, detail::int_list<Is...>, Tuple& tup, Ts&&... xs)
-> decltype(f(get<Is>(tup)..., std::forward<Ts>(xs)...)) {
  return f(get<Is>(tup)..., std::forward<Ts>(xs)...);
}

template <class Res, size_t Pos, size_t N>
struct apply_helper {
  template <class F, class Tuple>
  static Res apply(size_t pos, F& f, Tuple& tup) {
    return pos == Pos ? f(get<Pos>(tup))
                      : apply_helper<Res, Pos + 1, N>::apply(pos, f, tup);
  }
};

template <class Res, size_t N>
struct apply_helper<Res, N, N> {
  template <class F, class Tuple>
  static Res apply(size_t, F& f, Tuple& tup) {
    return f(get<N>(tup));
  }
};

template <class F, class Tuple>
auto apply(size_t pos, F& f, Tuple& xs) -> decltype(f(get<0>(xs))) {
  using res_t = decltype(f(get<0>(xs)));
  static constexpr size_t tuple_size = std::tuple_size<Tuple>::value;
  static_assert(tuple_size > 0, "cannot apply elements from an empty tuple");
  return apply_helper<res_t, 0, tuple_size - 1>::apply(pos, f, xs);
}

} // namespace detail
} // namespace caf

