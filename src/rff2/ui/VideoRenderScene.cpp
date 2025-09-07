//
// Created by Merutilm on 2025-09-06.
//

#include "VideoRenderScene.hpp"

#include "../../vulkan_helper/executor/ScopedCommandBufferExecutor.hpp"

namespace merutilm::rff2 {
    VideoRenderScene::VideoRenderScene(vkh::WindowContextRef wc) : WindowContextHandler(wc) {
        VideoRenderScene::init();
    }

    VideoRenderScene::~VideoRenderScene() {
        VideoRenderScene::destroy();
    }


    void VideoRenderScene::applyCurrentFrame() const {
        shaderPrograms->rendererIteration2Map->setCurrentFrame(currentFrame);
        // rendererStatic2Image->setCurrentFrame(currentFrame);
    }

    void VideoRenderScene::applyCurrentDynamicMap(const RFFDynamicMapBinary &normal,
                                                  const RFFDynamicMapBinary &zoomed, const float currentSec) const {
        auto &normalI = normal.getMatrix();

        if (currentFrame < 1) {
            const auto maxIteration = static_cast<double>(normal.getMaxIteration());
            const std::vector<double> zoomedDefault(normalI.getLength());
            shaderPrograms->rendererIteration2Map->setInfo(normalI.getWidth(), normalI.getHeight(), maxIteration,
                                                           currentSec);
            shaderPrograms->rendererIteration2Map->setAllIterations(normalI.getCanvas(), normalI.getCanvas());
        } else {
            auto &zoomedI = zoomed.getMatrix();
            const auto maxIteration = static_cast<double>(std::min(zoomed.getMaxIteration(), normal.getMaxIteration()));
            shaderPrograms->rendererIteration2Map->setInfo(normalI.getWidth(), normalI.getHeight(), maxIteration,
                                                           currentSec);
            shaderPrograms->rendererIteration2Map->setAllIterations(normalI.getCanvas(), zoomedI.getCanvas());
        }
    }

    void VideoRenderScene::applyShader(const Attribute &attr) const {
        vkDeviceWaitIdle(wc.core.getLogicalDevice().getLogicalDeviceHandle());
        shaderPrograms->rendererBoxBlur->setBlurSize(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_FOG, attr.shader.fog.radius);
        shaderPrograms->rendererBoxBlur->
                setBlurSize(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_BLOOM, attr.shader.bloom.radius);
    }

    void VideoRenderScene::draw(const uint32_t swapchainImageIndex) const {
        vkh::DescriptorUpdateQueue queue = vkh::DescriptorUpdater::createQueue();
        const VkDevice device = wc.core.getLogicalDevice().getLogicalDeviceHandle();

        for (const auto &shaderProgram: shaderPrograms->configurator) {
            shaderProgram->updateQueue(queue, frameIndex);
        }

        vkh::DescriptorUpdater::write(device, queue);

        const VkFence renderFence = wc.getSyncObject().getFence(frameIndex).getFenceHandle();
        const VkSemaphore imageAvailableSemaphore = wc.getSyncObject().getSemaphore(frameIndex).
                getImageAvailable();
        const VkSemaphore renderFinishedSemaphore = wc.getSyncObject().getSemaphore(frameIndex).
                getRenderFinished();
        const VkCommandBuffer cbh = wc.getCommandBuffer().getCommandBufferHandle(frameIndex);
        const auto mfg = [this](const uint32_t index) {
            return wc.getSharedImageContext().getImageContextMF(index)[frameIndex].image;
        };
    }

