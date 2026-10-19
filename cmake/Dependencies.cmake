
if (WIN32)
    if ("${MSYS2_CLANG}" STREQUAL "")
        message(FATAL_ERROR ${RFF_HELP_MSYS2})
    else ()
        set(CLANG64_DIRS "${MSYS2_CLANG}")
        set(CLANG64_LIBS "${MSYS2_CLANG}/lib")
    endif ()
endif ()


add_subdirectory(${CMAKE_SOURCE_DIR}/src/vulkan_helper)

target_link_libraries(RFF PRIVATE
        # Vulkan Helper
        vulkan_helper
)

target_include_directories(RFF PRIVATE
        # extern sources
        ${CMAKE_SOURCE_DIR}/extern
        # clang include dirs
        ${CMAKE_SOURCE_DIR}/include
)
if (WIN32)
    target_link_libraries(
            RFF PRIVATE
            # GMP Linker
            ${CLANG64_LIBS}/libgmp.a
            # OpenCV Linkers
            ${CLANG64_LIBS}/libopencv_core4110.dll.a
            ${CLANG64_LIBS}/libopencv_imgcodecs4110.dll.a
            ${CLANG64_LIBS}/libopencv_imgproc4110.dll.a
            ${CLANG64_LIBS}/libopencv_videoio4110.dll.a
            ${CLANG64_LIBS}/libopencv_highgui4110.dll.a
            # FFmpeg Linkers
#            ${CLANG64_LIBS}/libavcodec.dll.a
#            ${CLANG64_LIBS}/libavformat.dll.a
#            ${CLANG64_LIBS}/libavutil.dll.a
    )


    target_include_directories(RFF PRIVATE
            # clang include dirs
            ${CLANG64_DIRS}/include
            # opencv4 include dirs
            ${CLANG64_DIRS}/include/opencv4
    )
elseif (APPLE)
    target_link_libraries(RFF PRIVATE "-framework Cocoa")
    # TODO non compilable
elseif (UNIX)

    macro(find_static_library OUT_VAR LIB_NAME)
        find_library(${OUT_VAR}
                NAMES lib${LIB_NAME}.a
                PATHS
                /usr/local/lib
                /usr/lib
                /usr/lib/x86_64-linux-gnu
                REQUIRED
        )
    endmacro()

    find_package(OpenCV REQUIRED)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
#    pkg_check_modules(FFMPEG REQUIRED
#            libavcodec
#            libavformat
#            libavutil
#    )

    find_static_library(GMP_STATIC gmp)

    target_link_directories(RFF PRIVATE ${GTK3_LIBRARY_DIRS})
    target_compile_options(RFF PRIVATE ${GTK3_CFLAGS_OTHER})

    target_link_libraries(RFF PRIVATE
            # GMP Linker
            ${GMP_STATIC}
            # gtk libraries
            ${GTK3_LIBRARIES}
            # OpenCV Linkers
            ${OpenCV_LIBS}
            # FFmpeg Linkers
            ${FFMPEG_LIBRARIES}
    )
    target_include_directories(RFF PRIVATE
            # opencv4 include dirs
            ${OpenCV_INCLUDE_DIRS}
            # gtk
            ${GTK3_INCLUDE_DIRS}
            # ffmpeg
            ${FFMPEG_INCLUDE_DIRS}
    )

    target_link_directories(RFF PRIVATE
            # ffmpeg
            ${FFMPEG_LIBRARY_DIRS}
    )

endif ()

include(FetchContent)

FetchContent_Declare(
        nfd
        GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
        GIT_TAG master
)

FetchContent_MakeAvailable(nfd)

target_link_libraries(RFF PRIVATE nfd)