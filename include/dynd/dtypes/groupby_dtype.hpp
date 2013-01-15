//
// Copyright (C) 2011-12, Dynamic NDArray Developers
// BSD 2-Clause License, see LICENSE.txt


#ifndef _DYND__GROUPBY_DTYPE_HPP_
#define _DYND__GROUPBY_DTYPE_HPP_

#include <dynd/dtype.hpp>
#include <dynd/dtypes/pointer_dtype.hpp>

namespace dynd {

struct groupby_dtype_metadata {
};

struct groupby_dtype_data {
    const char *data_values_pointer;
    const char *by_values_pointer;
};

/**
 * The groupby dtype represents a transformation of
 * operand values and by ndobjects into a 2D variable-sized
 * array whose rows are the groups as specified by a categorical
 * dtype.
 */
class groupby_dtype : public base_expression_dtype {
    dtype m_value_dtype, m_operand_dtype, m_groups_dtype;

public:
    groupby_dtype(const dtype& data_values_dtype, const dtype& by_values_dtype, const dtype& groups_dtype);

    virtual ~groupby_dtype();

    const dtype& get_value_dtype() const {
        return m_value_dtype;
    }
    const dtype& get_operand_dtype() const {
        return m_operand_dtype;
    }
    const dtype& get_groups_dtype() const {
        return m_groups_dtype;
    }
    void print_data(std::ostream& o, const char *metadata, const char *data) const;

    void print_dtype(std::ostream& o) const;

    // Only support POD data for now (TODO: support blockref)
    dtype_memory_management_t get_memory_management() const {
        return pod_memory_management;
    }

    dtype apply_linear_index(int nindices, const irange *indices, int current_i, const dtype& root_dt) const;

    /**
     * Given some metadata for the groupby dtype, returns the
     * metadata for the pointer dtype that points at the data
     * values.
     *
     * \param metadata  An instance of groupby dtype metadata.
     *
     * \returns  The pointer<data_values_dtype> metadata within the
     *           groupby metadata.
     */
    pointer_dtype_metadata *get_data_values_pointer_metadata(char *metadata) const {
        m_operand_dtype.at_single(0, const_cast<const char **>(&metadata));
        return reinterpret_cast<pointer_dtype_metadata *>(metadata);
    }

    /**
     * Given some metadata for the groupby dtype, returns the
     * metadata for the pointer dtype that points at the by
     * values.
     *
     * \param metadata  An instance of groupby dtype metadata.
     *
     * \returns  The pointer<by_values_dtype> metadata within the
     *           groupby metadata.
     */
    pointer_dtype_metadata *get_by_values_pointer_metadata(char *metadata) const {
        m_operand_dtype.at_single(1, const_cast<const char **>(&metadata));
        return reinterpret_cast<pointer_dtype_metadata *>(metadata);
    }

    void get_shape(size_t i, intptr_t *out_shape) const;
    void get_shape(size_t i, intptr_t *out_shape, const char *metadata) const;

    bool is_lossless_assignment(const dtype& dst_dt, const dtype& src_dt) const;

    bool operator==(const base_dtype& rhs) const;

    // For expression_kind dtypes - converts to/from the storage's value dtype
    void get_operand_to_value_kernel(const eval::eval_context *ectx,
                            kernel_instance<unary_operation_pair_t>& out_borrowed_kernel) const;
    void get_value_to_operand_kernel(const eval::eval_context *ectx,
                            kernel_instance<unary_operation_pair_t>& out_borrowed_kernel) const;
    dtype with_replaced_storage_dtype(const dtype& replacement_dtype) const;
};

/**
 * Makes an unaligned dtype to view the given dtype without alignment requirements.
 */
inline dtype make_groupby_dtype(const dtype& data_values_dtype,
                const dtype& by_values_dtype, const dtype& groups_dtype)
{
    return dtype(new groupby_dtype(data_values_dtype,
                    by_values_dtype, groups_dtype), false);
}

} // namespace dynd

#endif // _DYND__GROUPBY_DTYPE_HPP_