//
// Created by Merutilm on 2025-07-15.
//

#pragma once
#include "../core/vkh_base.hpp"

namespace merutilm::vkh {
    struct Vertex {
        glm::vec3 position = {};
        glm::vec3 color = {};
        glm::vec2 texcoord = {};

        static Vertex generate(const glm::vec3 &position, const glm::vec3 &color, const glm::vec2 &texcoord) {
            return Vertex{glm::vec3{position.x, -position.y, position.z}, color, texcoord};
        }
    };
}
