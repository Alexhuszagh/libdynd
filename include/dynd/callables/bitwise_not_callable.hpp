//
// Copyright (C) 2011-16 DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#pragma once

#include <dynd/callables/apply_function_callable.hpp>
#include <dynd/kernels/arithmetic.hpp>

namespace dynd {
namespace nd {

  template <typename Arg0Type>
  using bitwise_not_callable =
      functional::apply_function_callable<decltype(&dynd::detail::inline_bitwise_not<Arg0Type>::f),
                                          &dynd::detail::inline_bitwise_not<Arg0Type>::f>;

} // namespace dynd::nd
} // namespace dynd
