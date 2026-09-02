#include <mpa.glsl>

#ifndef DESC_RENDER_META_INCLUDE
#define DESC_RENDER_META_INCLUDE

#extension GL_EXT_shader_explicit_arithmetic_types_int64: require

struct BatchStagingData {
    uint64_t iteration;
    uint64_t ref_iteration;
    vec2 dz;
    float distance2;
};


layout (std430, set = DESC_RENDER_META, binding = 0) readonly buffer RenderMeta {
    uint64_t max_iteration;
    uint64_t max_ref_iteration;
    float log_zoom;
    float bailout;
    float clarity_multiplier;
    uint decimalize_iteration_method;
    vec2 offset;
    vec2 orbit[];
} render_meta;


layout (std430, set = DESC_RENDER_META, binding = 1) readonly buffer MPTableMeta {
    uint64_t len;
    uint selection_method;
    //padding 4
    PA[] table; //28 bytes, padding per elem 4
} mp_table_meta;

layout (std430, set = DESC_RENDER_META, binding = 2) readonly buffer MPMapperMeta {
    uint64_t len;
    MPIndexMapper[] mapper;
} mp_mapper_meta;


layout (set = DESC_RENDER_META, binding = 3) uniform BatchInfo{
    uint batch_size;
} batch_info;

layout (std430, set = DESC_RENDER_META, binding = 4) buffer BatchData{
    BatchStagingData[] staging_values;
} batch_data;

#endif