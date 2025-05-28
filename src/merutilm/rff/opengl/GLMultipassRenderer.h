//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <vector>

#include "GLRenderer.h"
#include "GLRendererDisplayer.h"

class GLMultipassRenderer {
    std::vector<GLRenderer*> renderers = std::vector<GLRenderer*>();
    std::unique_ptr<GLRendererDisplayer> displayer = nullptr;
    float timeSec = 0;

public:
    GLMultipassRenderer();

    ~GLMultipassRenderer() = default;

    GLMultipassRenderer(const GLMultipassRenderer &) = delete;

    GLMultipassRenderer &operator=(const GLMultipassRenderer &) = delete;

    GLMultipassRenderer(GLMultipassRenderer &&) = delete;

    GLMultipassRenderer &operator=(GLMultipassRenderer &&) = delete;

    void add(GLRenderer &renderer);

    void reloadSize(int cw, int ch, int iw, int ih) const;

    void setTime(float timeSec);

    void render() const;

    void display() const;
};
