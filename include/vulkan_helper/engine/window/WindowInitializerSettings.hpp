//
// Created by Merutilm on 2026-02-06.
//
#pragma once
namespace merutilm::vkh {

    struct WindowInitializerSettings {

        struct Paddings {
            int top = 0;
            int left = 0;
            int bottom = 0;
            int right = 0;
        };

        struct Range {
            int min = 0;
            int max = INT_MAX;
            int first = 300;
        };

        float framerate = 60;
        const std::wstring name = L"VulkanHelperApplication";
        const std::wstring icon;

        Range widthInfo;
        Range heightInfo;
        bool useMenubar = false;
        bool resizable = true;
        bool filedrop = false;
        Paddings paddings {};

    };
}