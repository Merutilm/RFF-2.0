#pragma once

#include <glad.h>
#include "windows.h"
#include <commctrl.h>
#include <cmath>
#include <array>
#include <chrono>
#include <mutex>


using TextureFormat = std::array<GLuint, 3>;

namespace RFF {
    namespace Win32 {
        constexpr short INIT_RENDER_SCENE_WIDTH = 1280;
        constexpr short INIT_RENDER_SCENE_HEIGHT = 720;
        constexpr short INIT_RENDER_SCENE_FPS = 60;
        constexpr short INIT_SETTINGS_WINDOW_WIDTH = 700;
        constexpr short PROGRESS_BAR_HEIGHT = 40;
        constexpr short SETTINGS_INPUT_HEIGHT = 30;
        constexpr short GAP_SETTINGS_INPUT = 15;
        constexpr short SETTINGS_LABEL_WIDTH_DIVISOR = 2;
        constexpr short MAX_AMOUNT_COMBOBOX = 7;
        constexpr auto CLASS_MASTER_WINDOW = "RFF2MW";
        constexpr auto CLASS_SETTINGS_WINDOW = "RFF2SW";
        constexpr auto CLASS_VIDEO_WINDOW = "RFF2VW";
        constexpr auto CLASS_VIDEO_RENDER_WINDOW = "RFF2VS";
        constexpr auto CLASS_RENDER_SCENE = "RFF2RC";
        constexpr auto CLASS_GL_DUMMY = "RFF2GD";
        constexpr auto FONT_DEFAULT = "Segoe UI";
        constexpr short FONT_SIZE = 25;
        constexpr DWORD STYLE_EX_TOOLTIP = WS_EX_TOPMOST;
        constexpr DWORD STYLE_EX_SETTINGS_WINDOW = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
        constexpr DWORD STYLE_SETTINGS_WINDOW = WS_SYSMENU | WS_BORDER;
        constexpr DWORD STYLE_TOOLTIP = WS_POPUP | TTS_NOPREFIX | TTS_BALLOON | TTS_ALWAYSTIP;
        constexpr DWORD STYLE_LABEL = WS_CHILD | WS_VISIBLE | ES_CENTER | SS_NOTIFY;
        constexpr DWORD STYLE_RADIOBUTTON = WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_AUTORADIOBUTTON;
        constexpr DWORD STYLE_CHECKBOX = WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER | BS_CHECKBOX;
        constexpr DWORD STYLE_TEXT_FIELD = WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER | ES_RIGHT | ES_AUTOHSCROLL;
        constexpr DWORD STYLE_COMBOBOX = WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER | ES_CENTER | CBS_DROPDOWNLIST |
                                         WS_VSCROLL;
        constexpr int ID_MENUS = 0x2000;
        constexpr int ID_OPTIONS = 0x1000;
        constexpr int ID_OPTIONS_RADIO = 0x0100;
        constexpr COLORREF COLOR_PROGRESS_BACKGROUND_E = RGB(40, 140, 40);
        constexpr COLORREF COLOR_PROGRESS_BACKGROUND_D = RGB(40, 40, 40);
        constexpr COLORREF COLOR_PROGRESS_TEXT_E = RGB(0, 0, 0);
        constexpr COLORREF COLOR_PROGRESS_TEXT_D = RGB(255, 255, 255);
        constexpr COLORREF COLOR_TEXT_ERROR = RGB(255, 0, 0);
        constexpr COLORREF COLOR_TEXT_EDITED = RGB(100, 0, 0);
        constexpr COLORREF COLOR_TEXT_MODIFIED = RGB(0, 110, 160);
        constexpr COLORREF COLOR_TEXT_DEFAULT = RGB(0, 0, 0);
        constexpr COLORREF COLOR_LABEL_BACKGROUND = RGB(255, 255, 255);
        constexpr auto ICON_DEFAULT_PATH = "../res/icon.ico";
    }


    namespace Render {
        constexpr int EXIT_CHECK_INTERVAL = 256;
        constexpr float ZOOM_MIN = 1.0f;
        constexpr float ZOOM_INTERVAL = 0.235f;
        constexpr float ZOOM_DEADLINE = 290;
        constexpr uint64_t MINIMUM_ITERATION = 100;
        constexpr int AUTOMATIC_ITERATION_MULTIPLIER = 50;
        constexpr uint16_t GAUSSIAN_MAX_WIDTH = 200;
        constexpr int GAUSSIAN_REQUIRES_BOX = 3;
        constexpr double INTENTIONAL_ERROR_DCLMB = 1e16; //DCmax for Locate Mini Brot
        constexpr double INTENTIONAL_ERROR_DCPTB = 1e128; //DCmax for Per Tur Bation
        constexpr double INTENTIONAL_ERROR_REFZERO_POWER = 1024; // multiplier of exp10 when zr, zi is zero
        constexpr int EXP10_ADDITION = 15;
        inline static const unsigned long long INIT_TIME = std::chrono::system_clock::now().time_since_epoch().count();
    }

    namespace Constant {
        constexpr double LOG_2 = 0.693147180559945309417232121458;
        constexpr double LOG_10 = 2.302585092994045684017991454684;
        constexpr double LOG10_2 = 0.301029995663981195213738894724;
    }

    namespace TextureFormats {
        constexpr int INTERNAL = 0;
        constexpr int FORMAT = 1;
        constexpr int TYPE = 2;
        constexpr TextureFormat INT1 = {GL_R32I, GL_RED_INTEGER, GL_INT};
        constexpr TextureFormat INT2 = {GL_RG32I, GL_RG_INTEGER, GL_INT};
        constexpr TextureFormat INT3 = {GL_RGB32I, GL_RGB_INTEGER, GL_INT};
        constexpr TextureFormat INT4 = {GL_RGBA32I, GL_RGBA_INTEGER, GL_INT};
        constexpr TextureFormat FLOAT1 = {GL_R32F, GL_RED, GL_FLOAT};
        constexpr TextureFormat FLOAT2 = {GL_RG32F, GL_RG, GL_FLOAT};
        constexpr TextureFormat FLOAT3 = {GL_RGB32F, GL_RGB, GL_FLOAT};
        constexpr TextureFormat FLOAT4 = {GL_RGBA32F, GL_RGBA, GL_FLOAT};
    }

    namespace Status {
        constexpr int ITERATION_STATUS = 0;
        constexpr int ZOOM_STATUS = 1;
        constexpr int PERIOD_STATUS = 2;
        constexpr int TIME_STATUS = 3;
        constexpr int RENDER_STATUS = 4;
        constexpr int LENGTH = 5;
        constexpr int SET_PROCESS_INTERVAL_MS = 10;
    }

    namespace NullPointer {
        constexpr auto PROCESS_TERMINATED_REFERENCE = nullptr;
    }

    namespace Extension {
        constexpr auto MAP = "rfm";
        constexpr auto VIDEO = "mp4";
    }

    namespace GLConfig {
        constexpr auto SHADER_PATH_PREFIX = "shaders";
        constexpr auto VERTEX_PATH_DEFAULT = "default_vertex.vert";
        constexpr auto MESSAGE_CANNOT_OPEN_FILE = "Error: Could not open file: ";
    }
}
