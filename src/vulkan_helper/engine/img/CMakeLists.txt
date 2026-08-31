add_library(engine_img OBJECT
        CombinedImageSampler.cpp
        Sampler.cpp
        StorageImage.cpp
)

target_link_libraries(engine_img PRIVATE
        vkh_dependency
)