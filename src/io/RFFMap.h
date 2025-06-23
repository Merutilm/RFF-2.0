//
// Created by Merutilm on 2025-06-23.
//

#pragma once
#include <filesystem>

namespace merutilm::rff {
    class RFFMap {

        float logZoom;

    public:
        explicit RFFMap(float logZoom);

        virtual ~RFFMap() = default;

        virtual bool hasData() const = 0;

        virtual void exportAsKeyframe(const std::filesystem::path &dir) const = 0;

        virtual void exportFile(const std::filesystem::path &path) const = 0;

        float getLogZoom() const;
    };
}
