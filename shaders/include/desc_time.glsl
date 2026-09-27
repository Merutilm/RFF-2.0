#ifndef DESC_TIME_INCLUDE
#define DESC_TIME_INCLUDE

layout (set = DESC_TIME, binding = 0) uniform TimeUBO {
    double time;
} time_settings;

#endif