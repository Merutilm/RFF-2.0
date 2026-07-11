//
// Created by Merutilm on 2025-08-13.
//

#pragma once
#include <vulkan_helper/base/pch.hpp>

namespace merutilm::vkh {
    struct logger {
        logger() = delete;

        template<typename... Args>
        static void log_err_silent(std::format_string<Args...> message, Args &&...args) {
            const std::tm tm = get_tm();
            std::cerr << current_put_time(&tm) << " | " << std::format(message, std::forward<Args>(args)...) << "\n"
                      << std::flush;
        }


        template<typename... Args>
        static void log_err(std::format_string<Args...> message, Args &&...args) {
            log_err_silent(message, std::forward<Args>(args)...);
            std::string fmt = std::format(message, std::forward<Args>(args)...);
#ifdef _WIN32
            MessageBox(nullptr, fmt.data(), "Error", MB_ICONERROR | MB_OK);
#endif
#ifdef __linux__
            gtk_init_check(nullptr, nullptr);
            GtkWidget *dialog = gtk_message_dialog_new(nullptr, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                       "%s", fmt.data());
            gtk_window_set_title(GTK_WINDOW(dialog), "Error");
            gtk_widget_show_all(dialog);
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);

            while (gtk_events_pending())
                gtk_main_iteration();
#endif
        }

        template<typename... Args>
        static void log_warn(std::format_string<Args...> message, Args &&...args) {
            log_err_silent(message, std::forward<Args>(args)...);
            std::string fmt = std::format(message, std::forward<Args>(args)...);
#ifdef _WIN32
            MessageBox(nullptr, fmt.data(), "Warning", MB_ICONWARNING | MB_OK);
#endif
#ifdef __linux__
            gtk_init_check(nullptr, nullptr);
            GtkWidget *dialog = gtk_message_dialog_new(nullptr, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
                                                       "%s", fmt.data());
            gtk_window_set_title(GTK_WINDOW(dialog), "Error");
            gtk_widget_show_all(dialog);
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);

            while (gtk_events_pending())
                gtk_main_iteration();
#endif
        }

        template<typename... Args>
        static void log(std::format_string<Args...> message, Args &&...args) {
            const std::tm tm = get_tm();
            std::cout << current_put_time(&tm) << " | " << std::format(message, std::forward<Args>(args)...) << "\n"
                      << std::flush;
        }

        static std::tm get_tm() {
            const auto t = std::chrono::system_clock::now();
            const auto time = std::chrono::system_clock::to_time_t(t);
            std::tm tm = {};
#ifdef _WIN32
            localtime_s(&tm, &time);
#elif __linux__
            localtime_r(&time, &tm);
#endif
            return tm;
        }

        static std::_Put_time<char> current_put_time(const std::tm *const tm) {
            return std::put_time(tm, "%Y/%m/%d, %H:%M:%S");
        }
    };
} // namespace merutilm::vkh
