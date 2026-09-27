//
// Created by Merutilm on 7/29/26.
//

#pragma once
#include <glm/glm.hpp>
namespace merutilm::rff2 {
    struct ZoomAnimationInfo {

        bool animating = false;
        static constexpr float DURATION = 0.2f;

        glm::vec2 targetMouseDragOffset = glm::vec2(0.0f, 0.0f);
        glm::vec2 targetMouseZoomOffsetStart = glm::vec2(0.0f, 0.0f);
        glm::vec2 targetMouseZoomOffset = glm::vec2(0.0f, 0.0f);
        glm::vec2 targetMouseZoomOffsetAim = glm::vec2(0.0f, 0.0f);

        bool aimChanged = false;
        double targetLogZoomOffsetStart = 0.0f;
        double targetLogZoomOffset = 0.0f;
        double targetLogZoomOffsetAim = 0.0f;
        double timeAccumulator = 0.0f;


        void reset() {
            aimChanged = false;
            animating = false;
            timeAccumulator = 0;
            targetLogZoomOffset = 0;
            targetLogZoomOffsetAim = 0;
            targetMouseZoomOffset = {};
            targetMouseZoomOffsetAim = {};
            targetMouseDragOffset = {};
        }


        void stop() {
            animating = false;
            timeAccumulator = 0;
        }

        void update(const double dt) {
            if (!animating) {
                animating = true;
                targetLogZoomOffsetStart = targetLogZoomOffset;
                targetMouseZoomOffsetStart = targetMouseZoomOffset;
            }

            timeAccumulator += dt;

            if (timeAccumulator >= DURATION) {
                timeAccumulator = DURATION;
                aimChanged = false;
                animating = false;
            }
            const double t = timeAccumulator / DURATION;
            targetLogZoomOffset = std::lerp(targetLogZoomOffsetStart, targetLogZoomOffsetAim, t);

            const double dz = std::pow(10.0, targetLogZoomOffsetStart - targetLogZoomOffset);
            const double cz = std::pow(10.0, targetLogZoomOffsetStart - targetLogZoomOffsetAim);
            const double mt = cz == 1 ? t : (dz - 1) / (cz - 1);

            targetMouseZoomOffset = {
                std::lerp(targetMouseZoomOffsetStart.x, targetMouseZoomOffsetAim.x, mt),
                std::lerp(targetMouseZoomOffsetStart.y, targetMouseZoomOffsetAim.y, mt)
            };

        }
    };
}