    // void VideoRenderScene::configure(const HWND wnd, const HDC hdc, const HGLRC context) {
    //     // WGLScene::configure(wnd, hdc, context);
    //     // makeContextCurrent();
    //
    //     // rendererStatic = std::make_unique<GLMultipassRenderer>();
    //     // rendererStatic2Image = std::make_unique<GLRendererStatic2Image>();
    //     //
    //     // rendererStatic->add(*rendererStatic2Image);
    //     //
    //     // renderer = std::make_unique<GLMultipassRenderer>();
    //     // rendererShdIteration2Map = std::make_unique<GLRendererShdIteration2Map>();
    //     // rendererColIteration2Map = std::make_unique<GLRendererColIteration2Map>();
    //     // rendererStripe = std::make_unique<GLRendererStripe>();
    //     // rendererSlope = std::make_unique<GLRendererSlope>();
    //     // rendererColor = std::make_unique<GLRendererColor>();
    //     // rendererFog = std::make_unique<GLRendererFog>();
    //     // rendererBloom = std::make_unique<GLRendererBloom>();
    //     // rendererAntialiasing = std::make_unique<GLRendererAntialiasing>();
    //     //
    //     // renderer->add(*rendererShdIteration2Map);
    //     // renderer->add(*rendererColIteration2Map);
    //     // renderer->add(*rendererStripe);
    //     // renderer->add(*rendererSlope);
    //     // renderer->add(*rendererColor);
    //     // renderer->add(*rendererFog);
    //     // renderer->add(*rendererBloom);
    //     // renderer->add(*rendererAntialiasing);
    // }

    // GLMultipassRenderer &VideoRenderScene::getStaticRenderer() const {
    //     return *rendererStatic;
    // }
    //
    // GLMultipassRenderer &VideoRenderScene::getDynamicRenderer() const {
    //     return *renderer;
    // }

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

    void VideoRenderScene::reloadSize(const uint16_t cw, const uint16_t ch, const uint16_t iw, const uint16_t ih) {
        // renderer->reloadSize(cw, ch, iw, ih);
        // rendererStatic->reloadSize(cw, ch, iw, ih);
        reloadImageBuffer(iw, ih);
    }


    void VideoRenderScene::reloadImageBuffer(const uint16_t w, const uint16_t h) {
        pixels = std::vector<char>(static_cast<uint32_t>(w) * h * 3);
        currentImage = cv::Mat(h, w, CV_8UC3, pixels.data());
    }


    void VideoRenderScene::renderOnce() {
        vkDeviceWaitIdle(wc.core.getLogicalDevice().getLogicalDeviceHandle());
        uint32_t swapchainImageIndex = 0;
        vkAcquireNextImageKHR(wc.core.getLogicalDevice().getLogicalDeviceHandle(),
                              wc.getSwapchain().getSwapchainHandle(), UINT64_MAX,
                              wc.getSyncObject().getSemaphore(frameIndex).getImageAvailable(),
                              wc.getSyncObject().getFence(frameIndex).getFenceHandle(),
                              &swapchainImageIndex);

        //
        // GLMultipassRenderer *targetRenderer;
        //
        // if (isStatic) {
        //     targetRenderer = rendererStatic.get();
        // }else {
        //     targetRenderer = renderer.get();
        // }
        //
        // targetRenderer->render();
        // targetRenderer->display();
        // swapBuffers();
        //
        // const GLuint fbo = targetRenderer->getRenderedFBO();
        // const GLuint fboID = targetRenderer->getRenderedFBOTexID();
        // const int w = targetRenderer->getWidth();
        // const int h = targetRenderer->getHeight();
        // GLRenderer::bindFBO(fbo, fboID);
        // glReadPixels(0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, pixels.data());
        // cv::flip(currentImage, currentImage, 0);
        // GLRenderer::unbindFBO(fbo);
    }

    float VideoRenderScene::calculateZoom(const double defaultZoomIncrement) const {
        if (currentFrame < 1) {
            const float r = 1 - currentFrame;

            if (!normal->hasData()) {
                return 0;
            }

            const float z1 = normal->getLogZoom();
            return std::lerp(z1, z1 + std::log10(defaultZoomIncrement), r);
        }
        const int f1 = static_cast<int>(currentFrame); // it is smaller
        const int f2 = f1 + 1;
        //frame size : f1 = 1x, f2 = 2x
        const float r = f2 - currentFrame;

        if (!zoomed->hasData() || !normal->hasData()) {
            return 0;
        }

        const float z1 = zoomed->getLogZoom();
        const float z2 = normal->getLogZoom();
        return std::lerp(z2, z1, r);
    }


    const cv::Mat &VideoRenderScene::getCurrentImage() const {
        return currentImage;
    }

    void VideoRenderScene::init() {
        //noop
    }

    void VideoRenderScene::destroy() {
        shaderPrograms = nullptr;
    }
}
