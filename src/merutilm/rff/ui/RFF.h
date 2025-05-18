#pragma once

#include <glad.h>
#include "windows.h"
#include <commctrl.h>
#include <cmath>
#include <array>
#include <chrono>

#include "../calc/double_exp.h"

using TextureFormat = std::array<GLuint, 3>;

namespace RFF {
    namespace Win32 {
        constexpr short INIT_RENDER_SCENE_WIDTH = 1280;
        constexpr short INIT_RENDER_SCENE_HEIGHT = 720;
        constexpr short INIT_RENDER_SCENE_FPS = 60;
        constexpr short INIT_SETTINGS_WINDOW_WIDTH = 700;
        constexpr short HEIGHT_SETTINGS_INPUT = 30;
        constexpr short GAP_SETTINGS_INPUT = 15;
        constexpr short SETTINGS_LABEL_WIDTH_DIVISOR = 2;
        constexpr short MAX_AMOUNT_COMBOBOX = 7;
        constexpr auto CLASS_MASTER_WINDOW = "RFF 2.0";
        constexpr auto CLASS_RENDER_SCENE = "RFF 2.0 Renderer";
        constexpr auto CLASS_SETTINGS_WINDOW = "RFF Settings";
        constexpr auto FONT_DEFAULT = "Segoe UI";
        constexpr short FONT_SIZE = 25;
        constexpr DWORD STYLE_EX_TOOLTIP = WS_EX_TOPMOST;
        constexpr DWORD STYLE_EX_SETTINGS_WINDOW = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
        constexpr DWORD STYLE_TOOLTIP = WS_POPUP | TTS_NOPREFIX | TTS_BALLOON | TTS_ALWAYSTIP;
        constexpr DWORD STYLE_SETTINGS_WINDOW = WS_SYSMENU | WS_BORDER;
        constexpr DWORD STYLE_LABEL = WS_CHILD | WS_VISIBLE | ES_CENTER | SS_NOTIFY;
        constexpr DWORD STYLE_RADIOBUTTON = WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_AUTORADIOBUTTON;
        constexpr DWORD STYLE_CHECKBOX = WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER | BS_CHECKBOX;
        constexpr DWORD STYLE_TEXT_FIELD = WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER | ES_RIGHT | ES_AUTOHSCROLL;
        constexpr DWORD STYLE_COMBOBOX = WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER | ES_CENTER | CBS_DROPDOWNLIST |
                                         WS_VSCROLL;
        constexpr int ID_MENUS = 0x2000;
        constexpr int ID_OPTIONS = 0x1000;
        constexpr int ID_OPTIONS_RADIO = 0x0100;
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
        constexpr uint64_t MINIMUM_ITERATION = 3000;
        constexpr int AUTOMATIC_ITERATION_MULTIPLIER = 50;
        constexpr int GAUSSIAN_MAX_WIDTH = 200;
        constexpr int GAUSSIAN_REQUIRES_BOX = 3;
        inline static const unsigned long long INIT_TIME = std::chrono::system_clock::now().time_since_epoch().count();
    }

    namespace Precision {
        constexpr double LOG_2 = 0.693147180559945309417232121458;
        constexpr double LOG_10 = 2.302585092994045684017991454684;
        constexpr double LOG10_2 = 0.301029995663981195213738894724;
        constexpr int DOUBLE_PRECISION = 52;
        constexpr uint64_t SIGNUM_BIT = 0x8000000000000000;
        constexpr uint64_t EXP0_BITS = 0x3ff0000000000000;
        constexpr uint64_t DECIMAL_SIGNUM_BITS = 0x800fffffffffffff;
        constexpr int PRECISION_ADDITION = 15;
        constexpr double EXP_DEADLINE = 295;
        inline static const auto DEX_ZERO = double_exp(0, 0);
        inline static const auto DEX_ONE = double_exp(0, 1);
        inline static const auto DEX_NAN = double_exp(0, NAN);
        inline static const auto DEX_POSITIVE_INFINITY = double_exp(INT_MAX, INFINITY);
        inline static const auto DEX_NEGATIVE_INFINITY = double_exp(INT_MAX, -INFINITY);

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

