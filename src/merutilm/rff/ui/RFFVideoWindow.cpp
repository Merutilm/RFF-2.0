//
// Created by Merutilm on 2025-06-12.
//

#include "RFFVideoWindow.h"

#include "IOUtilities.h"
#include "../io/RFFMap.h"

void RFFVideoWindow::createVideo(Settings settings, const std::filesystem::path &open,
                                 const std::filesystem::path &save) {
    const auto &[defaultZoomIncrement] = settings.videoSettings.dataSettings;
    const auto &[overZoom, showText, mps] = settings.videoSettings.animationSettings;
    const auto &[fps, bitrate] = settings.videoSettings.exportSettings;

    auto frameInterval = (float) (mps / fps);
    RFFMap targetMap = RFFMap::readByID(open, 1);
    if (!targetMap.hasData()) {
        MessageBox(nullptr, "Cannot create video. There is no samples in the directory : Videos", "Export video",
                   MB_ICONERROR | MB_OK);
        return;
    }

    const Matrix<double> &targetMatrix = targetMap.getIterations();
    int imgWidth = targetMatrix.getWidth();
    int imgHeight = targetMatrix.getHeight();

    //
    // FFmpegFrameRecorder recorder = new FFmpegFrameRecorder(save, imgWidth, imgHeight);
    // Frame f = new Frame(imgWidth, imgHeight, Frame.DEPTH_BYTE, 4);
    // avutil.av_log_set_level(avutil.AV_LOG_QUIET);
    // int maxNumber = IOUtilities::fileNameCount(open, RFF::Extension::MAP);
    // double minNumber = -overZoom;
    // float currentFrameNumber = maxNumber;
    // FFmpegLogCallback.set();
    // recorder.setFrameRate(fps);
    // recorder.setVideoQuality(0); // maximum quality
    // recorder.setFormat("mp4");
    // recorder.setVideoBitrate(bitrate);
    // recorder.start();
    // double currentSec = 0;
    // RFFVideoWindow window = new RFFVideoWindow(imgWidth, imgHeight);
    // long startMillis = System.currentTimeMillis();
    // SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss.SSS");
    // sdf.setTimeZone(TimeZone.getTimeZone("GMT"));
    // RFFGLPanel.LOCKER.lock();
    // window.panel.init();
    // window.panel.getRenderer().reloadSize(imgWidth, imgHeight);
    // RFFGLPanel.LOCKER.unlock();
    //
    // while (currentFrameNumber > minNumber) {
    //     currentFrameNumber -= frameInterval;
    //     currentSec += 1 / fps;
    //     RFFMap zoomed = RFFMap::DEFAULT_MAP;
    //     RFFMap normal = RFFMap::DEFAULT_MAP;
    //     if (currentFrameNumber < 1) {
    //         normal = RFFMap::readByID(open, 1);
    //         zoomed = RFFMap::DEFAULT_MAP;
    //     } else {
    //         int f1 = (int) currentFrameNumber;
    //         int f2 = f1 + 1;
    //         zoomed = RFFMap::readByID(open, f1);
    //         normal = RFFMap::readByID(open, f2);
    //     }
    //
    //
    //     RFFGLPanel.LOCKER.lock();
    //     if (!window.isVisible()) {
    //         RFFGLPanel.LOCKER.unlock();
    //         break;
    //     }
    //     window.panel.makeCurrent();
    //     window.panel.setMap(currentFrameNumber, normal, zoomed);
    //     window.panel.applyCurrentMap();
    //     window.panel.applyColor(settings);
    //     window.panel.getRenderer().setTime((float) currentSec);
    //     double zoom = window.panel.calculateZoom(defaultZoomIncrement());
    //     window.panel.render();
    //
    //     RFFGLPanel.LOCKER.unlock();
    //     if (showText()) {
    //         Graphics2D g = window.panel.getImage().createGraphics();
    //         int xg = 20;
    //         int yg = 10;
    //         int size = imgWidth / 40;
    //         int off = size / 15;
    //         g.setFont(new Font(Font.SERIF, Font.BOLD, size));
    //         String zoomStr = String.format("Zoom : %.6fE%d", Math.pow(10, zoom % 1), (int) zoom);
    //         g.setColor(new Color(0, 0, 0, 128));
    //         g.drawString(zoomStr, xg + off, size + yg + off);
    //         g.setColor(new Color(255, 255, 255, 128));
    //         g.drawString(zoomStr, xg, size + yg);
    //     }
    //
    //     window.setImage(window.panel.getImage());
    //
    //     Java2DFrameConverter.copy(window.img, f);
    //     recorder.record(f, avutil.AV_PIX_FMT_ABGR);
    //     double progressRatio = (maxNumber - currentFrameNumber) / (maxNumber + overZoom());
    //     long spent = System.currentTimeMillis() - startMillis;
    //     long remained = (long) ((1 - progressRatio) / progressRatio * spent);
    //
    //     window.panel.repaint();
    //     window.bar.setValue((int) (progressRatio * 10000));
    //     window.bar.setString(
    //         std::format("Processing... {:2f}% [{}]", progressRatio * 100, sdf.format(new Date(remained))));
    // }
    // if (window.isVisible()) {
    //     window.setVisible(false);
    //     window.dispose();
    // }
    MessageBox(nullptr, "Render Finished!", "Success", MB_OK | MB_ICONINFORMATION);
}
