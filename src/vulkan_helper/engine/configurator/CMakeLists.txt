add_library(engine_configurator OBJECT
        ComputePipelineConfigurator.cpp
        GeneralGraphicsPipelineConfigurator.cpp
        GeneralPostProcessGraphicsPipelineConfigurator.cpp
)

target_link_libraries(engine_configurator PRIVATE
        vkh_dependency
)