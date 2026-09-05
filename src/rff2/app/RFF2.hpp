//
// Created by Merutilm on 2025-08-08.
//

#pragma once
#include <atomic>

#include "../io/RFFDynamicMapBinary.h"
#include "../mb/MB2Perturbator.h"
#include "../mb/MB2RenderData.hpp"
#include "../parallel/BackgroundThreads.h"
#include "../preset/Presets.hpp"
#include "../settings/Settings.h"
#include "../vulkan/desc/SharedDescriptorStorage.hpp"
#include "ComputeShaderRenderManager.hpp"
#include "CursorManager.hpp"
#include "RFF2Renderer.hpp"
#include "UpdateRequests.hpp"
#include "VideoKeyframeProgressInfo.hpp"
#include "VideoProgressInfo.hpp"
#include "ZoomAnimationInfo.hpp"
#include "vulkan_helper/Application.hpp"
#include "vulkan_helper/engine/executor/ScopedCommandBufferExecutor.hpp"

namespace merutilm::rff2 {
    class RFF2 final : public vkh::Application {

        ParallelRenderState state = {};

        Settings settings;
        UpdateRequests requests = {};
        RFF2Renderer *renderer = nullptr;


        std::atomic<bool> canShowPreview = false;

        std::array<std::string, Constants::Status::LENGTH> statusMessages = {};
        std::unique_ptr<MB2RenderDataBase> renderData = nullptr;
        std::unique_ptr<ApproxTableCacheBase> approxTableCache = nullptr;
        std::unique_ptr<CursorManager> cursorManager = nullptr;
        std::unique_ptr<ComputeShaderRenderManager> computeShaderManager = nullptr;

        ZoomAnimationInfo zoomAnimationInfo;
        VideoKeyframeProgressInfo videoKeyframeProgressInfo = {};
        VideoProgressInfo videoProgressInfo = {};
        BackgroundThreads backgroundThreads = BackgroundThreads();

    public:
        explicit RFF2(const vkh::WindowInitializerSettings &wic) : Application(wic), settings(genDefaultSettings()) {}

        ~RFF2() override = default;

        RFF2(const RFF2 &) = delete;

        RFF2 &operator=(const RFF2 &) = delete;

        RFF2(RFF2 &&) = delete;

        RFF2 &operator=(RFF2 &&) = delete;

        void updateMouseInteraction();

        void update();


        static Settings genDefaultSettings();

        [[nodiscard]] complex<dex> offsetConversion(const Settings &s, int px, int py) const;
        [[nodiscard]] std::array<int, 2> iterationBufferConversion(const Settings &s, const complex<dex> &offset) const;
        void moveCursor(int px, int py) const;

        [[nodiscard]] static dex getDivisor(const Settings &settings);

        [[nodiscard]] uint16_t calcIterationBufferWidth(const Settings &s) const;

        [[nodiscard]] uint16_t calcIterationBufferHeight(const Settings &s) const;
        [[nodiscard]] uint16_t getIterationBufferWidth() const;
        [[nodiscard]] uint16_t getIterationBufferHeight() const;


        void addListeners() override;

        void zoom(int16_t px, int16_t py, float logIncrement);

        void applyDefaultSettings();

        void applyCreateImage();

        void invokeUpdaters();

        void applyShaderSettings(const Settings &s) const;

        void refreshResizeParams(VkExtent2D swapchainExtent) const;

        void registerRenderers() override;

        void refreshSharedImgContexts(VkExtent2D extent) override;

        void overwriteMatrixFromMap(const RFFDynamicMapBinary &map) const;

        [[nodiscard]] static std::filesystem::path getBackupLocationPath();

        void saveBackup() const;

        void saveCurrentLocation(const std::filesystem::path &path) const;

        void loadLocation(const std::filesystem::path &path);

        [[nodiscard]] int16_t getMouseXOnIterationBuffer(int mx) const;

        [[nodiscard]] int16_t getMouseYOnIterationBuffer(int my) const;
        void checkBackupLoad();

        void recomputeThreaded();

        void moveCursorToCenter() const;

        void beforeIterationFill(Settings &s) const;

        static void matchSettingsBeforeCreatingRenderData(Settings &s);

        void matchSettingsAfterCreatingRenderData(Settings &s) const;

