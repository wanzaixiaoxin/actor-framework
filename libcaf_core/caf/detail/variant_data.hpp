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

#include <stdexcept>
#include <type_traits>

#include "caf/unit.hpp"
#include "caf/none.hpp"

#define CAF_VARIANT_DATA_CONCAT(x, y) x ## y

#define CAF_VARIANT_DATA_GETTER(pos)                                           \
  CAF_VARIANT_DATA_CONCAT(T, pos) & get(std::integral_constant<int, pos>) {    \
    return CAF_VARIANT_DATA_CONCAT(v, pos);                                    \
  }                                                                            \
  const CAF_VARIANT_DATA_CONCAT(T, pos) &                                      \
  get(std::integral_constant<int, pos>) const {                                \
    return CAF_VARIANT_DATA_CONCAT(v, pos);                                    \
  }

namespace caf {
namespace detail {

template <class T0,           class T1  = unit_t, class T2  = unit_t,
          class T3  = unit_t, class T4  = unit_t, class T5  = unit_t,
          class T6  = unit_t, class T7  = unit_t, class T8  = unit_t,
          class T9  = unit_t, class T10 = unit_t, class T11 = unit_t,
          class T12 = unit_t, class T13 = unit_t, class T14 = unit_t,
          class T15 = unit_t, class T16 = unit_t, class T17 = unit_t,
          class T18 = unit_t, class T19 = unit_t, class T20 = unit_t>
struct variant_data {
  using types = type_list<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12,
                          T13, T14, T15, T16, T17, T18, T19, T20>;

  union {
    T0  v0;  T1  v1;  T2  v2;  T3  v3;  T4  v4;  T5  v5;  T6   v6;
    T7  v7;  T8  v8;  T9  v9;  T10 v10; T11 v11; T12 v12; T13 v13;
    T14 v14; T15 v15; T16 v16; T17 v17; T18 v18; T19 v19; T20 v20;
  };

  variant_data() {
    // nop
  }

  ~variant_data() {
    // nop
  }

  CAF_VARIANT_DATA_GETTER(0)
  CAF_VARIANT_DATA_GETTER(1)
  CAF_VARIANT_DATA_GETTER(2)
  CAF_VARIANT_DATA_GETTER(3)
  CAF_VARIANT_DATA_GETTER(4)
  CAF_VARIANT_DATA_GETTER(5)
  CAF_VARIANT_DATA_GETTER(6)
  CAF_VARIANT_DATA_GETTER(7)
  CAF_VARIANT_DATA_GETTER(8)
  CAF_VARIANT_DATA_GETTER(9)
  CAF_VARIANT_DATA_GETTER(10)
  CAF_VARIANT_DATA_GETTER(11)
  CAF_VARIANT_DATA_GETTER(12)
  CAF_VARIANT_DATA_GETTER(13)
  CAF_VARIANT_DATA_GETTER(14)
  CAF_VARIANT_DATA_GETTER(15)
  CAF_VARIANT_DATA_GETTER(16)
  CAF_VARIANT_DATA_GETTER(17)
  CAF_VARIANT_DATA_GETTER(18)
  CAF_VARIANT_DATA_GETTER(19)
  CAF_VARIANT_DATA_GETTER(20)

  template <size_t N>
  friend typename tl_at<types, N>::type& get(variant_data& x) {
    std::integral_constant<int, N> token;
    return x.get(token);
  }

  template <size_t N>
  friend const typename tl_at<types, N>::type& get(const variant_data& x) {
    std::integral_constant<int, N> token;
    return x.get(token);
  }
};

struct variant_data_destructor {
  using result_type = void;

  template <class T>
  void operator()(T& storage) const {
    storage.~T();
  }
};

} // namespace detail
} // namespace caf

#undef CAF_VARIANT_DATA_CONCAT
#undef CAF_VARIANT_DATA_GETTER
