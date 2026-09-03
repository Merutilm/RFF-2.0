//
// Created by Merutilm on 8/29/26.
//

#pragma once
#include "SharedDescriptorManager.hpp"
#include "SharedDescriptorTemplate.hpp"
#include "vulkan_helper/engine/Engine.hpp"
#include "vulkan_helper/engine/repo/WindowLocalDescriptorRepo.hpp"
#include "vulkan_helper/engine/wrapped/DescriptorTemplate.hpp"
#include "vulkan_helper/handle/Handler.hpp"
#include "vulkan_helper/util/DescriptorUpdater.hpp"
namespace merutilm::rff2 {

    struct SharedDescriptorStorage : vkh::Handler {

        vkh::Engine &engine;
        vkh::WindowContext &wc;
        vkh::GlobalDescriptorSetLayoutRepo *layoutRepo;
        vkh::WindowLocalDescriptorRepo *descRepo;

        std::unique_ptr<SharedDescriptorManager::DescManagerCamera3D> camera3d;
        std::unique_ptr<SharedDescriptorManager::DescManagerTime> time;
        std::unique_ptr<SharedDescriptorManager::DescManagerIteration> iteration;
        std::unique_ptr<SharedDescriptorManager::DescManagerPalette> palette;
        std::unique_ptr<SharedDescriptorManager::DescManagerStripe> stripe;
        std::unique_ptr<SharedDescriptorManager::DescManagerSlope> slope;
        std::unique_ptr<SharedDescriptorManager::DescManagerColor> color;
        std::unique_ptr<SharedDescriptorManager::DescManagerFog> fog;
        std::unique_ptr<SharedDescriptorManager::DescManagerBloom> bloom;
        std::unique_ptr<SharedDescriptorManager::DescManagerNoiseReduction> noiseReduction;
        std::unique_ptr<SharedDescriptorManager::DescManagerVideo> video;
        std::unique_ptr<SharedDescriptorManager::DescManagerFractal3D> fractal3d;
        std::unique_ptr<SharedDescriptorManager::DescManagerIteration> renderMetaIterationVariant;
        std::unique_ptr<SharedDescriptorManager::DescManagerBatchResult> batchResult;
        std::unique_ptr<SharedDescriptorManager::DescManagerSmoothZoom> smoothZoom;

        explicit SharedDescriptorStorage(vkh::Engine &engine, vkh::WindowContext &wc) : engine(engine), wc(wc) {
            SharedDescriptorStorage::init();
        }

        ~SharedDescriptorStorage() override { SharedDescriptorStorage::cleanup(); }

        SharedDescriptorStorage(const SharedDescriptorStorage &) = delete;
        SharedDescriptorStorage &operator=(const SharedDescriptorStorage &) = delete;
        SharedDescriptorStorage(SharedDescriptorStorage &&) = delete;
        SharedDescriptorStorage &operator=(SharedDescriptorStorage &&) = delete;

    protected:
        void init() override {
            layoutRepo = engine.getGlobalRepositories().getRepository<vkh::GlobalDescriptorSetLayoutRepo>();
            descRepo = wc.getWindowLocalRepositories().getRepository<vkh::WindowLocalDescriptorRepo>();
            using namespace SharedDescriptorTemplate;
            using namespace SharedDescriptorManager;

            auto queue = vkh::DescriptorUpdater::createQueue();

            camera3d = pickAndQueue<DescCamera3D, DescManagerCamera3D>(queue);
            time = pickAndQueue<DescTime, DescManagerTime>(queue);
            iteration = pickAndQueue<DescIteration, DescManagerIteration>(queue);
            palette = pickAndQueue<DescPalette, DescManagerPalette>(queue);
            stripe = pickAndQueue<DescStripe, DescManagerStripe>(queue);
            slope = pickAndQueue<DescSlope, DescManagerSlope>(queue);
            color = pickAndQueue<DescColor, DescManagerColor>(queue);
            fog = pickAndQueue<DescFog, DescManagerFog>(queue);
            bloom = pickAndQueue<DescBloom, DescManagerBloom>(queue);
            noiseReduction = pickAndQueue<DescNoiseReduction, DescManagerNoiseReduction>(queue);
            video = pickAndQueue<DescVideo, DescManagerVideo>(queue);
            fractal3d = pickAndQueue<DescFractal3D, DescManagerFractal3D>(queue);
            renderMetaIterationVariant = pickAndQueue<DescRenderMetaIterationVariant, DescManagerIteration>(queue);
            batchResult = pickAndQueue<DescBatchResult, DescManagerBatchResult>(queue);
            smoothZoom = pickAndQueue<DescSmoothZoom, DescManagerSmoothZoom>(queue);

            vkh::DescriptorUpdater::write(wc.core.getLogicalDevice().getLogicalDeviceHandle(), queue);
        }

        template<vkh::DescTemplateHasID D, typename Ret> requires std::is_constructible_v<Ret, vkh::WindowContext&, vkh::Descriptor &>
        std::unique_ptr<Ret> pickAndQueue(vkh::DescriptorUpdateQueue& queue) const {
            auto desc = &descRepo->pick(vkh::DescriptorTemplate::from<D>(), *layoutRepo);
            for (uint32_t i = 0; i < engine.getCore().getPhysicalDeviceLoader().getMaxFramesInFlight(); i++) {
                desc->queue(queue, i, {}, {});
            }
            return std::make_unique<Ret>(wc, *desc);
        }


        void cleanup() override {
            //noop
        }
    };
} // namespace merutilm::rff2
