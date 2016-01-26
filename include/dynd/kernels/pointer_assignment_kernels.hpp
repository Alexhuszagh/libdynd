//
// Copyright (C) 2011-15 DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#pragma once

#include <dynd/kernels/base_kernel.hpp>

namespace dynd {

/**
 * Makes a kernel which assigns the pointer to a built-in value.
 */
DYND_API void make_builtin_value_to_pointer_assignment_kernel(nd::kernel_builder *ckb, type_id_t tp_id,
                                                              kernel_request_t kernreq);

/**
 * Makes a kernel which assigns the pointer to a value.
 */
DYND_API void make_value_to_pointer_assignment_kernel(nd::kernel_builder *ckb, const ndt::type &tp,
                                                      kernel_request_t kernreq);

} // namespace dynd
