//
// Created by Merutilm on 2026-02-06.
//
#pragma once
namespace merutilm::vkh {

    struct WindowInitializerSettings {

        struct Range {
            int min = 0;
            int max = INT_MAX;
            int first = 300;
        };

        float framerate = 60;
        const std::string name = "VulkanHelperApplication";
        const std::string icon;

        Range widthInfo;
        Range heightInfo;
        bool resizable = true;
        bool filedrop = false;

    };
}