        bool prepareRenderData(float startTime, Settings &s);


        template<Number Num, Number Other>
        void fillIterationComputeShader(float startTime, const Settings &s);

        void fillIterationMultithreaded(float startTime, const Settings &s);
        bool fillIteration(float startTime, const Settings &s);

        void afterComputeFinally(bool success);


        void setStatusMessage(const int index, const std::string_view &message) {
            statusMessages[index] = std::string("  ").append(message);
        }


        [[nodiscard]] Settings &getSettings() { return settings; }

        [[nodiscard]] ParallelRenderState &getState() { return state; }

        [[nodiscard]] MB2RenderDataBase *getCurrentRenderData() const { return renderData.get(); }

        [[nodiscard]] std::unique_ptr<MB2RenderDataBase> &getCurrentRenderDataOwnRef() { return renderData; }

        [[nodiscard]] std::unique_ptr<ApproxTableCacheBase> *getApproxTableCache() { return &approxTableCache; }

        [[nodiscard]] UpdateRequests &getRequests() { return requests; }


        void setCurrentPerturbator(std::unique_ptr<MB2RenderDataBase> data) { renderData = std::move(data); }

        [[nodiscard]] BackgroundThreads &getBackgroundThreads() { return backgroundThreads; }

        [[nodiscard]] RFFDynamicMapBinary generateMap() const {
            return {renderData->fractalSettings.general.logZoom,
                    renderData->getReference()->longestPeriod(),
                    renderData->fractalSettings.perturb.maxIteration,
                    renderer->visibleIterationBufferContext->getData(),
                    renderer->visibleIterationBufferContext->getWidth(),
                    renderer->visibleIterationBufferContext->getHeight()};
        }


        [[nodiscard]] vkh::WindowContext &getWindowContext() const { return *rootWindowContext; }


        template<typename P>
            requires std::is_base_of_v<Preset, P>
        void applyPreset(P &preset);

        void onStart();

        void initialize();

        void onResize(VkExtent2D newExtent);

        void onQuit();
        void resolveRequests();

        std::unique_ptr<MB2RenderDataBase>
        createAppropriateRenderData(bool computeShader, float logZoomTest, float startTime, const FractalSettings &frt,
                                    dex dcMax, int exp10, uint64_t refInitialCapacity, uint64_t forcedStrictFPGPeriod);


        VideoKeyframeProgressInfo &getKeyframeProgressInfo() { return videoKeyframeProgressInfo; }

        VideoProgressInfo &getVideoProgressInfo() { return videoProgressInfo; }

    protected:
        void renderImGui() override;


    private:
        static void initImGui();
        void renderControlImGui();
        void renderStatusImGui() const;
    };


