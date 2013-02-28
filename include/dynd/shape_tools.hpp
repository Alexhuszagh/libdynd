//
// Copyright (C) 2011-13, DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#ifndef _DYND__SHAPE_TOOLS_HPP_
#define _DYND__SHAPE_TOOLS_HPP_

#include <iostream>

#include <dynd/dtype.hpp>
#include <dynd/shortvector.hpp>
#include <dynd/ndobject.hpp>

namespace dynd {

/**
 * This function returns true if the src_shape can broadcast to the dst_shape
 * It's following the same rules as numpy. The
 * destination ndim must be greator or equal, and each
 * dimension size must be broadcastable with everything
 * shoved to the right.
 */
bool shape_can_broadcast(size_t dst_ndim, const intptr_t *dst_shape,
                        size_t src_ndim, const intptr_t *src_shape);

inline bool shape_can_broadcast(const std::vector<intptr_t>& dst_shape,
                        const std::vector<intptr_t>& src_shape)
{
    return shape_can_broadcast(dst_shape.size(), dst_shape.empty() ? NULL : &dst_shape[0],
                        src_shape.size(), src_shape.empty() ? NULL : &src_shape[0]);
}

/**
 * This function broadcasts the dimensions and strides of 'src' to a given
 * shape, raising an error if it cannot be broadcast.
 *
 * \param ndim        The number of dimensions being broadcast to.
 * \param shape       The shape being broadcast to.
 * \param src_ndim    The number of dimensions of the input which is to be broadcast.
 * \param src_shape   The shape of the input which is to be broadcast.
 * \param src_strides The strides of the input which is to be broadcast.
 * \param out_strides The resulting strides after broadcasting (with length 'ndim').
 */
void broadcast_to_shape(size_t ndim, const intptr_t *shape,
                size_t src_ndim, const intptr_t *src_shape, const intptr_t *src_strides,
                intptr_t *out_strides);

/**
 * This function broadcasts the input ndobject's shapes together,
 * producing a broadcast shape as the result. For any dimension in
 * an input with a variable-sized shape, the output shape is set
 * to a negative value.
 *
 * \param ninputs  The number of inputs whose shapes are to be broadcasted.
 * \param inputs  The inputs whose shapes are to be broadcasted.
 * \param out_undim  The number of dimensions in the output shape.
 * \param out_shape  This is filled with the broadcast shape.
 * \param out_axis_perm  A permutation of the axis for the output to use to match the input's memory ordering.
 */
void broadcast_input_shapes(size_t ninputs, const ndobject* inputs,
                        size_t& out_undim, dimvector& out_shape, shortvector<int>& out_axis_perm);

/**
 * Adjusts out_shape to broadcast it with the input shape.
 *
 * \param out_undim  The number of dimensions in the output
 *                   broadcast shape. This should be set to
 *                   the maximum of all the input undim values
 *                   that will be incrementally broadcasted.
 * \param out_shape  The shape that gets updated to become the
 *                   final broadcast shape. This should be
 *                   initialized to all ones before incrementally
 *                   broadcasting.
 * \param undim  The number of dimensions in the input shape.
 * \param shape  The input shape.
 */
void incremental_broadcast(size_t out_undim, intptr_t *out_shape,
                size_t undim, const intptr_t *shape);

/**
 * This function broadcasts the three operands together to create an output
 * with the broadcast result, swapping in the provided dtype for the uniform
 * dimension.
 *
 * \param result_inner_dt  The dtype that the output should have after the broadcast uniform dims.
 * \param op0  The first operand to broadcast.
 * \param op1  The second operand to broadcast.
 * \param op2  The third operand to broadcast.
 * \param out  This is populated with the created ndobject.
 * \param out_ndim  This is populated with the broadcast ndim.
 * \param out_shape  This is populated with the broadcast shape.
 */
void create_broadcast_result(const dtype& result_inner_dt,
                const ndobject& op0, const ndobject& op1, const ndobject& op2,
                ndobject &out, size_t& out_ndim, dimvector& out_shape);

/**
 * This function creates a permutation based on one ndarray's strides.
 * The value strides(out_axis_perm[0]) is the smallest stride,
 * and strides(out_axis_perm[ndim-1]) is the largest stride.
 *
 * \param ndim  The number of values in strides and out_axis_perm.
 * \param strides  The strides values used for sorting.
 * \param out_axis_perm  A permutation which corresponds to the input strides.
 */
void strides_to_axis_perm(size_t ndim, const intptr_t *strides, int *out_axis_perm);

/**
 * This function creates fresh strides based on the provided axis
 * permutation and element size. This function does not validate
 * that axis_perm is a valid permutation, the caller must ensure
 * this.
 *
 * \param ndim  The number of elements in axis_perm and out_strides.
 * \param axis_perm  A permutation of the axes, must contain the values
 *                   [0, ..., ndim) in some order.
 * \param shape  The shape of the array for the created strides.
 * \param element_size  The size of one array element (this is the smallest
 *                      stride in the created strides array.
 * \param out_strides  The calculated strides are placed here.
 */
void axis_perm_to_strides(size_t ndim, const int *axis_perm,
                const intptr_t *shape, intptr_t element_size, intptr_t *out_strides);

/**
 * This function creates a permutation based on the array of operand strides,
 * trying to match the memory ordering of both where possible and defaulting to
 * C-order where not possible.
 */
void multistrides_to_axis_perm(size_t ndim, int noperands, const intptr_t **operstrides, int *out_axis_perm);

// For some reason casting 'intptr_t **' to 'const intptr_t **' causes
// a warning in g++ 4.6.1, this overload works around that.
inline void multistrides_to_axis_perm(size_t ndim, int noperands, intptr_t **operstrides, int *out_axis_perm) {
    multistrides_to_axis_perm(ndim, noperands,
                const_cast<const intptr_t **>(operstrides), out_axis_perm);
}

void print_shape(std::ostream& o, size_t ndim, const intptr_t *shape);

inline void print_shape(std::ostream& o, const std::vector<intptr_t>& shape) {
    print_shape(o, (int)shape.size(), shape.empty() ? NULL : &shape[0]);
}

/**
 * Applies the indexing rules for a single linear indexing irange object to
 * a dimension of the specified size.
 *
 * \param idx  The irange indexing object.
 * \param dimension_size  The size of the dimension to which the idx is being applied.
 * \param error_i  The position in the shape where the indexing is being applied.
 * \param error_dt The dtype to which the indexing is being applied, or NULL.
 * \param out_remove_dimension  Is set to true if the dimension should be removed
 * \param out_start_index  The start index of the resolved indexing.
 * \param out_index_stride  The index stride of the resolved indexing.
 * \param out_dimension_size  The size of the resulting dimension from the resolved indexing.
 */
void apply_single_linear_index(const irange& idx, intptr_t dimension_size, size_t error_i, const dtype* error_dt,
        bool& out_remove_dimension, intptr_t& out_start_index, intptr_t& out_index_stride, intptr_t& out_dimension_size);

/**
 * \brief Applies indexing rules for a single integer index, returning an index in the range [0, dimension_size).
 *
 * \param i0  The integer index.
 * \param dimension_size  The size of the dimension being indexed.
 * \param error_dt  If non-NULL, a dtype used for error messages.
 *
 * \returns  An index value in the range [0, dimension_size).
 */
inline intptr_t apply_single_index(intptr_t i0, intptr_t dimension_size, const dtype* error_dt) {
    if (i0 >= 0) {
        if (i0 < dimension_size) {
            return i0;
        } else {
            if (error_dt) {
                size_t ndim = error_dt->extended()->get_undim();
                dimvector shape(ndim);
                error_dt->extended()->get_shape(0, shape.get());
                throw index_out_of_bounds(i0, 0, ndim, shape.get());
            } else {
                throw index_out_of_bounds(i0, dimension_size);
            }
        }
    } else if (i0 >= -dimension_size) {
        return i0 + dimension_size;
    } else {
        if (error_dt) {
            size_t ndim = error_dt->extended()->get_undim();
            dimvector shape(ndim);
            error_dt->extended()->get_shape(0, shape.get());
            throw index_out_of_bounds(i0, 0, ndim, shape.get());
        } else {
            throw index_out_of_bounds(i0, dimension_size);
        }
    }
}

/**
 * \brief Checks whether an array represents a valid permutation.
 *
 * \param size  The number of entries in the permutation
 * \param perm  The permutation array.
 *
 * \returns  True if it's a valid permutation, false otherwise.
 */
inline bool is_valid_perm(size_t size, const int *perm) {
    shortvector<char> flags(size);
    memset(flags.get(), 0, size);
    for (size_t i = 0; i < size; ++i) {
        int v = perm[i];
        if (static_cast<unsigned int>(v) >= size || flags[v]) {
            return false;
        }
        flags[v] = 1;
    }
    return true;
}

inline bool strides_are_c_contiguous(size_t ndim, intptr_t element_size, const intptr_t *shape, const intptr_t *strides) {
    // The loop counter must be a signed integer for this reverse loop to work
    for (intptr_t i = static_cast<intptr_t>(ndim)-1; i >= 0; --i) {
        if (shape[i] != 1 && strides[i] != element_size) {
            return false;
        }
        element_size *= shape[i];
    }
    return true;
}

inline bool strides_are_f_contiguous(size_t ndim, intptr_t element_size, const intptr_t *shape, const intptr_t *strides) {
    for (size_t i = 0; i < ndim; ++i) {
        if (shape[i] != 1 && strides[i] != element_size) {
            return false;
        }
        element_size *= shape[i];
    }
    return true;
}

enum shape_signal_t {
    /** Shape value that has never been initialized */
    shape_signal_uninitialized = -2,
    /** Shape value that may have more than one size, depending on index */
    shape_signal_varying = -1,
};

} // namespace dynd

#endif // _DYND__SHAPE_TOOLS_HPP_
