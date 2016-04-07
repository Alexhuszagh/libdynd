//
// Copyright (C) 2011-16 DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#pragma once

#include <dynd/callables/base_dispatch_callable.hpp>

namespace dynd {
namespace nd {

  class uniform_dispatch_callable : public base_dispatch_callable {
    dispatcher<callable> m_dispatcher;

  public:
    uniform_dispatch_callable(const ndt::type &tp, const dispatcher<callable> &dispatcher)
        : base_dispatch_callable(tp), m_dispatcher(dispatcher)
    {
    }

    void overload(const ndt::type &dst_tp, intptr_t DYND_UNUSED(nsrc), const ndt::type *DYND_UNUSED(src_tp),
                  const callable &value)
    {
      m_dispatcher.insert({{dst_tp.get_id()}, value});
    }

    const callable &specialize(const ndt::type &dst_tp, intptr_t DYND_UNUSED(nsrc),
                               const ndt::type *DYND_UNUSED(src_tp))
    {
      return m_dispatcher(dst_tp.get_id());
    }
  };

} // namespace dynd::nd
} // namespace dynd