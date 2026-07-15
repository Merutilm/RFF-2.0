//
// Created by Merutilm on 2025-09-06.
//

#include "VideoWindowRenderManager.hpp"

#include "../vulkan/RCCPresentPrepareImgui.hpp"
#include "opencv2/imgproc.hpp"
#include "vulkan_helper/engine/executor/ScopedCommandBufferExecutor.hpp"
#include "vulkan_helper/util/BufferImageContextUtils.hpp"

namespace merutilm::rff2 {
    VideoWindowRenderManager::VideoWindowRenderManager(vkh::Engine &engine, vkh::WindowContext &wc, const VkExtent2D &videoExtent,
                                       const Settings &targetSettings) :
        EngineHandler(engine), wc(wc), videoExtent(videoExtent), targetSettings(targetSettings) {
        VideoWindowRenderManager::init();
    }

    VideoWindowRenderManager::~VideoWindowRenderManager() { VideoWindowRenderManager::cleanup(); }


    void VideoWindowRenderManager::applyCurrentDynamicMap(const RFFDynamicMapBinary &normal, const RFFDynamicMapBinary &zoomed,
                                                  const float currentFrame) const {
        wc.core.getLogicalDevice().waitDeviceIdle();
        auto &normalI = normal.getMatrix();
        if (currentFrame < 1) {
            const std::vector<double> zoomedDefault(normalI.getLength());
            renderer->compute2MapIterationStripe->setAllIterations(normalI.getCanvas(), zoomedDefault);
        } else {
            auto &zoomedI = zoomed.getMatrix();
            renderer->compute2MapIterationStripe->setAllIterations(normalI.getCanvas(), zoomedI.getCanvas());
        }
    }

    void VideoWindowRenderManager::setMaxIterationDynamic(const double maxIteration) const {
        renderer->compute2MapIterationStripe->setInfo(maxIteration);
    }

    void VideoWindowRenderManager::applyShader() const {
        engine.getCore().getLogicalDevice().waitDeviceIdle();
        renderer->compute2MapIterationStripe->setPalette(targetSettings.shader.palette);
        renderer->compute2MapIterationStripe->set2MapSize(videoExtent);
        renderer->compute2MapIterationStripe->setDefaultZoomIncrement(targetSettings.video.data.defaultZoomIncrement);
        renderer->compute2MapIterationStripe->setStripe(targetSettings.shader.stripe);
        renderer->rcc2->slope->setSlope(targetSettings.shader.slope);
        renderer->rcc2->color->setColor(targetSettings.shader.color);
        renderer->rcc3->fog->setFog(targetSettings.shader.fog);
        renderer->rcc4->bloom->setBloom(targetSettings.shader.bloom);
        renderer->rcc5->linearInterpolation->setLinearInterpolation(targetSettings.render.linearInterpolation);
        renderer->computeBoxBlur->setBlurInfo(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_FOG,
                                               targetSettings.shader.fog.radius);
        renderer->computeBoxBlur->setBlurInfo(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_BLOOM,
                                               targetSettings.shader.bloom.radius);
    }

    void VideoWindowRenderManager::setTime(const float currentSec) const { renderer->currentSec = currentSec; }


    void VideoWindowRenderManager::setCurrentFrame(const float currentFrame) const { renderer->currentFrame = currentFrame; }

    void VideoWindowRenderManager::setStatic(const bool isStatic) const { renderer->isStaticImages = isStatic; }

    void VideoWindowRenderManager::setMap(RFFBinary *normal, RFFBinary *zoomed) {
        this->normal = normal;
        this->zoomed = zoomed;
    }

    void VideoWindowRenderManager::applyCurrentStaticImage(const cv::Mat &normal, const cv::Mat &zoomed) const {
        wc.core.getLogicalDevice().waitDeviceIdle();
        renderer->rccStatic2->static2Image->setImages(normal, zoomed);
    }


    void VideoWindowRenderManager::initRenderer() {
        renderer = std::make_unique<VideoWindowRenderer>(engine, wc, videoExtent);
        applySize();
        applyShader();
    }

    void VideoWindowRenderManager::applySize() const {
        auto [sWidth, sHeight] = wc.getSwapchain().getSwapchainExtent();
        auto [bWidth, bHeight] = RendererUtils::getBlurredImageExtent(videoExtent, 1);

        for (const auto &sp: renderer->configurators) {
            sp->renderContextRefreshed();
        }

        renderer->rccDownsample->downsample->setRescaledResolution(GPCDownsampleForBlur::DESC_INDEX_RESAMPLE_IMAGE_FOG, {bWidth, bHeight});
        renderer->rccDownsample->downsample->setRescaledResolution(GPCDownsampleForBlur::DESC_INDEX_RESAMPLE_IMAGE_BLOOM, {bWidth, bHeight});
        renderer->rccPresent->present->setRescaledResolution({sWidth, sHeight});
    }



