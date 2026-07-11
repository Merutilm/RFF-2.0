add_library(engine_buffer OBJECT
        BufferObject.cpp
        IndexBuffer.cpp
        ShaderStorage.cpp
        Uniform.cpp
        VertexBuffer.cpp
)

target_link_libraries(engine_buffer PRIVATE
        vkh_dependency
)