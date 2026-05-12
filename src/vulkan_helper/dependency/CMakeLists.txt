
find_package(Vulkan REQUIRED)

add_library(vkh_dependency OBJECT
        ${CMAKE_SOURCE_DIR}/extern/stb_image.c
)

target_include_directories(vkh_dependency PUBLIC
        # standard include
        ${CMAKE_SOURCE_DIR}/include
        # extern sources
        ${CMAKE_SOURCE_DIR}/extern
        # vulkan sdk directory
        ${Vulkan_INCLUDE_DIRS}
)

