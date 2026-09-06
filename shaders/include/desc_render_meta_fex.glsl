#include <fex.glsl>
#include <mpa.glsl>

#ifndef DESC_RENDER_META_FEX_INCLUDE
#define DESC_RENDER_META_FEX_INCLUDE

#extension GL_EXT_shader_explicit_arithmetic_types_int64: require

struct FexBatchStagingData {
    uint64_t iteration;
    uint64_t ref_iteration;
    FexComplex dz;
    float distance2;
};

layout (std430, set = DESC_RENDER_META, binding = 0) readonly buffer FexRenderMeta {
    uint64_t max_iteration;
    uint64_t max_ref_iteration;
    float log_zoom;
    float bailout;
    float clarity_multiplier;
    uint decimalize_iteration_method;
    FexComplex offset;
    FexComplex orbit[];
} render_meta;


layout (set = DESC_RENDER_META, binding = 1) uniform MPTableInfo {
    uint64_t len;
    uint selection_method;
} mp_table_info;

layout (std430, set = DESC_RENDER_META, binding = 2) readonly buffer MPTableData {
    FexPA[] table;
} mp_table_data;

layout (std430, set = DESC_RENDER_META, binding = 3) readonly buffer FexMPMapperMeta {
    MPIndexMapper[] mapper;
} mp_mapper_meta;


layout (set = DESC_RENDER_META, binding = 4) uniform FexBatchInfo{
    uint batch_size;
} batch_info;

layout (std430, set = DESC_RENDER_META, binding = 5) buffer FexBatchData{
    FexBatchStagingData[] staging_values;
} batch_data;

#endif