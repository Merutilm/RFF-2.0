add_library(engine_executor OBJECT
        RenderPassFullscreenRecorder.cpp
        ScopedCommandBufferExecutor.cpp
        ScopedNewCommandBufferExecutor.cpp
)

target_link_libraries(engine_executor PRIVATE
        vkh_dependency
)