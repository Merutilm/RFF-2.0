//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <vector>

#include "GLRenderer.h"

class GLMultipassRenderer {
    std::vector<GLRenderer*> renderers = std::vector<GLRenderer*>();
    float timeSec = 0;

public:
    GLMultipassRenderer() = default;

    ~GLMultipassRenderer() = default;

    GLMultipassRenderer(const GLMultipassRenderer &) = delete;

    GLMultipassRenderer &operator=(const GLMultipassRenderer &) = delete;

    GLMultipassRenderer(GLMultipassRenderer &&) = delete;

    GLMultipassRenderer &operator=(GLMultipassRenderer &&) = delete;



    void add(GLRenderer *renderer);

    void reloadSize(int w, int h) const;

    void setTime(float timeSec);

    void render() const;
};
