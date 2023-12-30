#pragma once
#include <d3d9.h>
#define IMGUI_DEFINE_MATH_OPERATORS

#include "../../external/imgui/imgui.h"

namespace llpp::gui
{
    // window boundaries
    constexpr int WIDTH = 720;
    constexpr int HEIGHT = 440;

    inline ImFont* font = nullptr;
    inline bool exit = true;

    // winapi window variables
    inline HWND window = nullptr;
    inline WNDCLASSEXW window_class = {};

    // window movement points
    inline POINTS position = {};

    // directX state variables
    inline PDIRECT3D9 d3d = nullptr;
    inline LPDIRECT3DDEVICE9 device = nullptr;
    inline D3DPRESENT_PARAMETERS present_parameters = {};

    long __stdcall window_process(HWND window, UINT message, WPARAM wide_param,
                                  LPARAM long_param);

    // window creation & destruction
    void create_window(const wchar_t* window_name, const wchar_t* class_name);
    void destroy_window();

    // device creation & destruction
    bool create_device();
    void reset_device();
    void destroy_device();

    // ImGui creation & destruction
    void create_imgui();
    void destroy_imgui();


    void begin_render();
    void end_render();
    void render();
}
