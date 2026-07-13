//
// Created by Merutilm on 2025-05-16.
//

#include "FnShader.hpp"
#include "Callback.hpp"
#include "Utilities.h"
#include "imgui.h"

#include "../io/KFRColorLoader.hpp"

namespace merutilm::rff2 {

    void FnShader::palette(RFFApplication &app) {

        static bool enabled = false;

        ImGui::Checkbox("Palette", &enabled);

        if (enabled) {
            ImGui::Begin("Palette");

            auto &[colors, iterationColoring, singleIterationColoring, iterationInterval, offsetRatio, animationSpeed] =
                    app.getSettings().shader.palette;

            if (ImGui::SliderFloat("Iteration Interval", &iterationInterval, 1.0f, 1000.0f)) {
                iterationInterval = std::max(iterationInterval, FLT_MIN);
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Required iterations for the palette to cycle once");


            if (ImGui::TreeNode("Colors")) {
                static int selected = -1;
                static int selectRequest = -1;
                static int page = 0;

                if (selected >= 0) selected = std::clamp(selected, 0, static_cast<int>(colors.size()) - 1);

                const bool addCheck = colors.size() < Constants::Fractal::MAX_PALETTE_LEN;
                const bool removeCheck = selected != -1 && colors.size() > 1;
                const bool clearCheck = colors.size() > 1;

                ImGui::BeginTable("##table", 3);
                ImGui::TableNextColumn();
                if (!addCheck) {
                    ImGui::BeginDisabled();
                }

                if (ImGui::Button("Add", ImVec2(-FLT_MIN, 0))) {
                    colors.emplace_back(1, 1, 1, 1);
                    page = (colors.size() - 1) / Constants::Fractal::PALETTE_COLORS_PER_PAGE;
                    selectRequest = colors.size() - 1;
                    app.getRequests().requestShader();
                }
                if (!addCheck) {
                    ImGui::EndDisabled();
                }
                ImGui::TableNextColumn();
                if (!removeCheck) {
                    ImGui::BeginDisabled();
                }


                if (ImGui::Button("Remove", ImVec2(-FLT_MIN, 0))) {
                    colors.erase(colors.begin() + selected);
                    app.getRequests().requestShader();

                    if (selected >= colors.size())
                        selectRequest = (int) colors.size() - 1;
                }

                if (!removeCheck) {
                    ImGui::EndDisabled();
                }
                ImGui::TableNextColumn();
                if (!clearCheck) {
                    ImGui::BeginDisabled();
                }


                if (ImGui::Button("Clear", ImVec2(-FLT_MIN, 0))) {
                    colors.clear();
                    colors.emplace_back(1, 1, 1, 1);
                    selectRequest = 0;
                    app.getRequests().requestShader();
                }
                if (!clearCheck) {
                    ImGui::EndDisabled();
                }

                ImGui::EndTable();


                const int pageCount = (colors.size() - 1) / Constants::Fractal::PALETTE_COLORS_PER_PAGE + 1;

                if (ImGui::ArrowButton("##prev", ImGuiDir_Left)) {
                    if (ImGui::GetIO().KeyCtrl) {
                        page -= 10;
                    } else {
                        --page;
                    }
                }
                ImGui::SameLine();
                ImGui::Text("Page %d / %d", page + 1, pageCount);
                ImGui::SameLine();

                if (ImGui::ArrowButton("##next", ImGuiDir_Right)) {
                    if (ImGui::GetIO().KeyCtrl) {
                        page += 10;
                    } else {
                        ++page;
                    }
                }
                page = std::clamp(page, 0, pageCount - 1);

                int jump = page + 1;
                ImGui::SetNextItemWidth(200);
                if (ImGui::InputInt("##page", &jump, 0, 0)) {
                    jump = std::clamp(jump, 1, pageCount);
                    page = jump - 1;
                }

                ImGui::Separator();

                ImGui::BeginChild("##ColorList", ImVec2(-FLT_MIN, 150), true);

                const int begin = page * Constants::Fractal::PALETTE_COLORS_PER_PAGE;
                const int end =
                        std::min(begin + Constants::Fractal::PALETTE_COLORS_PER_PAGE, static_cast<int>(colors.size()));

                if (selectRequest >= 0) {
                    selected = -1;
                }
                for (int i = begin; i < end; ++i) {
                    ImGui::PushID(i);

                    glm::vec4 &col = colors[i];

                    float colRaw[] = {col.r, col.g, col.b};
                    if (ImGui::ColorEdit3("##color", colRaw,
                                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {

                        col = {colRaw[0], colRaw[1], colRaw[2], 1};
                        app.getRequests().requestShader();
                    }

                    if (ImGui::IsItemClicked())
                        selected = i;

                    if (selectRequest == i) {
                        selected = selectRequest;
                        selectRequest = -1;
                        ImGui::SetScrollHereY(0.5f);
                    }

                    if (selected == i) {

                        ImDrawList *draw = ImGui::GetWindowDrawList();
                        draw->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 0, 255),
                                      0.0f, 0, 2.0f);
                    }

                    ImGui::SameLine();
                    ImGui::Text("%d", i + 1);

                    ImGui::PopID();
                }
                ImGui::EndChild();
                ImGui::TreePop();
            }


            if (ImGui::Button("Load KFR Palette", ImVec2(-FLT_MIN, 0))) {
                const auto loaded = KFRColorLoader::loadPaletteSettings();
                if (loaded.empty()) {
                    vkh::logger::log_err("No colors found");
                } else {
                    app.getSettings().shader.palette.colors = loaded;
                    app.getRequests().requestShader();
                }
            }


            if (ImGui::SliderFloat("Offset Ratio", &offsetRatio, 0.0f, 1.0f)) {
                offsetRatio = std::clamp(offsetRatio, 0.0f, 1.0f);
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Start offset ratio of cycling palette.");


            if (ImGui::SliderFloat("Animation Speed", &animationSpeed, -10.0f, 10.0f)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Color Animation Speed, The colors' offset(iterations) per second.");


            if (Utilities::imguiDropdown("Iteration Coloring", &iterationColoring)) {
                app.getRequests().requestShader();
            }

            if (Utilities::imguiDropdown("Single Iteration Coloring", &singleIterationColoring)) {
                app.getRequests().requestShader();
            }

            ImGui::End();
        }
    }
    void FnShader::stripe(RFFApplication &app) {

        static bool enabled = false;

        ImGui::Checkbox("Stripe", &enabled);

        if (enabled) {
            ImGui::Begin("Stripe");
            auto &[stripeType, firstInterval, secondInterval, opacity, offset, animationSpeed, iterationColoring] =
                    app.getSettings().shader.stripe;

            if (Utilities::imguiDropdown("Stripe Type", &stripeType)) {
                app.getRequests().requestShader();
            }

            if (ImGui::SliderFloat("Interval 1", &firstInterval, 0.0f, 1000.0f)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the first Stripe Interval");


            if (ImGui::SliderFloat("Interval 2", &secondInterval, 0.0f, 1000.0f)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the second Stripe Interval");


            if (ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the opacity of stripes.");


            if (ImGui::SliderFloat("Offset", &offset, 0.0f, 1000.0f)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Start offset iteration of stripes.");

            if (ImGui::SliderFloat("Animation Speed", &animationSpeed, 0.0f, 10.0f)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the stripe animation speed.");
            if (Utilities::imguiDropdown("Iteration Coloring", &iterationColoring)) {
                app.getRequests().requestShader();
            }


            ImGui::End();
        }
    }
    void FnShader::slope(RFFApplication &app) {
        static bool enabled = false;

        ImGui::Checkbox("Slope", &enabled);

        if (enabled) {
            ImGui::Begin("Slope");
            auto &[depth, reflectionRatio, opacity, zenith, azimuth] = app.getSettings().shader.slope;


            if (ImGui::SliderFloat("Depth", &depth, 0, 300)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the depth of slope.");

            if (ImGui::SliderFloat("Reflection Ratio", &reflectionRatio, 0, 1)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the reflection ratio of the slope. same as minimum brightness.");

            if (ImGui::SliderFloat("Opacity", &opacity, 0, 1)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the opacity of the slope.");

            if (ImGui::SliderFloat("Zenith", &zenith, 0, 360)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the zenith of the slope. 0 ~ 360 value is required.");

            if (ImGui::SliderFloat("Azimuth", &azimuth, 0, 360)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the azimuth of the slope. 0 ~ 360 value is required.");

            ImGui::End();
        }
    }
    void FnShader::color(RFFApplication &app) {

        static bool enabled = false;

        ImGui::Checkbox("Color", &enabled);

        if (enabled) {
            ImGui::Begin("Color");
            auto &[gamma, exposure, hue, saturation, brightness, contrast] = app.getSettings().shader.color;

            if (ImGui::SliderFloat("Gamma", &gamma, 0.5f, 1.5f)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the gamma.");
            if (ImGui::SliderFloat("Exposure", &exposure, -1, 1)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the exposure.");
            if (ImGui::SliderFloat("Hue", &hue, 0, 1)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the hue.");
            if (ImGui::SliderFloat("Saturation", &saturation, -1, 1)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the saturation.");
            if (ImGui::SliderFloat("Brightness", &brightness, -1, 1)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the brightness.");
            if (ImGui::SliderFloat("Contrast", &contrast, -1, 1)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the contrast.");
            ImGui::End();
        }
    }
    void FnShader::fog(RFFApplication &app) {

        static bool enabled = false;

        ImGui::Checkbox("Fog", &enabled);

        if (enabled) {
            ImGui::Begin("Fog");
            auto &[radius, opacity] = app.getSettings().shader.fog;

            ImGui::SliderFloat("Radius", &radius, 0, 1);
            {
                radius = std::clamp(radius, 0.0f, 1.0f);
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the radius of the fog.");
            if (ImGui::SliderFloat("Opacity", &opacity, 0, 1)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the opacity of the fog.");

            ImGui::End();
        }
    }
    void FnShader::bloom(RFFApplication &app) {
        static bool enabled = false;

        ImGui::Checkbox("Bloom", &enabled);

        if (enabled) {
            ImGui::Begin("Bloom");
            auto &[threshold, radius, softness, intensity] = app.getSettings().shader.bloom;

            if (ImGui::SliderFloat("Threshold", &threshold, 0, 1)) {
                threshold = std::clamp(threshold, 0.0f, 1.0f);
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the threshold of the bloom.");
            if (ImGui::SliderFloat("Radius", &radius, 0, 1)) {
                radius = std::clamp(radius, 0.0f, 1.0f);
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the radius of the bloom.");
            if (ImGui::SliderFloat("Softness", &softness, 0, 1)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the softness of the bloom.");
            if (ImGui::SliderFloat("Intensity", &intensity, 0, 1)) {
                app.getRequests().requestShader();
            }
            Utilities::imguiHelpMarker("Sets the intensity of the bloom.");

            ImGui::End();
        }
    }


} // namespace merutilm::rff2
