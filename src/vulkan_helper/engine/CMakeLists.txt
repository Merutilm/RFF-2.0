
add_subdirectory(buffer)
add_subdirectory(cmd)
add_subdirectory(configurator)
add_subdirectory(context)
add_subdirectory(descriptor)
add_subdirectory(executor)
add_subdirectory(graphics)
add_subdirectory(manage)
add_subdirectory(pipeline)
add_subdirectory(repo)
add_subdirectory(sampler)
add_subdirectory(sync)
add_subdirectory(window)


add_library(engine OBJECT
        Engine.cpp
        SharedResource.cpp
)

target_link_libraries(engine PRIVATE
        vkh_dependency
)