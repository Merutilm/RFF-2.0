//
// Created by Merutilm on 2025-09-06.
//

#include "VideoWindowRenderManager.hpp"

#include "../vulkan/RCCPresentVid.hpp"
#include "opencv2/imgproc.hpp"
#include "vulkan_helper/engine/executor/ScopedCommandBufferExecutor.hpp"
#include "vulkan_helper/util/BufferImageContextUtils.hpp"

namespace merutilm::rff2 {
    VideoWindowRenderManager::VideoWindowRenderManager(vkh::Engine &engine, vkh::WindowContext &wc, vkh::SharedResource &sr, const VkExtent2D &videoExtent,
                                       const Settings &targetSettings) :
        EngineHandler(engine), wc(wc), sr(sr), videoExtent(videoExtent), targetSettings(targetSettings) {
        VideoWindowRenderManager::init();
    }

    VideoWindowRenderManager::~VideoWindowRenderManager() { VideoWindowRenderManager::cleanup(); }


    void VideoWindowRenderManager::applyCurrentDynamicMap(const RFFDynamicMapBinary &normal, const RFFDynamicMapBinary &zoomed,
                                                  const float currentFrame) const {
        wc.core.getLogicalDevice().waitDeviceIdle();
        auto &normalI = normal.getMatrix();
        if (currentFrame < 1) {
            const std::vector<double> zoomedDefault(normalI.getLength());
            renderer->renderer2MapIterationStripe->setAllIterations(normalI.getCanvas(), zoomedDefault);
        } else {
            auto &zoomedI = zoomed.getMatrix();
            renderer->renderer2MapIterationStripe->setAllIterations(normalI.getCanvas(), zoomedI.getCanvas());
        }
    }

    void VideoWindowRenderManager::setMaxIterationDynamic(const double maxIteration) const {
        renderer->renderer2MapIterationStripe->setInfo(maxIteration);
    }

    void VideoWindowRenderManager::applyShader() const {
        engine.getCore().getLogicalDevice().waitDeviceIdle();
        renderer->renderer2MapIterationStripe->setPalette(targetSettings.shader.palette);
        renderer->renderer2MapIterationStripe->set2MapSize(videoExtent);
        renderer->renderer2MapIterationStripe->setDefaultZoomIncrement(targetSettings.video.data.defaultZoomIncrement);
        renderer->renderer2MapIterationStripe->setStripe(targetSettings.shader.stripe);
        renderer->rendererSlope->setSlope(targetSettings.shader.slope);
        renderer->rendererColor->setColor(targetSettings.shader.color);
        renderer->rendererFog->setFog(targetSettings.shader.fog);
        renderer->rendererBloom->setBloom(targetSettings.shader.bloom);
        renderer->rendererLinearInterpolation->setLinearInterpolation(targetSettings.render.linearInterpolation);
        renderer->rendererBoxBlur->setBlurInfo(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_FOG,
                                               targetSettings.shader.fog.radius);
        renderer->rendererBoxBlur->setBlurInfo(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_BLOOM,
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
        renderer->rendererStaticImage->setImages(normal, zoomed);
    }

    void VideoWindowRenderManager::initRenderContext() const {
        const auto swapchainImageContextGetter = [this] {
            auto &swapchain = wc.getSwapchain();
            return vkh::ImageContext::fromSwapchain(wc.core, swapchain);
        };
        wc.attachRenderContext<RCC1Vid>([this] { return videoExtent; }, swapchainImageContextGetter);
        wc.attachRenderContext<RCCDownsampleForBlurVid>([this] { return getBlurredImageExtent(); },
                                                        swapchainImageContextGetter);
        wc.attachRenderContext<RCC2Vid>([this] { return videoExtent; }, swapchainImageContextGetter);
        wc.attachRenderContext<RCC3Vid>([this] { return videoExtent; }, swapchainImageContextGetter);
        wc.attachRenderContext<RCC4Vid>([this] { return videoExtent; }, swapchainImageContextGetter);
        wc.attachRenderContext<RCCPresentVid>([this] { return wc.getSwapchain().populateSwapchainExtent(); },
                                              swapchainImageContextGetter);
        wc.attachRenderContext<RCCStatic2Image>([this] { return videoExtent; }, swapchainImageContextGetter);
    }

    void VideoWindowRenderManager::initRenderer() {
        renderer = std::make_unique<VideoWindowRenderer>(engine, wc, *sr.vertexBufferPP, *sr.indexBufferPP);
        applySize();
        applyShader();
    }

    void VideoWindowRenderManager::applySize() const {
        auto [sWidth, sHeight] = wc.getSwapchain().populateSwapchainExtent();
        auto [bWidth, bHeight] = getBlurredImageExtent();

        for (const auto &sp: renderer->configurators) {
            sp->renderContextRefreshed();
        }

        renderer->rendererDownsampleForBlur->setRescaledResolution(0, {bWidth, bHeight});
        renderer->rendererDownsampleForBlur->setRescaledResolution(1, {bWidth, bHeight});
        renderer->rendererPresent->setRescaledResolution({sWidth, sHeight});
    }

    VkExtent2D VideoWindowRenderManager::getBlurredImageExtent() const {
        if (const float rat = Constants::Fractal::GAUSSIAN_MAX_WIDTH / static_cast<float>(videoExtent.width); rat < 1) {
            return {Constants::Fractal::GAUSSIAN_MAX_WIDTH,
                    static_cast<uint32_t>(static_cast<float>(videoExtent.height) * rat)};
        }
        return videoExtent;
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
        const auto blurredImageExtent = getBlurredImageExtent();

        sharedImg.appendMultiframeImageContext(
                MF_VIDEO_RENDER_IMAGE_PRIMARY,
                iiiGetter(videoExtent, VK_FORMAT_R8G8B8A8_UNORM,
                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
                                  VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT));
        sharedImg.appendMultiframeImageContext(
                MF_VIDEO_RENDER_IMAGE_SECONDARY,
                iiiGetter(videoExtent, VK_FORMAT_R8G8B8A8_UNORM,
                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
                                  VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                  VK_IMAGE_USAGE_STORAGE_BIT));
        sharedImg.appendMultiframeImageContext(MF_VIDEO_RENDER_DOWNSAMPLED_IMAGE_PRIMARY,
                                               iiiGetter(blurredImageExtent, VK_FORMAT_R8G8B8A8_UNORM,
                                                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                                 VK_IMAGE_USAGE_SAMPLED_BIT |
                                                                 VK_IMAGE_USAGE_STORAGE_BIT));
        sharedImg.appendMultiframeImageContext(MF_VIDEO_RENDER_DOWNSAMPLED_IMAGE_SECONDARY,
                                               iiiGetter(blurredImageExtent, VK_FORMAT_R8G8B8A8_UNORM,
                                                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                                 VK_IMAGE_USAGE_SAMPLED_BIT |
                                                                 VK_IMAGE_USAGE_STORAGE_BIT));
    }


    void VideoWindowRenderManager::renderOnce() const {
        renderer->execute();
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
        const vkh::BufferContext &srcBuffer = renderer->rendererImageRGBA2BGR->getBufferContext(frameIndex);
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
        initRenderContext();
        initRenderer();
    }

    void VideoWindowRenderManager::cleanup() { engine.getCore().getLogicalDevice().waitDeviceIdle(); }
} // namespace merutilm::rff2
