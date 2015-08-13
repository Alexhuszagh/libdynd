//
// Copyright (C) 2011-15 DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#pragma once

#include <dynd/kernels/base_kernel.hpp>
#include <dynd/func/callable.hpp>

namespace dynd {
namespace nd {
  namespace functional {

    // Right associative, evaluate the reduction from right to left:
    //    dst_(0) = a[n-1]
    //    dst_(i+1) = dst_(i) <OP> a[n-1-(i+1)]
    struct left_compound_kernel
        : base_kernel<left_compound_kernel, kernel_request_host, 1> {
      void single(char *dst, char *const *src)
      {
        ckernel_prefix *child = get_child_ckernel();
        expr_single_t single = child->get_function<expr_single_t>();
        char *src_binary[2] = {dst, src[0]};
        single(child, dst, src_binary);
      }

      void strided(char *dst, intptr_t dst_stride, char *const *src,
                   const intptr_t *src_stride, size_t count)
      {
        ckernel_prefix *child = get_child_ckernel();
        expr_strided_t childop = child->get_function<expr_strided_t>();
        char *src_binary[2] = {dst, src[0]};
        const intptr_t src_binary_stride[2] = {dst_stride, src_stride[0]};
        childop(child, dst, dst_stride, src_binary, src_binary_stride, count);
      }

      void destruct_children()
      {
        get_child_ckernel()->destroy();
      }

      static intptr_t
      instantiate(char *static_data, size_t data_size, char *data, void *ckb,
                  intptr_t ckb_offset, const ndt::type &dst_tp,
                  const char *dst_arrmeta, intptr_t nsrc,
                  const ndt::type *src_tp, const char *const *src_arrmeta,
                  kernel_request_t kernreq, const eval::eval_context *ectx,
                  const nd::array &kwds,
                  const std::map<std::string, ndt::type> &tp_vars)
      {
        callable &child = *reinterpret_cast<callable *>(static_data);
        self_type::make(ckb, kernreq, ckb_offset);

        ndt::type child_src_tp[2] = {dst_tp, src_tp[0]};
        const char *child_src_arrmeta[2] = {dst_arrmeta, src_arrmeta[0]};
        return child.get()->instantiate(
            child.get()->static_data, data_size, data, ckb, ckb_offset, dst_tp,
            dst_arrmeta, nsrc + 1, child_src_tp, child_src_arrmeta, kernreq,
            ectx, kwds, tp_vars);
      }
    };

    // Left associative, evaluate the reduction from left to right:
    //    dst_(0) = a[0]
    //    dst_(i+1) = a[i+1] <OP> dst_(i)
    struct right_compound_kernel
        : base_kernel<right_compound_kernel, kernel_request_host, 1> {
      void single(char *dst, char *const *src)
      {
        ckernel_prefix *child = get_child_ckernel();
        expr_single_t childop = child->get_function<expr_single_t>();
        char *src_binary[2] = {src[0], dst};
        childop(child, dst, src_binary);
      }

      void strided(char *dst, intptr_t dst_stride, char *const *src,
                   const intptr_t *src_stride, size_t count)
      {
        ckernel_prefix *child = get_child_ckernel();
        expr_strided_t childop = child->get_function<expr_strided_t>();
        char *src_binary[2] = {src[0], dst};
        const intptr_t src_binary_stride[2] = {src_stride[0], dst_stride};
        childop(child, dst, dst_stride, src_binary, src_binary_stride, count);
      }

      void destruct_children()
      {
        get_child_ckernel()->destroy();
      }

      static intptr_t
      instantiate(char *static_data, size_t data_size, char *data, void *ckb,
                  intptr_t ckb_offset, const ndt::type &dst_tp,
                  const char *dst_arrmeta, intptr_t nsrc,
                  const ndt::type *src_tp, const char *const *src_arrmeta,
                  kernel_request_t kernreq, const eval::eval_context *ectx,
                  const nd::array &kwds,
                  const std::map<std::string, ndt::type> &tp_vars)
      {
        callable &child = *reinterpret_cast<callable *>(static_data);
        self_type::make(ckb, kernreq, ckb_offset);

        ndt::type child_src_tp[2] = {src_tp[0], dst_tp};
        const char *child_src_arrmeta[2] = {src_arrmeta[0], dst_arrmeta};
        return child.get()->instantiate(
            child.get()->static_data, data_size, data, ckb, ckb_offset, dst_tp,
            dst_arrmeta, nsrc + 1, child_src_tp, child_src_arrmeta, kernreq,
            ectx, kwds, tp_vars);
      }
    };

  } // namespace dynd::nd::functional
} // namespace dynd::nd
} // namespace dynd