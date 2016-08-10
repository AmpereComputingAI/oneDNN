#include "nstl.hpp"

#include "c_types_map.hpp"
#include "type_helpers.hpp"
#include "engine.hpp"
#include "primitive.hpp"

#include "utils.hpp"

using namespace mkl_dnn::impl;
using namespace mkl_dnn::impl::status;
using namespace mkl_dnn::impl::prop_kind;
using namespace mkl_dnn::impl::alg_kind;

status_t mkl_dnn_convolution_desc_init(convolution_desc_t *convolution_desc,
        prop_kind_t prop_kind, alg_kind_t alg_kind,
        const memory_desc_t *src_desc, const memory_desc_t *weights_desc,
        const memory_desc_t *bias_desc, const memory_desc_t *dst_desc,
        const dims_t strides, const nd_offset_t padding,
        padding_kind_t padding_kind)
{
    const bool args_ok = !any_null(convolution_desc, src_desc, weights_desc,
            dst_desc, strides, padding)
        && one_of(prop_kind, forward, backward_data, backward_weights,
                backward_bias)
        && implication(prop_kind == backward_bias, !any_null(bias_desc))
        && one_of(alg_kind, convolution_direct);
    if (!args_ok)
        return invalid_arguments;

    convolution_desc_t cd;
    cd.prop_kind = prop_kind;
    cd.alg_kind = alg_kind;
    cd.src_desc = *src_desc;
    cd.weights_desc = *weights_desc;
    cd.bias_desc = bias_desc ? *bias_desc : types::zero<memory_desc_t>();
    cd.dst_desc = *dst_desc;
    cd.padding_kind = padding_kind;
    const uint32_t ndims_spatial = src_desc->tensor_desc.ndims - 2;
    array_copy(cd.strides, strides, ndims_spatial);
    array_copy(cd.padding, padding, ndims_spatial);

    status_t status = types::convolution_desc_is_ok(cd);
    if (status == success)
        *convolution_desc = cd;

    return status;
}

status_t mkl_dnn_convolution_primitive_desc_init(
        convolution_primitive_desc_t *convolution_primitive_desc,
        const convolution_desc_t *convolution_desc,
        const engine *engine)
{
    if (any_null(convolution_primitive_desc, convolution_desc, engine))
        return invalid_arguments;

    return primitive_desc_init(
            primitive_desc_t::convert_from_c(convolution_primitive_desc),
            *convolution_desc, *engine);
}

status_t mkl_dnn_convolution_create(primitive **convolution,
        const convolution_primitive_desc_t *convolution_primitive_desc,
        const primitive_at_t src, const primitive_at_t weights,
        const primitive_at_t bias, const primitive *dst) {
    auto cpd = reinterpret_cast<const mkl_dnn_primitive_desc_t *>(
            convolution_primitive_desc);
    // XXX: must check that shapes of in/out memory match what's in the desc (?)
    const primitive_at_t inputs[] = {src, weights, bias};
    const primitive *outputs[] = {dst};
    return mkl_dnn_primitive_create(convolution, cpd, inputs, outputs);
}

status_t mkl_dnn_convolution_get_primitive_desc(const primitive *convolution,
        convolution_primitive_desc_t *convolution_primitive_desc)
{
    if (any_null(convolution, convolution_primitive_desc)
            || convolution->kind() != primitive_kind::convolution)
        return invalid_arguments;
    *convolution_primitive_desc = convolution->primitive_desc().convolution;
    return success;
}

// vim: et ts=4 sw=4 cindent cino^=l0,\:0,N-s
