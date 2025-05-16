//
// Created by Merutilm on 2025-05-09.
//

#pragma once

struct GLTimeProvider {
    virtual ~GLTimeProvider() = default;
    virtual float getTime() = 0;
};