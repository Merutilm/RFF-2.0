//
// Created by Merutilm on 8/29/26.
//

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../settings/ShdFractal3DSettings.hpp"
#include "SharedDescriptorTemplate.hpp"
#include "vulkan_helper/engine/wrapped/DescriptorTemplateManager.hpp"
#include "vulkan_helper/util/BufferImageContextUtils.hpp"
#include "vulkan_helper/util/DescriptorUpdater.hpp"

namespace merutilm::rff2::SharedDescriptorManager {

    struct DescManagerCamera3D : vkh::DescriptorTemplateManager {

        using DescriptorTemplateManager::DescriptorTemplateManager;

        void set(const ShdFractal3DSettings &fractal3DSettings) const {
            using namespace SharedDescriptorTemplate;
            auto &cameraUBO = desc.get<vkh::Uniform>(0, DescCamera3D::BINDING_UBO_CAMERA);
            auto &cameraUBOHost = cameraUBO.getHostObject();

            const float altitudeRad = glm::radians(fractal3DSettings.altitude);
            const float rotationRad = glm::radians(fractal3DSettings.rotation);
            const float distance = fractal3DSettings.distance;
            const glm::vec3 cameraPos = {distance * std::cos(altitudeRad) * std::sin(rotationRad),
                                         distance * std::cos(altitudeRad) * -std::cos(rotationRad),
                                         distance * std::sin(altitudeRad)};
            const glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0, 0, 1));

            const auto &[w, h] = wc.getSwapchain().getSwapchainExtent();
            const float fov = 90.f;
            glm::mat4 proj =
                    glm::infinitePerspective(glm::radians(fov), static_cast<float>(w) / static_cast<float>(h), 0.01f);
            proj[1][1] *= -1;

            cameraUBOHost.set<glm::mat4>(DescCamera3D::TARGET_CAMERA_MODEL, glm::mat4{1.0f});
            cameraUBOHost.set<glm::mat4>(DescCamera3D::TARGET_CAMERA_VIEW, view);
            cameraUBOHost.set<glm::mat4>(DescCamera3D::TARGET_CAMERA_PROJ, proj);

