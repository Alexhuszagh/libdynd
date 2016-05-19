//
// Copyright (C) 2011-16 DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#pragma once

#include <dynd/array.hpp>

namespace dynd {
namespace nd {

  /**
   * General version of range, with raw pointers to the values. Returns
   * a one-dimensional array with the values {beginval, beginval + stepval, ...,
   * beginval + (k-1) * stepval} where the next value in the sequence would hit
   * or cross endval.
   */
  DYND_API nd::array old_range(const ndt::type &scalar_tp, const void *beginval, const void *endval,
                               const void *stepval);

  /**
   * Version of range templated for C++ scalar types.
   */
  template <class T>
  typename std::enable_if<is_dynd_scalar<T>::value, nd::array>::type old_range(T beginval, T endval, T stepval = T(1)) {
    return old_range(ndt::make_type<T>(), &beginval, &endval, &stepval);
  }

  /**
   * Version of range templated for C++ scalar types, with just the end
   * parameter.
   */
  template <class T>
  typename std::enable_if<is_dynd_scalar<T>::value, nd::array>::type old_range(T endval) {
    T beginval = T(0), stepval = T(1);
    return old_range(ndt::make_type<T>(), &beginval, &endval, &stepval);
  }

  /**
   * Version of range based on an irange object.
   */
  inline nd::array old_range(const irange &i) {
    return old_range(ndt::make_type<intptr_t>(), &i.start(), &i.finish(), &i.step());
  }

  /**
   * Most general linspace function, creates an array of length 'count',
   *linearly
   * interpolating from the value 'start' to the value 'stop'.
   *
   * \param start  The value placed at index 0 of the result.
   * \param stop  The value placed at index count-1 of the result.
   * \param count  The size of the result's first dimension.
   * \param tp  The required dtype of the output.
   */
  DYND_API nd::array old_linspace(const nd::array &start, const nd::array &stop, intptr_t count, const ndt::type &tp);

  /**
   * Most general linspace function, creates an array of length 'count',
   *linearly
   * interpolating from the value 'start' to the value 'stop'. This version
   * figures out the type from that of 'start' and 'stop'.
   *
   * \param start  The value placed at index 0 of the result.
   * \param stop  The value placed at index count-1 of the result.
   * \param count  The size of the result's first dimension.
   */
  DYND_API nd::array old_linspace(const nd::array &start, const nd::array &stop, intptr_t count);

  /**
   * Creates a one-dimensional array of 'count' values, evenly spaced
   * from 'startval' to 'stopval', including both values.
   *
   * The only built-in types supported are float and double.
   */
  DYND_API nd::array old_linspace(const ndt::type &dt, const void *startval, const void *stopval, intptr_t count);

  inline nd::array old_linspace(float start, float stop, intptr_t count = 50) {
    return old_linspace(ndt::type(float32_id), &start, &stop, count);
  }

  inline nd::array old_linspace(double start, double stop, intptr_t count = 50) {
    return old_linspace(ndt::type(float64_id), &start, &stop, count);
  }

  template <class T>
  typename std::enable_if<is_signed_integral<T>::value || is_unsigned_integral<T>::value, nd::array>::type
  old_linspace(T start, T stop, intptr_t count = 50) {
    return old_linspace((double)start, (double)stop, count);
  }
}
} // namespace dynd::nd
