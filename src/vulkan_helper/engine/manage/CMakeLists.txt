add_library(engine_manage OBJECT
        DescriptorManager.cpp
        HostDataObjectManager.cpp
        PipelineLayoutManager.cpp
        PipelineManager.cpp
        RenderPassManager.cpp
)

target_link_libraries(engine_manage PRIVATE
        vkh_dependency
)