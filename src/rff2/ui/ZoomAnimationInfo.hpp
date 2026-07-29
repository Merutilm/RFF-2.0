//
// Created by Merutilm on 7/29/26.
//

#pragma once
#include <glm/glm.hpp>
namespace merutilm::rff2 {
    struct ZoomAnimationInfo {

        bool animating = false;
        static constexpr float AIM_APPLY_TIME = 0.3f;

        glm::vec2 targetMouseDragOffset = glm::vec2(0.0f, 0.0f);

        glm::vec2 targetMouseZoomOffsetStart = glm::vec2(0.0f, 0.0f);
        glm::vec2 targetMouseZoomOffset = glm::vec2(0.0f, 0.0f);
        glm::vec2 targetMouseZoomOffsetAim = glm::vec2(0.0f, 0.0f);
        float targetLogZoomOffsetStart = 0.0f;
        float targetLogZoomOffset = 0.0f;
        float targetLogZoomOffsetAim = 0.0f;
        float timeAccumulator = 0.0f;


        void reset() {
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

        void update(const float dt) {
            if (!animating) {
                animating = true;
                targetLogZoomOffsetStart = targetLogZoomOffset;
                targetMouseZoomOffsetStart = targetMouseZoomOffset;
            }

            timeAccumulator += dt;

            if (timeAccumulator >= AIM_APPLY_TIME) {
                timeAccumulator = AIM_APPLY_TIME;
                animating = false;
            }
            const float t = timeAccumulator / AIM_APPLY_TIME;
            targetLogZoomOffset = std::lerp(targetLogZoomOffsetStart, targetLogZoomOffsetAim, t);

            const float dz = std::pow(10.0f, targetLogZoomOffsetStart - targetLogZoomOffset);
            const float cz = std::pow(10.0f, targetLogZoomOffsetStart - targetLogZoomOffsetAim);
            const float mt = cz == 1 ? t : (dz - 1) / (cz - 1);

            targetMouseZoomOffset = {
                std::lerp(targetMouseZoomOffsetStart.x, targetMouseZoomOffsetAim.x, mt),
                std::lerp(targetMouseZoomOffsetStart.y, targetMouseZoomOffsetAim.y, mt)
            };

        }
    };
}
