add_library(engine_context OBJECT
        BufferContext.cpp
        ImageContext.cpp
        RenderContext.cpp
        SharedImageContext.cpp
        WindowContext.cpp
)

target_link_libraries(engine_context PRIVATE
        vkh_dependency
)