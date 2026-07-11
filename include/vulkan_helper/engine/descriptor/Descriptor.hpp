//
// Created by Merutilm on 2025-07-09.
//

#pragma once

#include <vulkan_helper/engine/context/DescriptorUpdateContext.hpp>
#include <vulkan_helper/engine/descriptor/DescriptorSetLayout.hpp>
#include <vulkan_helper/engine/manage/DescriptorManager.hpp>


namespace merutilm::vkh {
    /**
     * The Index Picker of Descriptor.
     * In Update, Empty picker means all descriptors.
     * In Enumeration, Empty picker means filled zeros.
     */
    using DescIndexPicker = std::vector<uint32_t>;


    class Descriptor final : public CoreHandler {
        std::vector<VkDescriptorPool> descriptorPools = {};
        std::vector<std::vector<VkDescriptorSet>> descriptorSets = {};
        DescriptorSetLayout &descriptorSetLayout;
        std::vector<std::vector<DescriptorType>> data = {};

    public:
        explicit Descriptor(Core &core, DescriptorSetLayout &descriptorSetLayout, std::vector<DescriptorManager> &&manager);

        ~Descriptor() override;

        Descriptor(const Descriptor &) = delete;

        Descriptor &operator=(const Descriptor &) = delete;

        Descriptor(Descriptor &&) = delete;

        Descriptor &operator=(Descriptor &&) = delete;


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
                if (std::holds_alternative<std::unique_ptr<T>>(variant))
                    ++count;
            }
            return count;
        }

        [[nodiscard]] uint32_t getDescriptorElements() const { return static_cast<uint32_t>(data[0].size()); }

        [[nodiscard]] DescriptorType &getRaw(const uint32_t descIndex, const uint32_t bindingIndex) {
            if (bindingIndex >= data[descIndex].size()) {
                throw exception_invalid_args("bindingIndex out of range");
            }
            return data[descIndex][bindingIndex];
        }

        [[nodiscard]] uint32_t getDescriptorCount() const { return static_cast<uint32_t>(data.size()); }

        template<typename T>
        [[nodiscard]] T &get(const uint32_t descIndex, const uint32_t bindingIndex) {
            if (bindingIndex >= data[descIndex].size()) {
                throw exception_invalid_args("binding out of range");
            }
            return *std::get<std::unique_ptr<T>>(data[descIndex][bindingIndex]);
        }

        [[nodiscard]] DescriptorSetLayout &getLayout() const { return descriptorSetLayout; }

        void queue(DescriptorUpdateQueue &updateQueue, uint32_t frameIndex, DescIndexPicker &&descIndices,
                   DescIndexPicker &&bindings);


    protected:
        void init() override;

        void cleanup() override;

    private:
        void updateIndices(DescriptorUpdateQueue &updateQueue, uint32_t frameIndex,
                           const std::vector<uint32_t> &descIndices, const std::vector<uint32_t> &bindings);

    };


} // namespace merutilm::vkh
