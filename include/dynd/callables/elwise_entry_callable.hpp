//
// Copyright (C) 2011-16 DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#pragma once

#include <dynd/callables/base_callable.hpp>
#include <dynd/callables/elwise_callable.hpp>
#include <dynd/callables/elwise_dispatch_callable.hpp>
#include <dynd/types/dim_fragment_type.hpp>

namespace dynd {
namespace nd {
  namespace functional {

    class elwise_entry_callable : public base_callable {
    public:
      typedef typename elwise_dispatch_callable<0>::data_type data_type;

      callable m_child;
      bool m_res_ignore;
      bool m_state;

      elwise_entry_callable(bool res_ignore)
          : base_callable(ndt::type("(...) -> Any")), m_res_ignore(res_ignore), m_state(false) {}

      elwise_entry_callable(const ndt::type &tp, const callable &child, bool state, bool res_ignore)
          : base_callable(tp), m_child(child), m_res_ignore(res_ignore), m_state(state) {}

      ndt::type resolve(base_callable *caller, char *DYND_UNUSED(data), call_graph &cg, const ndt::type &dst_tp,
                        size_t nsrc, const ndt::type *src_tp, size_t nkwd, const array *kwds,
                        const std::map<std::string, ndt::type> &tp_vars) {
        data_type data{m_child ? m_child.get() : caller, m_res_ignore, m_state, 0, true};

        if (m_state) {
          for (size_t i = 0; i < nsrc; ++i) {
            size_t ndim = src_tp[i].get_ndim() - data.child->get_argument_types()[i].get_ndim();
            if (ndim > data.ndim) {
              data.ndim = ndim;
            }
          }

          cg.emplace_back([ndim = data.ndim](kernel_builder & kb, kernel_request_t kernreq, char *data,
                                             const char *dst_arrmeta, size_t nsrc, const char *const *src_arrmeta) {
            kb.pass();

            state &st = *reinterpret_cast<state *>(data);
            st.ndim = ndim;
            st.index = new size_t[ndim];

            kb(kernreq, reinterpret_cast<char *>(st.index), dst_arrmeta, nsrc, src_arrmeta);
          });
        }

        static callable table[8] = {
            make_callable<elwise_dispatch_callable<0>>(), make_callable<elwise_dispatch_callable<1>>(),
            make_callable<elwise_dispatch_callable<2>>(), make_callable<elwise_dispatch_callable<3>>(),
            make_callable<elwise_dispatch_callable<4>>(), make_callable<elwise_dispatch_callable<5>>(),
            make_callable<elwise_dispatch_callable<6>>(), make_callable<elwise_dispatch_callable<7>>()};
        return table[nsrc]->resolve(this, reinterpret_cast<char *>(&data), cg, dst_tp, nsrc, src_tp, nkwd, kwds,
                                    tp_vars);
      }
    };

  } // namespace dynd::nd::functional
} // namespace dynd::nd
} // namespace dynd
