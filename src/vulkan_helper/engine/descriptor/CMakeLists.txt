add_library(engine_descriptor OBJECT
        Descriptor.cpp
        DescriptorSetLayout.cpp
)

target_link_libraries(engine_descriptor PRIVATE
        vkh_dependency
)