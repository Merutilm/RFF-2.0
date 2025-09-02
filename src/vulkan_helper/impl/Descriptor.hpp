//
// Created by Merutilm on 2025-07-09.
//

#pragma once

#include "DescriptorSetLayout.hpp"
#include "../context/DescriptorUpdateContext.hpp"
#include "../manage/DescriptorManager.hpp"


namespace merutilm::vkh {
    /**
     * The Index Picker of Descriptor.
     * In Update, Empty picker means all descriptors.
     * In Enumeration, Empty picker means filled zeros.
     */
    using DescIndexPicker = std::vector<uint32_t>;


    class DescriptorImpl final : public CoreHandler {
        std::vector<VkDescriptorPool> descriptorPools = {};
        std::vector<std::vector<VkDescriptorSet> > descriptorSets = {};
        DescriptorSetLayoutRef descriptorSetLayout;
        std::vector<std::vector<DescriptorType> > data;

    public:
        explicit DescriptorImpl(CoreRef core, DescriptorSetLayoutRef descriptorSetLayout,
                                std::vector<DescriptorManager> &&descriptorManager);

        ~DescriptorImpl() override;

        DescriptorImpl(const DescriptorImpl &) = delete;

        DescriptorImpl &operator=(const DescriptorImpl &) = delete;

        DescriptorImpl(DescriptorImpl &&) = delete;

        DescriptorImpl &operator=(DescriptorImpl &&) = delete;


        [[nodiscard]] VkDescriptorSet getDescriptorSetHandle(const uint32_t frameIndex,
                                                             const uint32_t descIndex = 0) const {
            safe_array::check_index(descIndex, descriptorSets[frameIndex].size(), "Descriptor index");
            return descriptorSets[frameIndex][descIndex];
        }

        [[nodiscard]] VkDescriptorPool getDescriptorPoolHandle(const uint32_t frameIndex) const {
            return descriptorPools[frameIndex];
        }

        template<typename T>
        [[nodiscard]] uint32_t getElementCount() const {
            uint32_t count = 0;
            for (auto &variant: data[0]) {
                if (std::holds_alternative<T>(variant)) ++count;
            }
            return count;
        }

        [[nodiscard]] uint32_t getDescriptorElements() const {
            return static_cast<uint32_t>(data[0].size());
        }

        [[nodiscard]] const DescriptorType &getRaw(const uint32_t descIndex, const uint32_t bindingIndex) const {
            if (bindingIndex >= data[descIndex].size()) {
                throw exception_invalid_args("bindingIndex out of range");
            }
            return data[descIndex][bindingIndex];
        }

        [[nodiscard]] uint32_t getDescriptorCount() const {
            return static_cast<uint32_t>(data.size());
        }

        template<typename T>
        [[nodiscard]] T &get(
            const uint32_t descIndex, const uint32_t bindingIndex) {
            if (bindingIndex >= data[descIndex].size()) {
                throw exception_invalid_args("binding out of range");
            }
            return std::get<T>(data[descIndex][bindingIndex]);
        }

        [[nodiscard]] DescriptorSetLayoutRef getLayout() const {
            return descriptorSetLayout;
        }

        void queue(DescriptorUpdateQueue &updateQueue, uint32_t frameIndex,
            DescIndexPicker &&descIndices,
            DescIndexPicker &&bindings) const;

    private:
        void updateIndices(DescriptorUpdateQueue &updateQueue, uint32_t frameIndex,
                           const std::vector<uint32_t> &descIndices, const std::vector<uint32_t> &bindings) const;

        void init() override;

        void destroy() override;
    };


    using Descriptor = std::unique_ptr<DescriptorImpl>;
    using DescriptorPtr = DescriptorImpl *;
    using DescriptorRef = DescriptorImpl &;
}