    namespace ValidCondition {
        constexpr auto POSITIVE_CHAR = [](const char &e) { return e > 0; };
        constexpr auto NEGATIVE_CHAR = [](const char &e) { return e < 0; };
        constexpr auto POSITIVE_CHAR_ZERO = [](const char &e) { return e >= 0; };
        constexpr auto NEGATIVE_CHAR_ZERO = [](const char &e) { return e <= 0; };
        constexpr auto POSITIVE_U_CHAR = [](const unsigned char &e) { return e > 0; };
        constexpr auto ALL_U_CHAR = [](const unsigned char &) { return true; };
        constexpr auto POSITIVE_SHORT = [](const short &e) { return e > 0; };
        constexpr auto NEGATIVE_SHORT = [](const short &e) { return e < 0; };
        constexpr auto POSITIVE_SHORT_ZERO = [](const short &e) { return e >= 0; };
        constexpr auto NEGATIVE_SHORT_ZERO = [](const short &e) { return e <= 0; };
        constexpr auto POSITIVE_U_SHORT = [](const unsigned short &e) { return e > 0; };
        constexpr auto ALL_U_SHORT = [](const unsigned short &) { return true; };
        constexpr auto POSITIVE_INT = [](const int &e) { return e > 0; };
        constexpr auto NEGATIVE_INT = [](const int &e) { return e < 0; };
        constexpr auto POSITIVE_INT_ZERO = [](const int &e) { return e >= 0; };
        constexpr auto NEGATIVE_INT_ZERO = [](const int &e) { return e <= 0; };
        constexpr auto POSITIVE_LONG = [](const long &e) { return e > 0; };
        constexpr auto NEGATIVE_LONG = [](const long &e) { return e < 0; };
        constexpr auto POSITIVE_LONG_ZERO = [](const long &e) { return e >= 0; };
        constexpr auto NEGATIVE_LONG_ZERO = [](const long &e) { return e <= 0; };
        constexpr auto POSITIVE_LONG_LONG = [](const long long &e) { return e > 0; };
        constexpr auto NEGATIVE_LONG_LONG = [](const long long &e) { return e < 0; };
        constexpr auto POSITIVE_LONG_LONG_ZERO = [](const long long &e) { return e >= 0; };
        constexpr auto NEGATIVE_LONG_LONG_ZERO = [](const long long &e) { return e <= 0; };
        constexpr auto POSITIVE_U_LONG = [](const unsigned long &e) { return e > 0; };
        constexpr auto ALL_U_LONG = [](const unsigned long) { return true; };
        constexpr auto POSITIVE_U_LONG_LONG = [](const unsigned long long &e) { return e > 0; };
        constexpr auto ALL_U_LONG_LONG = [](const unsigned long long) { return true; };
        constexpr auto FLOAT_ZERO_TO_ONE = [](const float &e) { return e >= 0 && e <= 1; };
        constexpr auto FLOAT_DEGREE = [](const float &e) { return e >= 0 && e < 360; };
        constexpr auto ALL_FLOAT = [](const float &) { return true; };
        constexpr auto POSITIVE_FLOAT = [](const float &e) { return e > 0; };
        constexpr auto NEGATIVE_FLOAT = [](const float &e) { return e < 0; };
        constexpr auto POSITIVE_FLOAT_ZERO = [](const float &e) { return e >= 0; };
        constexpr auto NEGATIVE_FLOAT_ZERO = [](const float &e) { return e <= 0; };
        constexpr auto DOUBLE_ZERO_TO_ONE = [](const double &e) { return e >= 0 && e <= 1; };
        constexpr auto DOUBLE_DEGREE = [](const double &e) { return e >= 0 && e < 360; };
        constexpr auto ALL_DOUBLE = [](const double &) { return true; };
        constexpr auto POSITIVE_DOUBLE = [](const double &e) { return e > 0; };
        constexpr auto NEGATIVE_DOUBLE = [](const double &e) { return e < 0; };
        constexpr auto POSITIVE_DOUBLE_ZERO = [](const double &e) { return e >= 0; };
        constexpr auto NEGATIVE_DOUBLE_ZERO = [](const double &e) { return e <= 0; };
        constexpr auto POSITIVE_LONG_DOUBLE = [](const long double &e) { return e > 0; };
        constexpr auto NEGATIVE_LONG_DOUBLE = [](const long double &e) { return e < 0; };
        constexpr auto POSITIVE_LONG_DOUBLE_ZERO = [](const long double &e) { return e >= 0; };
        constexpr auto NEGATIVE_LONG_DOUBLE_ZERO = [](const long double &e) { return e <= 0; };
    }

