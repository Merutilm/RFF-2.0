//
// Created by Merutilm on 8/26/26.
//

#pragma once
#include <vector>

namespace merutilm::vkh {

    struct PipelineSpecialization {

        uint32_t count;
        std::vector<VkSpecializationMapEntry> entries;
        std::vector<std::vector<std::byte>> data;

        explicit PipelineSpecialization(const uint32_t count) : count(count), data(count) {
            if (count == 0) throw exception_init("Pipeline specialization count cannot be 0");
        }


        template<typename T>
        void appendEntry(const uint32_t idExpected, std::vector<T> &&values) {
            safe_array::check_index_equal(idExpected, entries.size(), "specialization entry count");
            safe_array::check_index_equal(values.size(), count, "specialization count");

            const auto off = static_cast<uint32_t>(data[0].size());
            for (uint32_t i = 0; i < count; ++i) {
                data[i].resize(off + sizeof(T));
                memcpy(&data[i][off], &values[i], sizeof(T));
            }
            entries.push_back(VkSpecializationMapEntry{.constantID = idExpected, .offset = off, .size = sizeof(T)});
        }

        bool isEmpty() const {
            return entries.empty();
        }

        std::vector<VkSpecializationInfo> buildSpecializationInfo() const {

            std::vector<VkSpecializationInfo> result(count);
            for (uint32_t i = 0; i < count; ++i) {
                result[i] = VkSpecializationInfo{
                    .mapEntryCount = static_cast<uint32_t>(entries.size()),
                    .pMapEntries = entries.data(),
                    .dataSize = data[i].size(),
                    .pData = data[i].data()
                };
            }
            return result;
        }
    };
} // namespace merutilm::vkh