            for (uint32_t i = 0; i < wc.core.getPhysicalDeviceLoader().getMaxFramesInFlight(); ++i) {
                cameraUBO.updateMF(i);
            }
        }
    };


    struct DescManagerTime : vkh::DescriptorTemplateManager {
        using DescriptorTemplateManager::DescriptorTemplateManager;

        void setManualTime(const float time, const uint32_t frameIndex) const {
            using namespace SharedDescriptorTemplate;
            auto &timeBinding = desc.get<vkh::Uniform>(0, DescTime::BINDING_UBO_TIME);

            timeBinding.getHostObject().set(DescTime::TARGET_TIME_CURRENT, time);
            timeBinding.updateMF(frameIndex);
        }

        void setToCurrentTime(const uint32_t frameIndex) const { setManualTime(wc.getWindow()->getTime(), frameIndex); }
    };
    struct DescManagerIteration : vkh::DescriptorTemplateManager {

        using DescriptorTemplateManager::DescriptorTemplateManager;
        void cmdRefreshIterations(const vkh::CommandBuffer &commandBuffer, const vkh::BufferContext &src) const {
            vkh::BufferImageContextUtils::cmdCopyBuffer(commandBuffer, src, getResultIterationBuffer());
        }


        [[nodiscard]] const vkh::BufferContext &getResultIterationBuffer() const {
            using namespace SharedDescriptorTemplate;
            auto &iterSSBO = desc.get<vkh::ShaderStorage>(0, DescIteration::BINDING_SSBO_ITERATION_MATRIX);
            return iterSSBO.getBufferContext();
        }

        void resetIterationBuffer(const uint32_t width, const uint32_t height) const {
            using namespace SharedDescriptorTemplate;
            auto &iterUBO = desc.get<vkh::Uniform>(0, DescIteration::BINDING_UBO_ITERATION_INFO);
            auto &iterUBOHost = iterUBO.getHostObject();
            auto &iterSSBO = desc.get<vkh::ShaderStorage>(0, DescIteration::BINDING_SSBO_ITERATION_MATRIX);
            auto &iterSSBOHost = iterSSBO.getHostObject();

            iterUBOHost.set<glm::uvec2>(DescIteration::TARGET_UBO_ITERATION_EXTENT, {width, height});
            iterUBO.update(DescIteration::TARGET_UBO_ITERATION_EXTENT);

            iterSSBOHost.resizeArray<double>(DescIteration::TARGET_SSBO_ITERATION_BUFFER, width * height);
            iterSSBO.reloadBuffer();
            iterSSBO.localize(wc.getCommandPool());

            vkh::DescriptorUpdateQueue queue = vkh::DescriptorUpdater::createQueue();
            for (uint32_t i = 0; i < wc.core.getPhysicalDeviceLoader().getMaxFramesInFlight(); ++i) {
                desc.queue(queue, i, {}, {DescIteration::BINDING_SSBO_ITERATION_MATRIX});
            }
            vkh::DescriptorUpdater::write(wc.core.getLogicalDevice().getLogicalDeviceHandle(), queue);
        }

        void applyMaxIteration() const {
            using namespace SharedDescriptorTemplate;
            const auto &iterUBO = desc.get<vkh::Uniform>(0, DescIteration::BINDING_UBO_ITERATION_INFO);
            iterUBO.update();
        }


        void setMaxIteration(const double maxIteration) const {
            using namespace SharedDescriptorTemplate;
            auto &iterUBO = desc.get<vkh::Uniform>(0, DescIteration::BINDING_UBO_ITERATION_INFO);

            iterUBO.getHostObject().set<double>(DescIteration::TARGET_UBO_ITERATION_MAX, maxIteration);
        }
    };
    struct DescManagerPalette : vkh::DescriptorTemplateManager {
        using DescriptorTemplateManager::DescriptorTemplateManager;

        void set(const ShdPaletteSettings &palette) const {
            using namespace SharedDescriptorTemplate;
            auto &paletteSSBO = desc.get<vkh::ShaderStorage>(0, DescPalette::BINDING_SSBO_PALETTE);
            auto &paletteSSBOHost = paletteSSBO.getHostObject();

            const auto paletteLength = static_cast<uint32_t>(palette.colors.size());

            paletteSSBOHost.set<uint32_t>(DescPalette::TARGET_PALETTE_SIZE, paletteLength);
            paletteSSBOHost.set<float>(DescPalette::TARGET_PALETTE_INTERVAL, palette.iterationInterval);
            paletteSSBOHost.set<double>(DescPalette::TARGET_PALETTE_OFFSET, palette.offsetRatio);
            paletteSSBOHost.set<uint32_t>(DescPalette::TARGET_PALETTE_SMOOTHING,
                                          static_cast<uint32_t>(palette.iterationColoring));
            paletteSSBOHost.set<uint32_t>(DescPalette::TARGET_PALETTE_SINGLE_SMOOTHING,
                                          static_cast<uint32_t>(palette.singleIterationColoring));
            paletteSSBOHost.set<float>(DescPalette::TARGET_PALETTE_ANIMATION_SPEED, palette.animationSpeed);
            paletteSSBOHost.resizeArray<glm::vec4>(DescPalette::TARGET_PALETTE_COLORS, paletteLength);
            paletteSSBOHost.set<glm::vec4>(DescPalette::TARGET_PALETTE_COLORS, palette.colors);
            paletteSSBO.reloadBuffer();
            paletteSSBO.update();
            paletteSSBO.localize(wc.getCommandPool());

            vkh::DescriptorUpdateQueue queue = vkh::DescriptorUpdater::createQueue();
            for (uint32_t i = 0; i < wc.core.getPhysicalDeviceLoader().getMaxFramesInFlight(); ++i) {
                desc.queue(queue, i, {}, {DescPalette::BINDING_SSBO_PALETTE});
            }
            vkh::DescriptorUpdater::write(wc.core.getLogicalDevice().getLogicalDeviceHandle(), queue);
        }
    };
    struct DescManagerStripe : vkh::DescriptorTemplateManager {
        using DescriptorTemplateManager::DescriptorTemplateManager;

        void set(const ShdStripeSettings &stripe) const {
            using namespace SharedDescriptorTemplate;
            auto &stripeUBO = desc.get<vkh::Uniform>(0, DescStripe::BINDING_UBO_STRIPE);
            auto &stripeUBOHost = stripeUBO.getHostObject();
            stripeUBOHost.set(DescStripe::TARGET_STRIPE_TYPE, static_cast<uint32_t>(stripe.stripeType));
            stripeUBOHost.set(DescStripe::TARGET_STRIPE_FIRST_INTERVAL, stripe.firstInterval);
            stripeUBOHost.set(DescStripe::TARGET_STRIPE_SECOND_INTERVAL, stripe.secondInterval);
            stripeUBOHost.set(DescStripe::TARGET_STRIPE_OPACITY, stripe.opacity);
            stripeUBOHost.set(DescStripe::TARGET_STRIPE_OFFSET, stripe.offset);
            stripeUBOHost.set(DescStripe::TARGET_STRIPE_ANIMATION_SPEED, stripe.animationSpeed);
            stripeUBOHost.set(DescStripe::TARGET_STRIPE_ITERATION_COLORING, stripe.iterationColoring);
            stripeUBO.update();
        }
    };
    struct DescManagerSlope : vkh::DescriptorTemplateManager {
        using DescriptorTemplateManager::DescriptorTemplateManager;

        void set(const ShdSlopeSettings &slope, const float depthMultiplier, uint32_t frameIndex) const {
            using namespace SharedDescriptorTemplate;
            auto &slopeUBO = desc.get<vkh::Uniform>(0, DescSlope::BINDING_UBO_SLOPE);
            auto &slopeUBOHost = slopeUBO.getHostObject();
            slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_DEPTH, slope.depth * depthMultiplier);
            slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_REFLECTION_RATIO, slope.reflectionRatio);
            slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_OPACITY, slope.opacity);
            slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_ZENITH, slope.zenith);
            slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_AZIMUTH, slope.azimuth);
            slopeUBO.updateMF(frameIndex);
        }
    };
    struct DescManagerColor : vkh::DescriptorTemplateManager {
        using DescriptorTemplateManager::DescriptorTemplateManager;
        void set(const ShdColorSettings &color) const {
            using namespace SharedDescriptorTemplate;
            auto &colorUBO = desc.get<vkh::Uniform>(0, DescColor::BINDING_UBO_COLOR);
            auto &colorUBOHost = colorUBO.getHostObject();
            colorUBOHost.set<float>(DescColor::TARGET_COLOR_GAMMA, color.gamma);
            colorUBOHost.set<float>(DescColor::TARGET_COLOR_EXPOSURE, color.exposure);
            colorUBOHost.set<float>(DescColor::TARGET_COLOR_HUE, color.hue);
            colorUBOHost.set<float>(DescColor::TARGET_COLOR_SATURATION, color.saturation);
            colorUBOHost.set<float>(DescColor::TARGET_COLOR_BRIGHTNESS, color.brightness);
            colorUBOHost.set<float>(DescColor::TARGET_COLOR_CONTRAST, color.contrast);
            colorUBO.update();
        }
    };
    struct DescManagerFog : vkh::DescriptorTemplateManager {
        using DescriptorTemplateManager::DescriptorTemplateManager;

        void set(const ShdFogSettings &fog) const {
            using namespace SharedDescriptorTemplate;
            auto &fogUBO = desc.get<vkh::Uniform>(0, DescFog::BINDING_UBO_FOG);
            auto &fogUBOHost = fogUBO.getHostObject();
            fogUBOHost.set<float>(DescFog::TARGET_FOG_RADIUS, fog.radius);
            fogUBOHost.set<float>(DescFog::TARGET_FOG_OPACITY, fog.opacity);
            fogUBO.update();
        }
    };
    struct DescManagerBloom : vkh::DescriptorTemplateManager {
        using DescriptorTemplateManager::DescriptorTemplateManager;

        void set(const ShdBloomSettings &bloom) const {
            using namespace SharedDescriptorTemplate;
            auto &bloomUBO = desc.get<vkh::Uniform>(0, DescBloom::BINDING_UBO_BLOOM);
            auto &bloomUBOHost = bloomUBO.getHostObject();

            if (bloomUBO.isLocalized()) {
                bloomUBO.expose(wc.getCommandPool());
            }

            bloomUBOHost.set<float>(DescBloom::TARGET_BLOOM_THRESHOLD, bloom.threshold);
            bloomUBOHost.set<float>(DescBloom::TARGET_BLOOM_RADIUS, bloom.radius);
            bloomUBOHost.set<float>(DescBloom::TARGET_BLOOM_SOFTNESS, bloom.softness);
            bloomUBOHost.set<float>(DescBloom::TARGET_BLOOM_INTENSITY, bloom.intensity);
            bloomUBO.update();
            bloomUBO.localize(wc.getCommandPool());
            vkh::DescriptorUpdateQueue queue = vkh::DescriptorUpdater::createQueue();
            for (uint32_t i = 0; i < wc.core.getPhysicalDeviceLoader().getMaxFramesInFlight(); ++i) {
                desc.queue(queue, i, {}, {DescBloom::BINDING_UBO_BLOOM});
            }
            vkh::DescriptorUpdater::write(wc.core.getLogicalDevice().getLogicalDeviceHandle(), queue);
        }
    };
    struct DescManagerNoiseReduction : vkh::DescriptorTemplateManager {
        using DescriptorTemplateManager::DescriptorTemplateManager;

        void set(const ShdNoiseReduction &noiseReduction) const {
            using namespace SharedDescriptorTemplate;
            auto &noiseUBO = desc.get<vkh::Uniform>(0, DescNoiseReduction::BINDING_UBO_NOISE_REDUCTION);
            auto &interUBOHost = noiseUBO.getHostObject();
            interUBOHost.set<bool>(DescNoiseReduction::TARGET_NOISE_REDUCTION_USE, noiseReduction.use);
            interUBOHost.set<uint32_t>(DescNoiseReduction::TARGET_NOISE_REDUCTION_SIMILAR_COUNT_THRESHOLD,
                                       noiseReduction.similarCountThreshold);
            interUBOHost.set<float>(DescNoiseReduction::TARGET_NOISE_REDUCTION_DIFFERENCE_THRESHOLD,
                                    noiseReduction.differenceThreshold);
            noiseUBO.update();
        }
    };
    struct DescManagerVideo : vkh::DescriptorTemplateManager {
        using DescriptorTemplateManager::DescriptorTemplateManager;

        void setCurrentFrame(const float currentFrame, const uint32_t frameIndex) const {
            using namespace SharedDescriptorTemplate;
            auto &vidUBO = desc.get<vkh::Uniform>(0, DescVideo::BINDING_UBO_VIDEO);
            auto &vidUBOHost = vidUBO.getHostObject();
            vidUBOHost.set<float>(DescVideo::TARGET_VIDEO_CURRENT_FRAME, currentFrame);
            vidUBO.updateMF(frameIndex);
        }


        void setDefaultZoomIncrement(const float defaultZoomIncrement) const {
            using namespace SharedDescriptorTemplate;
            auto &vidUBO = desc.get<vkh::Uniform>(0, DescVideo::BINDING_UBO_VIDEO);
            auto &vidUBOHost = vidUBO.getHostObject();
            vidUBOHost.set<float>(DescVideo::TARGET_VIDEO_DEFAULT_ZOOM_INCREMENT, defaultZoomIncrement);
            for (uint32_t i = 0; i < wc.core.getPhysicalDeviceLoader().getMaxFramesInFlight(); ++i) {
                vidUBO.updateMF(i);
            }
        }
    };
    struct DescManagerFractal3D : vkh::DescriptorTemplateManager {

        using DescriptorTemplateManager::DescriptorTemplateManager;
        void set(const ShdFractal3DSettings &fractal3DSettings) const {
            using namespace SharedDescriptorTemplate;
            auto &f3dUBO = desc.get<vkh::Uniform>(0, DescFractal3D::BINDING_UBO_F3D);
            auto &f3dUBOHost = f3dUBO.getHostObject();

            const float rotationRad = glm::radians(fractal3DSettings.rotation);

            f3dUBOHost.set<float>(DescFractal3D::TARGET_F3D_BASE_ITERATION, fractal3DSettings.baseIteration);
            f3dUBOHost.set<float>(DescFractal3D::TARGET_F3D_DEPTH_DIVISOR, fractal3DSettings.depthDivisor);
            f3dUBOHost.set<float>(DescFractal3D::TARGET_F3D_ROTATION, rotationRad);

            f3dUBO.update();
        }
    };
    struct DescManagerRenderMeta : vkh::DescriptorTemplateManager {
        using DescriptorTemplateManager::DescriptorTemplateManager;
        void set(const FractalSettings &frt, const RenderSettings &render,
                           const std::vector<complex<float>> &reference, const complex<float> offset,
                           const uint32_t maxIteration, const PA<float> *mpTableData, const uint64_t tableLen,
                           const MPAIndexMapper *mapperData, const uint64_t mapperLen,
                           vkh::CommandPool &commandPool) const {

            using namespace SharedDescriptorTemplate;
            auto &rmSSBO = desc.get<vkh::ShaderStorage>(0, DescRenderMeta::BINDING_RM_SSBO);
            auto &rmTableSSBO = desc.get<vkh::ShaderStorage>(0, DescRenderMeta::BINDING_RM_TABLE_SSBO);
            auto &rmMapperSSBO = desc.get<vkh::ShaderStorage>(0, DescRenderMeta::BINDING_RM_MAPPER_SSBO);

            auto &rmSSBOHost = rmSSBO.getHostObject();
            auto &rmTableSSBOHost = rmTableSSBO.getHostObject();
            auto &rmMapperSSBOHost = rmMapperSSBO.getHostObject();

            rmSSBOHost.set<uint64_t>(DescRenderMeta::TARGET_RM_MAX_ITERATION, maxIteration);
            rmSSBOHost.set<uint64_t>(DescRenderMeta::TARGET_RM_MAX_REF_ITERATION, reference.size() - 1);
            rmSSBOHost.set<float>(DescRenderMeta::TARGET_RM_LOG_ZOOM, frt.general.logZoom);
            rmSSBOHost.set<float>(DescRenderMeta::TARGET_RM_BAILOUT, frt.general.bailout);
            rmSSBOHost.set<float>(DescRenderMeta::TARGET_RM_CLARITY_MULTIPLIER, render.clarityMultiplier);
            rmSSBOHost.set<uint32_t>(DescRenderMeta::TARGET_RM_DECIMALIZE_ITERATION_METHOD,
                                     static_cast<uint32_t>(frt.perturb.decimalizeIterationMethod));
            rmSSBOHost.set<complex<float>>(DescRenderMeta::TARGET_RM_OFFSET, static_cast<complex<float>>(offset));
            rmSSBOHost.resizeArray<complex<float>>(DescRenderMeta::TARGET_RM_ORBIT, reference.size());
            rmSSBOHost.set<complex<float>>(DescRenderMeta::TARGET_RM_ORBIT, reference);
            rmTableSSBOHost.set<uint64_t>(DescRenderMeta::TARGET_RM_TABLE_LEN, tableLen);

            rmTableSSBOHost.set<uint32_t>(DescRenderMeta::TARGET_RM_TABLE_SELECTION_METHOD,
                                          static_cast<uint32_t>(frt.mpa.selectionMethod));
            rmTableSSBOHost.resizeArray<PA<float>>(DescRenderMeta::TARGET_RM_TABLE_DATA, tableLen);
            if (tableLen > 0)
                rmTableSSBOHost.set<PA<float>>(DescRenderMeta::TARGET_RM_TABLE_DATA, mpTableData);

            rmMapperSSBOHost.set<uint64_t>(DescRenderMeta::TARGET_RM_MAPPER_LEN, mapperLen);
            rmMapperSSBOHost.resizeArray<MPAIndexMapper>(DescRenderMeta::TARGET_RM_MAPPER_DATA, mapperLen);
            if (mapperLen > 0)
                rmMapperSSBOHost.set<MPAIndexMapper>(DescRenderMeta::TARGET_RM_MAPPER_DATA, mapperData);

            rmSSBO.reloadBuffer();
            rmTableSSBO.reloadBuffer();
            rmMapperSSBO.reloadBuffer();

            rmSSBO.update();
            rmTableSSBO.update();
            rmMapperSSBO.update();

            rmSSBO.localize(commandPool);
            rmTableSSBO.localize(commandPool);
            rmMapperSSBO.localize(commandPool);

            vkh::DescriptorUpdateQueue queue = vkh::DescriptorUpdater::createQueue();
            for (uint32_t i = 0; i < wc.core.getPhysicalDeviceLoader().getMaxFramesInFlight(); ++i) {
                desc.queue(queue, i, {},
                           {DescRenderMeta::BINDING_RM_SSBO, DescRenderMeta::BINDING_RM_TABLE_SSBO,
                            DescRenderMeta::BINDING_RM_MAPPER_SSBO});
            }
            vkh::DescriptorUpdater::write(wc.core.getLogicalDevice().getLogicalDeviceHandle(), queue);
        }

        void resizeWriteBuffer(const uint32_t width, const uint32_t height) const {
            using namespace SharedDescriptorTemplate;

            auto &rmBatchSSBO = desc.get<vkh::ShaderStorage>(0, DescRenderMeta::BINDING_RM_BATCH_SSBO);
            auto &rmBatchSSBOHost = rmBatchSSBO.getHostObject();

            rmBatchSSBOHost.resizeAndClear<ComputeShaderBatchStagingData>(DescRenderMeta::TARGET_RM_BATCH_STAGING_DATA, width * height);

            rmBatchSSBO.reloadBuffer();
            rmBatchSSBO.update();
            rmBatchSSBO.localize(wc.getCommandPool());

            vkh::DescriptorUpdateQueue queue = vkh::DescriptorUpdater::createQueue();
            for (uint32_t i = 0; i < wc.core.getPhysicalDeviceLoader().getMaxFramesInFlight(); ++i) {
                desc.queue(queue, i, {}, {DescRenderMeta::BINDING_RM_BATCH_SSBO});
            }
            vkh::DescriptorUpdater::write(wc.core.getLogicalDevice().getLogicalDeviceHandle(), queue);
        }

        void clearWriteBuffer(vkh::CommandPool &commandPool) const {
            using namespace SharedDescriptorTemplate;

            auto &rmBatchSSBO = desc.get<vkh::ShaderStorage>(0, DescRenderMeta::BINDING_RM_BATCH_SSBO);
            auto &rmBatchSSBOHost = rmBatchSSBO.getHostObject();

            rmBatchSSBOHost.reset(DescRenderMeta::TARGET_RM_BATCH_STAGING_DATA);
            rmBatchSSBO.reloadBuffer();
            rmBatchSSBO.update();
            rmBatchSSBO.localize(commandPool);

            vkh::DescriptorUpdateQueue queue = vkh::DescriptorUpdater::createQueue();
            for (uint32_t i = 0; i < wc.core.getPhysicalDeviceLoader().getMaxFramesInFlight(); ++i) {
                desc.queue(queue, i, {}, {DescRenderMeta::BINDING_RM_BATCH_SSBO});
            }
            vkh::DescriptorUpdater::write(wc.core.getLogicalDevice().getLogicalDeviceHandle(), queue);
        }

        void setBatchSize(const uint32_t batchSize) const {
            using namespace SharedDescriptorTemplate;
            auto &rmBatchInfoUBO = desc.get<vkh::Uniform>(0, DescRenderMeta::BINDING_RM_BATCH_INFO_UBO);
            auto &rmBatchInfoUBOHost = rmBatchInfoUBO.getHostObject();
            rmBatchInfoUBOHost.set<uint32_t>(DescRenderMeta::TARGET_RM_BATCH_SIZE, batchSize);
            rmBatchInfoUBO.update();
        }
    };

    struct DescManagerBatchResult : vkh::DescriptorTemplateManager {
        using DescriptorTemplateManager::DescriptorTemplateManager;


        void resizeBatchResultBuffer(const uint32_t width, const uint32_t height) const {
            using namespace SharedDescriptorTemplate;
            auto &batchResultSSBO = desc.get<vkh::ShaderStorage>(0, DescBatchResult::BINDING_BATCH_RESULT_SSBO);
            auto &batchResultSSBOHost = batchResultSSBO.getHostObject();
            batchResultSSBOHost.resizeArray<uint32_t>(DescBatchResult::BINDING_BATCH_RESULT_SSBO, width * height);
            batchResultSSBO.reloadBuffer();
            batchResultSSBO.update();
            batchResultSSBO.localize(wc.getCommandPool());

            vkh::DescriptorUpdateQueue queue = vkh::DescriptorUpdater::createQueue();
            for (uint32_t i = 0; i < wc.core.getPhysicalDeviceLoader().getMaxFramesInFlight(); ++i) {
                desc.queue(queue, i, {}, {DescBatchResult::BINDING_BATCH_RESULT_SSBO});
            }
            vkh::DescriptorUpdater::write(wc.core.getLogicalDevice().getLogicalDeviceHandle(), queue);
        }

        [[nodiscard]] const vkh::BufferContext &getBatchResultBuffer() const {
            using namespace SharedDescriptorTemplate;
            return desc.get<vkh::ShaderStorage>(0, DescBatchResult::BINDING_BATCH_RESULT_SSBO).getBufferContext();
        }
    };
    struct DescManagerSmoothZoom : vkh::DescriptorTemplateManager {
        using DescriptorTemplateManager::DescriptorTemplateManager;

        void reset() const { set(glm::vec2(0.0f, 0.0f), 0.0f); }

        void set(const glm::vec2 &positionDelta, const float logZoomDelta) const {

            using namespace SharedDescriptorTemplate;
            auto &smoothZoomUBO = desc.get<vkh::Uniform>(0, DescSmoothZoom::BINDING_SMOOTH_ZOOM_UBO);
            vkh::HostDataObject &smoothZoomUBOHost = smoothZoomUBO.getHostObject();
            smoothZoomUBOHost.set(DescSmoothZoom::TARGET_SMOOTH_ZOOM_POSITION_DELTA, positionDelta);
            smoothZoomUBOHost.set(DescSmoothZoom::TARGET_SMOOTH_ZOOM_LOG_ZOOM_DELTA, logZoomDelta);
            smoothZoomUBO.update();
        }
    };


} // namespace merutilm::rff2::SharedDescriptorManager
