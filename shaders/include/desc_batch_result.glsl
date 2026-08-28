#ifndef DESC_BATCH_RESULT_INCLUDE
#define DESC_BATCH_RESULT_INCLUDE

layout (std430, set = DESC_BATCH_RESULT, binding = 0) buffer BatchResultData{
    uint[] completed;
} batch_result_data;


#endif