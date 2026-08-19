#ifndef DESC_ITERATION_INCLUDE
#define DESC_ITERATION_INCLUDE

#ifndef DESC_ITERATION_BUFFER_ACCESS
#define DESC_ITERATION_BUFFER_ACCESS readonly
#endif

layout (set = DESC_ITERATION, binding = 0) uniform IterUBO {
    uvec2 extent;
    double max_value;
} iteration_info_settings;

layout (set = DESC_ITERATION, binding = 1) DESC_ITERATION_BUFFER_ACCESS buffer IterSSBO {
    double iterations[];
} iteration_settings;

#endif