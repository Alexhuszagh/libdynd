//
// Copyright (C) 2011-13 Mark Wiebe, DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#ifndef _DYND__STRUCT_COMPARISON_KERNELS_HPP_
#define _DYND__STRUCT_COMPARISON_KERNELS_HPP_

#include <dynd/kernels/comparison_kernels.hpp>
#include <dynd/typed_data_assign.hpp>

namespace dynd {

/**
 * Makes a kernel which lexicographically compares two
 * instances of the same struct/cstruct.
 */
size_t make_struct_comparison_kernel(
                hierarchical_kernel *out, size_t offset_out,
                const ndt::type& src_dt,
                const char *src0_metadata, const char *src1_metadata,
                comparison_type_t comptype,
                const eval::eval_context *ectx);

/**
 * Makes a kernel which lexicographically compares two
 * instances with struct_kind.
 */
size_t make_general_struct_comparison_kernel(
                hierarchical_kernel *out, size_t offset_out,
                const ndt::type& src0_dt, const char *src0_metadata,
                const ndt::type& src1_dt, const char *src1_metadata,
                comparison_type_t comptype,
                const eval::eval_context *ectx);

} // namespace dynd

#endif // _DYND__STRUCT_COMPARISON_KERNELS_HPP_