    namespace Callback {
        constexpr auto NOTHING = [] { /*NO CALLBACKS*/ };
    }

    namespace Parser {
        constexpr auto STRING = [](const std::string &s) { return s; };
        constexpr auto CHAR = [](const std::string &s) { return static_cast<char>(std::stoi(s) & 0xFF); };
        constexpr auto U_CHAR = [](const std::string &s) { return static_cast<unsigned char>(std::stoul(s) & 0xFF); };
        constexpr auto SHORT = [](const std::string &s) { return static_cast<short>(std::stoi(s) & 0xFFFF); };
        constexpr auto U_SHORT = [](const std::string &s) { return static_cast<unsigned short>(std::stoul(s) & 0xFFFF); };
        constexpr auto INT = [](const std::string &s) { return std::stoi(s); };
        constexpr auto LONG = [](const std::string &s) { return std::stol(s); };
        constexpr auto LONG_LONG = [](const std::string &s) { return std::stoll(s); };
        constexpr auto U_LONG = [](const std::string &s) { return std::stoul(s); };
        constexpr auto U_LONG_LONG = [](const std::string &s) { return std::stoull(s); };
        constexpr auto FLOAT = [](const std::string &s) { return std::stof(s); };
        constexpr auto DOUBLE = [](const std::string &s) { return std::stod(s); };
        constexpr auto LONG_DOUBLE = [](const std::string &s) { return std::stold(s); };
    }

    namespace Unparser {
        constexpr auto STRING = [](const std::string &s) { return s; };
        constexpr auto CHAR = [](const char &s) { return std::to_string(s); };
        constexpr auto U_CHAR = [](const unsigned char &s) { return std::to_string(s); };
        constexpr auto SHORT = [](const short &s) { return std::to_string(s); };
        constexpr auto U_SHORT = [](const unsigned short &s) { return std::to_string(s); };
        constexpr auto INT = [](const int &s) { return std::to_string(s); };
        constexpr auto LONG = [](const long &s) { return std::to_string(s); };
        constexpr auto LONG_LONG = [](const long long &s) { return std::to_string(s); };
        constexpr auto U_LONG = [](const unsigned long &s) { return std::to_string(s); };
        constexpr auto U_LONG_LONG = [](const unsigned long long &s) { return std::to_string(s); };
        constexpr auto FLOAT = [](const float &s) { return std::to_string(s); };
        constexpr auto DOUBLE = [](const double &s) { return std::to_string(s); };
        constexpr auto LONG_DOUBLE = [](const long double &s) { return std::to_string(s); };
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

    namespace Approximation {
        constexpr int REQUIRED_PERTURBATION = 2;
    }
    namespace Locator {
        constexpr float MINIBROT_LOG_ZOOM_OFFSET = 1.5f;
        constexpr float ZOOM_INCREMENT_LIMIT = 0.01f;
    }

    namespace GLConfig {
        constexpr auto SHADER_PATH_PREFIX = "../shaders/";
        constexpr auto VERTEX_PATH_DEFAULT = "default_vertex";
        constexpr auto VERTEX_PATH_SUFFIX = ".vert";
        constexpr auto FRAGMENT_PATH_SUFFIX = ".frag";
        constexpr auto MESSAGE_CANNOT_OPEN_FILE = "Error: Could not open file: ";
    }

}
