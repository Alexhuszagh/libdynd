//
// Copyright (C) 2011-14 Mark Wiebe, DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#include <dynd/types/tuple_type.hpp>
#include <dynd/types/type_alignment.hpp>
#include <dynd/types/property_type.hpp>
#include <dynd/exceptions.hpp>
#include <dynd/func/make_callable.hpp>
#include <dynd/kernels/struct_assignment_kernels.hpp>
#include <dynd/kernels/struct_comparison_kernels.hpp>

using namespace std;
using namespace dynd;

tuple_type::~tuple_type()
{
}

void tuple_type::print_type(std::ostream& o) const
{
    // Use the tuple datashape syntax
    o << "(";
    for (size_t i = 0, i_end = m_field_count; i != i_end; ++i) {
        if (i != 0) {
            o << ", ";
        }
        o << get_field_type(i);
    }
    o << ")";
}

void tuple_type::transform_child_types(type_transform_fn_t transform_fn, void *extra,
                ndt::type& out_transformed_tp, bool& out_was_transformed) const
{
    nd::array tmp_field_types(
        nd::empty(m_field_count, ndt::make_strided_dim(ndt::make_type())));
    ndt::type *tmp_field_types_raw = reinterpret_cast<ndt::type *>(
        tmp_field_types.get_readwrite_originptr());

    bool was_transformed = false;
    for (size_t i = 0, i_end = m_field_count; i != i_end; ++i) {
        transform_fn(get_field_type(i), extra, tmp_field_types_raw[i],
                     was_transformed);
    }
    if (was_transformed) {
        tmp_field_types.flag_as_immutable();
        out_transformed_tp = ndt::make_tuple(tmp_field_types);
        out_was_transformed = true;
    } else {
        out_transformed_tp = ndt::type(this, true);
    }
}

ndt::type tuple_type::get_canonical_type() const
{
    nd::array tmp_field_types(
        nd::empty(m_field_count, ndt::make_strided_dim(ndt::make_type())));
    ndt::type *tmp_field_types_raw = reinterpret_cast<ndt::type *>(
        tmp_field_types.get_readwrite_originptr());

    for (size_t i = 0, i_end = m_field_count; i != i_end; ++i) {
        tmp_field_types_raw[i] = get_field_type(i).get_canonical_type();
    }

    tmp_field_types.flag_as_immutable();
    return ndt::make_tuple(tmp_field_types);
}

bool tuple_type::is_lossless_assignment(const ndt::type& dst_tp, const ndt::type& src_tp) const
{
    if (dst_tp.extended() == this) {
        if (src_tp.extended() == this) {
            return true;
        } else if (src_tp.get_type_id() == tuple_type_id) {
            return *dst_tp.extended() == *src_tp.extended();
        }
    }

    return false;
}

size_t tuple_type::make_assignment_kernel(
                ckernel_builder *DYND_UNUSED(out_ckb), size_t DYND_UNUSED(ckb_offset),
                const ndt::type& dst_tp, const char *DYND_UNUSED(dst_arrmeta),
                const ndt::type& src_tp, const char *DYND_UNUSED(src_arrmeta),
                kernel_request_t DYND_UNUSED(kernreq), assign_error_mode DYND_UNUSED(errmode),
                const eval::eval_context *DYND_UNUSED(ectx)) const
{
    /*
    if (this == dst_tp.extended()) {
        if (this == src_tp.extended()) {
            return make_tuple_identical_assignment_kernel(out_ckb, ckb_offset,
                            dst_tp,
                            dst_arrmeta, src_arrmeta,
                            kernreq, errmode, ectx);
        } else if (src_tp.get_kind() == struct_kind) {
            return make_tuple_assignment_kernel(out_ckb, ckb_offset,
                            dst_tp, dst_arrmeta,
                            src_tp, src_arrmeta,
                            kernreq, errmode, ectx);
        } else if (src_tp.is_builtin()) {
            return make_broadcast_to_struct_assignment_kernel(out_ckb, ckb_offset,
                            dst_tp, dst_arrmeta,
                            src_tp, src_arrmeta,
                            kernreq, errmode, ectx);
        } else {
            return src_tp.extended()->make_assignment_kernel(out_ckb, ckb_offset,
                            dst_tp, dst_arrmeta,
                            src_tp, src_arrmeta,
                            kernreq, errmode, ectx);
        }
    }
    */

    stringstream ss;
    ss << "Cannot assign from " << src_tp << " to " << dst_tp;
    throw dynd::type_error(ss.str());
}

