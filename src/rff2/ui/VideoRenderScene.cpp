//
// Created by Merutilm on 2025-09-06.
//

#include "VideoRenderScene.hpp"

#include "../../vulkan_helper/executor/ScopedNewCommandBufferExecutor.hpp"
#include "../../vulkan_helper/util/BufferImageContextUtils.hpp"
#include "../vulkan/RCCPresentVid.hpp"
#include "opencv2/imgproc.hpp"

namespace merutilm::rff2 {
    VideoRenderScene::VideoRenderScene(vkh::EngineRef engine, vkh::WindowContextRef wc, const VkExtent2D &videoExtent, const Attribute &targetAttribute) : EngineHandler(engine), wc(wc), videoExtent(videoExtent), targetAttribute(targetAttribute) {
        VideoRenderScene::init();
    }

    VideoRenderScene::~VideoRenderScene() {
        VideoRenderScene::destroy();
    }


    void VideoRenderScene::applyCurrentFrame() const {
        renderer->rendererIteration2Map->setCurrentFrame(currentFrame);
        // rendererStatic2Image->setCurrentFrame(currentFrame);
    }

    void VideoRenderScene::applyCurrentDynamicMap(const RFFDynamicMapBinary &normal,
                                                  const RFFDynamicMapBinary &zoomed, const float currentSec) const {
        auto &normalI = normal.getMatrix();

        if (currentFrame < 1) {
            const auto maxIteration = static_cast<double>(normal.getMaxIteration());
            const std::vector<double> zoomedDefault(normalI.getLength());
            renderer->rendererIteration2Map->setInfo(maxIteration, currentSec);
            renderer->rendererIteration2Map->setAllIterations(normalI.getCanvas(), zoomedDefault);
        } else {
            auto &zoomedI = zoomed.getMatrix();
            const auto maxIteration = static_cast<double>(std::min(zoomed.getMaxIteration(), normal.getMaxIteration()));
            renderer->rendererIteration2Map->setInfo(maxIteration, currentSec);
            renderer->rendererIteration2Map->setAllIterations(normalI.getCanvas(), zoomedI.getCanvas());
        }
    }

    void VideoRenderScene::applyShader() const {
        engine.getCore().getLogicalDevice().waitDeviceIdle();
        renderer->rendererIteration2Map->setPalette(targetAttribute.shader.palette);
        renderer->rendererIteration2Map->set2MapSize(videoExtent);
        renderer->rendererIteration2Map->setDefaultZoomIncrement(targetAttribute.video.dataAttribute.defaultZoomIncrement);
        renderer->rendererStripe->setStripe(targetAttribute.shader.stripe);
        renderer->rendererSlope->setSlope(targetAttribute.shader.slope);
        renderer->rendererColor->setColor(targetAttribute.shader.color);
        renderer->rendererFog->setFog(targetAttribute.shader.fog);
        renderer->rendererBloom->setBloom(targetAttribute.shader.bloom);
        renderer->rendererLinearInterpolation->setLinearInterpolation(targetAttribute.render.linearInterpolation);
        renderer->rendererBoxBlur->setBlurSize(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_FOG, targetAttribute.shader.fog.radius);
        renderer->rendererBoxBlur->
                setBlurSize(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_BLOOM, targetAttribute.shader.bloom.radius);
    }


    void VideoRenderScene::setCurrentFrame(const float currentFrame) {
        this->currentFrame = currentFrame;
    }

    void VideoRenderScene::setStatic(const bool isStatic) {
        this->isStatic = isStatic;
    }

    void VideoRenderScene::setMap(RFFBinary *normal, RFFBinary *zoomed) {
        this->normal = normal;
        this->zoomed = zoomed;
    }

    void VideoRenderScene::applyCurrentStaticImage(const cv::Mat &normal, const cv::Mat &zoomed,
                                                   float currentSec) const {
        // rendererStatic2Image->setImageBuffer(normal, zoomed);
    }

    void VideoRenderScene::initRenderContext() const {

        const auto swapchainImageContextGetter = [this] {
            auto &swapchain = wc.getSwapchain();
            return vkh::ImageContext::fromSwapchain(wc.core, swapchain);
        };

        wc.attachRenderContext<RCC1Vid>(wc.core,
                                     [this] { return videoExtent; },
                                     swapchainImageContextGetter);
        wc.attachRenderContext<RCCDownsampleForBlurVid>(wc.core,
                                                     [this] { return videoExtent; },
                                                     swapchainImageContextGetter);
        wc.attachRenderContext<RCC2Vid>(wc.core,
                                     [this] { return videoExtent; },
                                     swapchainImageContextGetter);
        wc.attachRenderContext<RCC3Vid>(wc.core,
                                     [this] { return videoExtent; },
                                     swapchainImageContextGetter);
        wc.attachRenderContext<RCC4Vid>(wc.core,
                                     [this] { return videoExtent; },
                                     swapchainImageContextGetter);
        wc.attachRenderContext<RCCPresentVid>(wc.core,
                                           [this] { return wc.getSwapchain().populateSwapchainExtent(); },
                                           swapchainImageContextGetter);
    }

    void VideoRenderScene::initRenderer() {
        renderer = std::make_unique<VideoRenderSceneRenderer>(engine, wc.getAttachmentIndex());
        applySize();
        applyShader();
    }

