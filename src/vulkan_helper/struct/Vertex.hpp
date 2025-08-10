//
// Created by Merutilm on 2025-07-15.
//

#pragma once
#include <glm/vec3.hpp>

namespace merutilm::mvk {
    struct Vertex {
        glm::vec3 position = {};
        glm::vec3 color = {};
        glm::vec2 texcoord = {};

        static Vertex generate(const glm::vec3 &position, const glm::vec3 &color, const glm::vec2 &texcoord) {
            return Vertex{glm::vec3{position.x, -position.y, position.z}, color, texcoord};
        }
    };
}
