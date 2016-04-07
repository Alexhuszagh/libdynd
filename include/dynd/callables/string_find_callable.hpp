//
// Copyright (C) 2011-16 DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#pragma once

#include <dynd/callables/default_instantiable_callable.hpp>
#include <dynd/kernels/string_find_kernel.hpp>

namespace dynd {
namespace nd {

  class string_find_callable : public default_instantiable_callable<string_find_kernel> {
  public:
    string_find_callable()
        : default_instantiable_callable<string_find_kernel>(
              ndt::callable_type::make(ndt::make_type<intptr_t>(), {ndt::type(string_id), ndt::type(string_id)}))
    {
    }
  };

} // namespace dynd::nd
} // namespace dynd