size_t tuple_type::make_comparison_kernel(
                ckernel_builder *DYND_UNUSED(out), size_t DYND_UNUSED(offset_out),
                const ndt::type& src0_tp, const char *DYND_UNUSED(src0_arrmeta),
                const ndt::type& src1_tp, const char *DYND_UNUSED(src1_arrmeta),
                comparison_type_t comptype,
                const eval::eval_context *DYND_UNUSED(ectx)) const
{
    /*
    if (this == src0_dt.extended()) {
        if (*this == *src1_dt.extended()) {
            return make_tuple_comparison_kernel(out, offset_out,
                            src0_dt, src0_arrmeta, src1_arrmeta,
                            comptype, ectx);
        } else if (src1_dt.get_kind() == struct_kind) {
            return make_general_tuple_comparison_kernel(out, offset_out,
                            src0_dt, src0_arrmeta,
                            src1_dt, src1_arrmeta,
                            comptype, ectx);
        }
    }
    */

    throw not_comparable_error(src0_tp, src1_tp, comptype);
}

bool tuple_type::operator==(const base_type& rhs) const
{
    if (this == &rhs) {
        return true;
    } else if (rhs.get_type_id() != tuple_type_id) {
        return false;
    } else {
        const tuple_type *dt = static_cast<const tuple_type*>(&rhs);
        return get_data_alignment() == dt->get_data_alignment() &&
                m_field_types.equals_exact(dt->m_field_types);
    }
}

void tuple_type::metadata_debug_print(const char *arrmeta, std::ostream& o, const std::string& indent) const
{
    const size_t *data_offsets = reinterpret_cast<const size_t *>(arrmeta);
    o << indent << "tuple arrmeta\n";
    o << indent << " field offsets: ";
    for (size_t i = 0, i_end = m_field_count; i != i_end; ++i) {
        o << data_offsets[i];
        if (i != i_end - 1) {
            o << ", ";
        }
    }
    o << "\n";
    const uintptr_t *arrmeta_offsets = get_arrmeta_offsets_raw();
    for (size_t i = 0; i < m_field_count; ++i) {
        const ndt::type& field_dt = get_field_type(i);
        if (!field_dt.is_builtin() && field_dt.extended()->get_metadata_size() > 0) {
            o << indent << " field " << i << " arrmeta:\n";
            field_dt.extended()->metadata_debug_print(arrmeta + arrmeta_offsets[i], o, indent + "  ");
        }
    }
}

static nd::array property_get_field_types(const ndt::type& tp) {
    return tp.tcast<tuple_type>()->get_field_types();
}

static nd::array property_get_arrmeta_offsets(const ndt::type& tp) {
    return tp.tcast<tuple_type>()->get_arrmeta_offsets();
}

void tuple_type::get_dynamic_type_properties(const std::pair<std::string, gfunc::callable> **out_properties, size_t *out_count) const
{
    static pair<string, gfunc::callable> type_properties[] = {
        pair<string, gfunc::callable>(
            "field_types",
            gfunc::make_callable(&property_get_field_types, "self")),
        pair<string, gfunc::callable>(
            "arrmeta_offsets",
            gfunc::make_callable(&property_get_arrmeta_offsets, "self"))};

    *out_properties = type_properties;
    *out_count = sizeof(type_properties) / sizeof(type_properties[0]);
}
