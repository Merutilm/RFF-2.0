

add_library(engine_cmd OBJECT
        CommandBufferGroup.cpp
        CommandBuffer.cpp
        CommandPool.cpp
)

target_link_libraries(engine_cmd PRIVATE
        vkh_dependency
)