    void VideoWindowRenderManager::refreshSharedImgContext() const {
        using namespace SharedImageContextIndices;

        auto &sharedImg = wc.getSharedImageContext();
        sharedImg.cleanupContexts();

        auto iiiGetter = [](const VkExtent2D extent, const VkFormat format, const VkImageUsageFlags usage) {
            return vkh::ImageInitInfo{
                    .imageType = VK_IMAGE_TYPE_2D,
                    .imageViewType = VK_IMAGE_VIEW_TYPE_2D,
                    .imageFormat = format,
                    .extent = VkExtent3D{extent.width, extent.height, 1},
                    .useMipmap = VK_FALSE,
                    .arrayLayers = 1,
                    .samples = VK_SAMPLE_COUNT_1_BIT,
                    .imageTiling = VK_IMAGE_TILING_OPTIMAL,
                    .usage = usage,
                    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                    .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            };
        };
        const auto blurredImageExtent = RendererUtils::getBlurredImageExtent(videoExtent, 1);

        sharedImg.appendMultiframeImageContext(
                MF_MAIN_RENDER_IMAGE_PRIMARY,
                iiiGetter(videoExtent, VK_FORMAT_R8G8B8A8_UNORM,
                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
                                  VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT));
        sharedImg.appendMultiframeImageContext(
                MF_MAIN_RENDER_IMAGE_SECONDARY,
                iiiGetter(videoExtent, VK_FORMAT_R8G8B8A8_UNORM,
                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
                                  VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                  VK_IMAGE_USAGE_STORAGE_BIT));
        sharedImg.appendMultiframeImageContext(MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_PRIMARY,
                                               iiiGetter(blurredImageExtent, VK_FORMAT_R8G8B8A8_UNORM,
                                                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                                 VK_IMAGE_USAGE_SAMPLED_BIT |
                                                                 VK_IMAGE_USAGE_STORAGE_BIT));
        sharedImg.appendMultiframeImageContext(MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_SECONDARY,
                                               iiiGetter(blurredImageExtent, VK_FORMAT_R8G8B8A8_UNORM,
                                                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                                 VK_IMAGE_USAGE_SAMPLED_BIT |
                                                                 VK_IMAGE_USAGE_STORAGE_BIT));
    }


    void VideoWindowRenderManager::renderOnce() const {
        renderer->render();
    }

    float VideoWindowRenderManager::calculateZoom(const float defaultZoomIncrement, const float currentFrame) const {
        if (currentFrame < 1) {
            const float r = 1 - currentFrame;

            if (!normal->hasData()) {
                return 0;
            }

            const float z1 = normal->getLogZoom();
            return std::lerp(z1, z1 + std::log10(defaultZoomIncrement), r);
        }
        const auto f1 = static_cast<int>(currentFrame); // it is smaller
        const auto f2 = f1 + 1;
        // frame size : f1 = 1x, f2 = 2x
        const float r = static_cast<float>(f2) - currentFrame;

        if (!zoomed->hasData() || !normal->hasData()) {
            return 0;
        }

        const float z1 = zoomed->getLogZoom();
        const float z2 = normal->getLogZoom();
        return std::lerp(z2, z1, r);
    }


    VideoBufferCache VideoWindowRenderManager::createImage() const {
        const uint32_t frameIndex = renderer->getFrameIndex();
        wc.getSyncObject().getFence(frameIndex).waitAndReset();
        const vkh::BufferContext &srcBuffer = renderer->computeImageRGBA2BGR->getBufferContext(frameIndex);
        vkh::BufferContext dstBuffer =
                vkh::BufferContext::createContext(wc.core, {
                                                                   .size = srcBuffer.bufferSize,
                                                                   .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                                   .properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                           });

        vkh::BufferContext::mapMemory(wc.core, dstBuffer);
        {
            vkh::ScopedCommandBufferExecutor executor(wc, frameIndex,
                                                      wc.getSyncObject().getFence(frameIndex).getFenceHandle(),
                                                      VK_NULL_HANDLE, VK_NULL_HANDLE);
            vkh::BufferImageContextUtils::cmdCopyBuffer(wc.getCommandBuffer().getCommandBufferHandle(frameIndex),
                                                        srcBuffer, dstBuffer);
        }
        wc.getSyncObject().getFence(frameIndex).wait();

        vkh::BufferContext::unmapMemory(wc.core, dstBuffer);
        return VideoBufferCache(wc.core, std::move(dstBuffer), static_cast<int>(videoExtent.width),
                                static_cast<int>(videoExtent.height),
                                calculateZoom(targetSettings.video.data.defaultZoomIncrement, renderer->currentFrame));
    }

    void VideoWindowRenderManager::init() {
        refreshSharedImgContext();
        initRenderer();
    }

    void VideoWindowRenderManager::cleanup() { engine.getCore().getLogicalDevice().waitDeviceIdle(); }
} // namespace merutilm::rff2