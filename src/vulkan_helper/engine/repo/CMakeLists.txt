add_library(engine_repo OBJECT
        GlobalDescriptorSetLayoutRepo.cpp
        GlobalPipelineLayoutRepo.cpp
        GlobalSamplerRepo.cpp
        GlobalShaderModuleRepo.cpp
        WindowLocalDescriptorRepo.cpp
)

target_link_libraries(engine_repo PRIVATE
        vkh_dependency
)