    template<Number Num, Number Other>
    void RFF2::fillIterationComputeShader(const float startTime, const Settings &s) {
        setStatusMessage(Constants::Status::RENDER_STATUS, "Preparing Render Meta...");

        const uint32_t width = getIterationBufferWidth();
        const uint32_t height = getIterationBufferHeight();
        const VkExtent2D extent{width, height};

        CPCIterate<Num> *target = std::is_same_v<Num, float>
                                          ? dynamic_cast<CPCIterate<Num> *>(renderer->computeIterateFloat)
                                          : dynamic_cast<CPCIterate<Num> *>(renderer->computeIterateFex);
        CPCIterate<Other> *other = std::is_same_v<Other, float>
                                           ? dynamic_cast<CPCIterate<Other> *>(renderer->computeIterateFloat)
                                           : dynamic_cast<CPCIterate<Other> *>(renderer->computeIterateFex);

        vkh::CommandPool &commandPool = *computeShaderManager->commandPool;

        const auto cache = dynamic_cast<ApproxTableCache<Num> *>(approxTableCache.get());
#ifndef NDEBUG
        const auto tableData = cache ? cache->mpaTable.data() : nullptr;
        const auto mapperData = cache ? cache->flattenIndexMapper.data() : nullptr;
#else
        const auto tableData = cache ? cache->mpaTable : nullptr;
        const auto mapperData = cache ? cache->flattenIndexMapper : nullptr;
#endif

        const auto tableLen = cache ? approxTableCache->tableSizeUsed : 0;
        const auto mapperLen = cache ? approxTableCache->mapperSizeUsed : 0;

        other->clearMeta(commandPool);
        target->setMPAIgnore(s.render.computeShader.completelyIgnoreMpa);
        target->setBatchSize(Constants::Render::COMPUTE_SHADER_INIT_BATCH_SIZE);
        target->clearWriteBuffer(commandPool);
        target->setMeta(s.fractal, s.render,
                        dynamic_cast<MB2Reference<Num> *>(renderData->getReference())->refOrbit,
                        static_cast<complex<Num>>(renderData->getPerturbator()->off),
                        s.fractal.perturb.maxIteration, tableData, tableLen, mapperData, mapperLen,
                        commandPool);

        // preparing render meta scope


        if (state.interruptRequested()) {
            return;
        }

        const vkh::BufferContext &iterResultCtx =
                renderer->descriptorStorage->renderMetaIterationVariant->getResultIterationBuffer();
        const vkh::BufferContext &batchResultCtx = renderer->descriptorStorage->batchResult->getBatchResultBuffer();
        computeShaderManager->tryCreateOrResizeTransferDstBuffer(batchResultCtx, iterResultCtx, extent);

        const vkh::BufferContext &dstBatchBuffer = computeShaderManager->dstBatchBuffer;
        const vkh::BufferContext &dstIterBuffer = computeShaderManager->dstIterBuffer;


        std::vector<uint32_t> stagingData(width * height);
        uint64_t glitches = width * height;
        uint64_t currentBatchIteration = 0;
        uint32_t batchSizeMultiplier = 1;
        bool prevIgnoreMpa = s.render.computeShader.completelyIgnoreMpa;

        for (uint32_t i = 0; glitches > s.render.computeShader.allowedGlitchPixelCount; ++i) {

            if (state.interruptRequested()) {
                break;
            }


            computeShaderManager->fence->waitAndReset();

            const auto actualTime = std::chrono::high_resolution_clock::now();

            {
                const vkh::CommandBuffer &commandBuffer = *computeShaderManager->commandBuffer;
                const vkh::Fence &fence = *computeShaderManager->fence;
                const VkCommandBuffer cbh = commandBuffer.getCommandBufferHandle();

                const vkh::ScopedCommandBufferExecutor executor(*rootWindowContext, commandBuffer, fence,
                                                                VK_NULL_HANDLE, VK_NULL_HANDLE);


                target->cmdRender(cbh, 0, {});


                if (s.render.computeShader.interpolateIsolated) {
                    {
                        vkh::ScopedPipelineBarrierRecorder spr(cbh);
                        spr.cmdBufferMemoryBarrier(
                                VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
                                iterResultCtx.buffer, 0, iterResultCtx.bufferSize, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
                        spr.cmdBufferMemoryBarrier(
                                VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
                                batchResultCtx.buffer, 0, batchResultCtx.bufferSize,
                                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
                    }

                    renderer->computeIgnoreIsolated->cmdRender(cbh, 0, {});
                }
                {
                    vkh::ScopedPipelineBarrierRecorder spr(cbh);
                    spr.cmdBufferMemoryBarrier(VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT,
                                               iterResultCtx.buffer, 0, iterResultCtx.bufferSize,
                                               VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

                    spr.cmdBufferMemoryBarrier(VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT,
                                               batchResultCtx.buffer, 0, batchResultCtx.bufferSize,
                                               VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
                }
                vkh::BufferImageContextUtils::cmdCopyBuffer(commandBuffer, iterResultCtx, dstIterBuffer);
                vkh::BufferImageContextUtils::cmdCopyBuffer(commandBuffer, batchResultCtx, dstBatchBuffer);
            }

            computeShaderManager->fence->wait();
            const auto elapsed = std::chrono::duration_cast<std::chrono::duration<float>>(
                    std::chrono::high_resolution_clock::now() - actualTime);

            const float time = rootWindowContext->getWindow()->getTime();
            setStatusMessage(Constants::Status::TIME_STATUS,
                             std::format("Time : {}", Utilities::formatTime(time - startTime)));
            setStatusMessage(Constants::Status::RENDER_STATUS,
                             std::format("Batching... ({:L}, {:L}, {:L}ms)", currentBatchIteration, glitches,
                                         static_cast<uint32_t>(elapsed.count() * 1000)));


            currentBatchIteration += Constants::Render::COMPUTE_SHADER_INIT_BATCH_SIZE * batchSizeMultiplier;

            if (elapsed.count() < settings.render.computeShader.preferredBatchDuration) {
                batchSizeMultiplier *= 2;
                target->setBatchSize(Constants::Render::COMPUTE_SHADER_INIT_BATCH_SIZE * batchSizeMultiplier);
            }
            if (elapsed.count() > settings.render.computeShader.preferredBatchDuration * 2) {
                batchSizeMultiplier = std::max(static_cast<uint32_t>(1), batchSizeMultiplier / 2);
                target->setBatchSize(Constants::Render::COMPUTE_SHADER_INIT_BATCH_SIZE * batchSizeMultiplier);
            }
            bool currIgnoreMpa = settings.render.computeShader.completelyIgnoreMpa ||
                                 (i > settings.render.computeShader.automaticAcceptMpaBatches &&
                                  settings.render.computeShader.automaticAcceptMpaBatches != 0);
            if (prevIgnoreMpa != currIgnoreMpa) {
                batchSizeMultiplier = 1;
                target->setMPAIgnore(currIgnoreMpa);
                target->setBatchSize(Constants::Render::COMPUTE_SHADER_INIT_BATCH_SIZE);
                prevIgnoreMpa = currIgnoreMpa;
            }


            memcpy(stagingData.data(), dstBatchBuffer.mappedMemory, dstBatchBuffer.bufferSize);
            memcpy(renderer->visibleIterationBufferContext->getData().data(), dstIterBuffer.mappedMemory,
                   dstIterBuffer.bufferSize);

            glitches = std::ranges::count_if(stagingData, [](const uint32_t data) { return data != 1; });

            renderer->visibleIterationBufferContext->markUpdate();
            canShowPreview = true;
        } // batching and checking scope
    }


    template<typename P>
        requires std::is_base_of_v<Preset, P>
    void RFF2::applyPreset(P &preset) {
        if constexpr (std::is_base_of_v<Presets::CalculationPreset, P>) {
            if constexpr (std::is_base_of_v<Presets::CalculationPresets::ApproxPreset, P>) {
                settings.fractal.mpa = preset.genMPA();
                requests.requestRecompute();
            }
            if constexpr (std::is_base_of_v<Presets::CalculationPresets::CompressPreset, P>) {
                settings.fractal.mpa = preset.genMPA();
                settings.fractal.reference.compression = preset.genRefComp();
                requests.requestRecompute();
            }
            if constexpr (std::is_base_of_v<Presets::CalculationPresets::ReferenceSyncPreset, P>) {
                settings.fractal.reference.sync = preset.genRefSync();
                requests.requestRecompute();
            }
        }
        if constexpr (std::is_base_of_v<Presets::RenderPreset, P>) {
            if constexpr (std::is_base_of_v<Presets::RenderPresets::DisplayPreset, P>) {
                settings.render.display = preset.genDisplay();
                requests.requestResize(rootWindowContext->getSwapchain().getSwapchainExtent());
                requests.requestRecompute();
            }
            if constexpr (std::is_base_of_v<Presets::RenderPresets::ComputeShaderPreset, P>) {
                settings.render.computeShader = preset.genComputeShader();
            }
        }
        if constexpr (std::is_base_of_v<Presets::ResolutionPreset, P>) {
            auto r = preset.genResolution();
            rootWindowContext->getWindow()->setResolution(r[0], r[1]);
        }
        if constexpr (std::is_base_of_v<Presets::ShaderPreset, P>) {
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::PalettePreset, P>) {
                settings.shader.palette = preset.genPalette();
            }
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::StripePreset, P>) {
                settings.shader.stripe = preset.genStripe();
            }
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::SlopePreset, P>) {
                settings.shader.slope = preset.genSlope();
            }
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::ColorPreset, P>) {
                settings.shader.color = preset.genColor();
            }
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::FogPreset, P>) {
                settings.shader.fog = preset.genFog();
            }
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::BloomPreset, P>) {
                settings.shader.bloom = preset.genBloom();
            }
            requests.requestShader();
        }
    }
} // namespace merutilm::rff2
