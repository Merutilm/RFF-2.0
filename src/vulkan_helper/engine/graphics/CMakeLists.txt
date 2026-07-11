add_library(engine_graphics OBJECT
        Framebuffer.cpp
        Renderer.cpp
        RenderPass.cpp
        RenderPassGraphGenerator.cpp
        RenderPassGraphGeneratorBase.cpp
)

target_link_libraries(engine_graphics PRIVATE
        vkh_dependency
)