    void VideoRenderScene::applySize() const {
        auto [sWidth, sHeight] = wc.getSwapchain().populateSwapchainExtent();
        auto &[vWidth, vHeight] = videoExtent;

        for (const auto &sp: renderer->configurators) {
            sp->windowResized();
        }

        renderer->rendererDownsampleForBlur->setRescaledResolution(0, {vWidth, vHeight});
        renderer->rendererDownsampleForBlur->setRescaledResolution(1, {vWidth, vHeight});
        renderer->rendererPresent->setRescaledResolution({sWidth, sHeight});
    }

    void VideoRenderScene::refreshSharedImgContext() const {
        using namespace SharedImageContextIndices;
        auto &[w, h] = videoExtent;
        auto &sharedImg = wc.getSharedImageContext();
        sharedImg.cleanupContexts();

        auto iiiGetter = [w, h]( const VkFormat format, const VkImageUsageFlags usage) {
            return vkh::ImageInitInfo{
                .imageType = VK_IMAGE_TYPE_2D,
                .imageViewType = VK_IMAGE_VIEW_TYPE_2D,
                .imageFormat = format,
                .extent = VkExtent3D{w, h, 1},
                .useMipmap = VK_FALSE,
                .arrayLayers = 1,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .imageTiling = VK_IMAGE_TILING_OPTIMAL,
                .usage = usage,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            };
        };

        sharedImg.appendMultiframeImageContext(MF_VIDEO_RENDER_IMAGE_PRIMARY,
                                               iiiGetter(VK_FORMAT_R16G16B16A16_UNORM,
                                                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                         VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
                                                         VK_IMAGE_USAGE_SAMPLED_BIT));
        sharedImg.appendMultiframeImageContext(MF_VIDEO_RENDER_IMAGE_SECONDARY,
                                               iiiGetter( VK_FORMAT_R16G16B16A16_UNORM,
                                                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                         VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
                                                         VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                                         VK_IMAGE_USAGE_SAMPLED_BIT |
                                                        VK_IMAGE_USAGE_STORAGE_BIT));
        sharedImg.appendMultiframeImageContext(MF_VIDEO_RENDER_DOWNSAMPLED_IMAGE_PRIMARY,
                                               iiiGetter(VK_FORMAT_R8G8B8A8_UNORM,
                                                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                         VK_IMAGE_USAGE_SAMPLED_BIT |
                                                         VK_IMAGE_USAGE_STORAGE_BIT));
        sharedImg.appendMultiframeImageContext(MF_VIDEO_RENDER_DOWNSAMPLED_IMAGE_SECONDARY,
                                               iiiGetter(VK_FORMAT_R8G8B8A8_UNORM,
                                                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                         VK_IMAGE_USAGE_SAMPLED_BIT |
                                                         VK_IMAGE_USAGE_STORAGE_BIT));
    }


    void VideoRenderScene::renderOnce() const {
        engine.getCore().getLogicalDevice().waitDeviceIdle();
        renderer->execute();
    }

    float VideoRenderScene::calculateZoom(const float defaultZoomIncrement) const {
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
        //frame size : f1 = 1x, f2 = 2x
        const float r = static_cast<float>(f2) - currentFrame;

        if (!zoomed->hasData() || !normal->hasData()) {
            return 0;
        }

        const float z1 = zoomed->getLogZoom();
        const float z2 = normal->getLogZoom();
        return std::lerp(z2, z1, r);
    }


    cv::Mat VideoRenderScene::generateImage() const {
        wc.core.getLogicalDevice().waitDeviceIdle();

        const auto &imgCtx = wc.getSharedImageContext().getImageContextMF(
            SharedImageContextIndices::MF_VIDEO_RENDER_IMAGE_SECONDARY)[renderer->getFrameIndex()];

        vkh::BufferContext bufCtx = vkh::BufferContext::createContext(wc.core, {
                                                                          .size = imgCtx.capacity,
                                                                          .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                                          .properties =
                                                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                                      });
        vkh::BufferContext::mapMemory(wc.core, bufCtx);
        //NEW COMMAND BUFFER
        {
            const auto executor = vkh::ScopedNewCommandBufferExecutor(wc.core, wc.getCommandPool());
            vkh::BarrierUtils::cmdImageMemoryBarrier(executor.getCommandBufferHandle(), imgCtx.image,
                                                     VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT,
                                                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 0, 1,
                                                     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                     VK_PIPELINE_STAGE_TRANSFER_BIT);
            vkh::BufferImageContextUtils::cmdCopyImageToBuffer(executor.getCommandBufferHandle(), imgCtx, bufCtx);
        }
        vkh::BufferContext::unmapMemory(wc.core, bufCtx);
        auto currentImage = cv::Mat(static_cast<int>(videoExtent.height), static_cast<int>(videoExtent.width), CV_16UC4, bufCtx.mappedMemory);
        cv::cvtColor(currentImage, currentImage, cv::COLOR_RGBA2BGRA);
        vkh::BufferContext::destroyContext(wc.core, bufCtx);
        return currentImage;
    }

    void VideoRenderScene::init() {
        refreshSharedImgContext();
        initRenderContext();
        initRenderer();
    }

    void VideoRenderScene::destroy() {
        renderer = nullptr;
    }
}
