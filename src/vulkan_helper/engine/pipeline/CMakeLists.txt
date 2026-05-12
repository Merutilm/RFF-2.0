add_library(engine_pipeline OBJECT
        ComputeShaderPipeline.cpp
        GraphicsPipeline.cpp
        Pipeline.cpp
        PipelineLayout.cpp
        PushConstant.cpp
        ShaderModule.cpp
)

target_link_libraries(engine_pipeline PRIVATE
        vkh_dependency
)