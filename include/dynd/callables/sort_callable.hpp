//
// Copyright (C) 2011-16 DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#pragma once

#include <dynd/callables/base_callable.hpp>
#include <dynd/comparison.hpp>
#include <dynd/kernels/sort_kernel.hpp>

namespace dynd {
namespace nd {

  class sort_callable : public base_callable {
  public:
    sort_callable()
        : base_callable(ndt::make_type<ndt::callable_type>(ndt::make_type<void>(), {ndt::type("Fixed * Scalar")})) {}

    ndt::type resolve(base_callable *DYND_UNUSED(caller), char *DYND_UNUSED(data), call_graph &cg,
                      const ndt::type &dst_tp, size_t DYND_UNUSED(nsrc), const ndt::type *src_tp,
                      size_t DYND_UNUSED(nkwd), const array *DYND_UNUSED(kwds),
                      const std::map<std::string, ndt::type> &tp_vars) {
      const ndt::type &src0_element_tp = src_tp[0].extended<ndt::fixed_dim_type>()->get_element_type();
      size_t src0_element_data_size = src0_element_tp.get_data_size();
      cg.emplace_back([src0_element_data_size](kernel_builder &kb, kernel_request_t kernreq, char *DYND_UNUSED(data),
                                               const char *DYND_UNUSED(dst_arrmeta), size_t DYND_UNUSED(nsrc),
                                               const char *const *src_arrmeta) {
        kb.emplace_back<sort_kernel>(
            kernreq, reinterpret_cast<const fixed_dim_type_arrmeta *>(src_arrmeta[0])->dim_size,
            reinterpret_cast<const fixed_dim_type_arrmeta *>(src_arrmeta[0])->stride, src0_element_data_size);

        kb(kernel_request_single, nullptr, nullptr, 2, nullptr);
      });

      const ndt::type child_src_tp[2] = {src0_element_tp, src0_element_tp};
      less->resolve(this, nullptr, cg, ndt::make_type<bool1>(), 2, child_src_tp, 0, nullptr, tp_vars);

      return dst_tp;
    }
  };

} // namespace dynd::nd
} // namespace dynd
