add_library(engine_sampler OBJECT
        CombinedImageSampler.cpp
        Sampler.cpp
)

target_link_libraries(engine_sampler PRIVATE
        vkh_dependency
)