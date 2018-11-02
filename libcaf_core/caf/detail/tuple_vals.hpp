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

#include <tuple>

#include "caf/type_nr.hpp"
#include "caf/serializer.hpp"
#include "caf/deserializer.hpp"
#include "caf/deep_to_string.hpp"
#include "caf/make_type_erased_value.hpp"

#include "caf/detail/apply_args.hpp"
#include "caf/detail/message_data.hpp"
#include "caf/detail/safe_equal.hpp"
#include "caf/detail/stringification_inspector.hpp"
#include "caf/detail/try_serialize.hpp"
#include "caf/detail/type_list.hpp"

namespace caf {
namespace detail {

struct void_ptr_access {
  template <class T>
  void* operator()(T& x) const noexcept {
    return &x;
  }
  template <class T>
  const void* operator()(const T& x) const noexcept {
    return &x;
  }
};

struct rtti_access {
  using res_t = typename message_data::rtti_pair;
  template <class T>
  res_t operator()(const T& x) const noexcept {
    // TODO: replace this static dispatching with `constexpr if`
    //       when switching to C++17
    std::integral_constant<uint16_t, type_nr<T>::value> token;
    return get(x, token);
  }

  template <class T>
  static res_t get(const T&, std::integral_constant<uint16_t, 0>) noexcept {
    return {0, &typeid(T)};
  }

  template <class T, uint16_t N>
  static res_t get(const T&, std::integral_constant<uint16_t, N>) noexcept {
    return {N, nullptr};
  }
};

template <class Base, class... Ts>
class tuple_vals_impl : public Base {
public:
  // -- static invariants ------------------------------------------------------

  static_assert(sizeof...(Ts) > 0, "tuple_vals is not allowed to be empty");

  // -- member types -----------------------------------------------------------

  using super = message_data;

  using rtti_pair = typename message_data::rtti_pair;

  using data_type = std::tuple<Ts...>;

  // -- friend functions -------------------------------------------------------

  template <class Inspector>
  friend typename Inspector::result_type inspect(Inspector& f,
                                                 tuple_vals_impl& x) {
    return apply_args(f, get_indices(x.data_), x.data_);
  }

  tuple_vals_impl(const tuple_vals_impl&) = default;

  template <class... Us>
  tuple_vals_impl(Us&&... xs) : data_(std::forward<Us>(xs)...) {
    // nop
  }

  data_type& data() {
    return data_;
  }

  const data_type& data() const {
    return data_;
  }

  size_t size() const noexcept override {
    return sizeof...(Ts);
  }

  const void* get(size_t pos) const noexcept override {
    CAF_ASSERT(pos < size());
    void_ptr_access f;
    return apply(pos, f, data_);
  }

  void* get_mutable(size_t pos) override {
    CAF_ASSERT(pos < size());
    void_ptr_access f;
    return apply(pos, f, data_);
  }

  std::string stringify(size_t pos) const override {
    CAF_ASSERT(pos < size());
    std::string result;
    stringification_inspector f{result};
    apply(pos, f, data_);
    return result;
  }

  using Base::copy;

  type_erased_value_ptr copy(size_t pos) const override {
    CAF_ASSERT(pos < size());
    type_erased_value_factory f;
    return apply(pos, f, data_);
  }

  error load(size_t pos, deserializer& source) override {
    CAF_ASSERT(pos < size());
    return detail::apply(pos, source, data_);
  }

  uint32_t type_token() const noexcept override {
    return make_type_token<Ts...>();
  }

  rtti_pair type(size_t pos) const noexcept override {
    CAF_ASSERT(pos < size());
    rtti_access f;
    return detail::apply(pos, f, data_);
  }

  error save(size_t pos, serializer& sink) const override {
    CAF_ASSERT(pos < size());
    return detail::apply(pos, sink, const_cast<data_type&>(data_));
  }

private:
  data_type data_;
};

template <class... Ts>
class tuple_vals : public tuple_vals_impl<message_data, Ts...> {
public:
  static_assert(sizeof...(Ts) > 0, "tuple_vals is not allowed to be empty");

  using super = tuple_vals_impl<message_data, Ts...>;

  using super::super;

  using super::copy;

  tuple_vals* copy() const override {
    return new tuple_vals(*this);
  }
};

} // namespace detail
} // namespace caf
