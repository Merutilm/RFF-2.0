

add_library(engine_cmd OBJECT
        CommandBuffer.cpp
        CommandPool.cpp
)

target_link_libraries(engine_cmd PRIVATE
        vkh_dependency
)