#include "gui.h"

#include <iostream>

#include "../../external/imgui/imgui.h"
#include "../../external/imgui/imgui_impl_dx9.h"
#include "../../external/imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND window, UINT message, WPARAM wide_param, LPARAM long_param);


LRESULT CALLBACK WindowProcess(const HWND window, const UINT message,
                               const WPARAM wide_param, const LPARAM long_param)
{
    // let imgui handle the message
    if (ImGui_ImplWin32_WndProcHandler(window, message, wide_param, long_param)) {
        return true;
    }

    switch (message) {
    // resize message
    case WM_SIZE:
    {
        if (llpp::gui::device && wide_param != SIZE_MINIMIZED) {
            llpp::gui::present_parameters.BackBufferWidth = LOWORD(long_param);
            llpp::gui::present_parameters.BackBufferHeight = HIWORD(long_param);
            llpp::gui::reset_device();
        }
        return 0;
    }
    // disables alt application menu
    case WM_SYSCOMMAND:
    {
        if ((wide_param & 0xfff0) == SC_KEYMENU) { return 0; }
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        llpp::gui::position = MAKEPOINTS(long_param);
        return 0;
    }
    case WM_MOUSEMOVE:
    {
        if (wide_param != MK_LBUTTON) { break; }

        const auto points = MAKEPOINTS(long_param);
        auto rect = RECT{};

        GetWindowRect(window, &rect);
        const auto pos = llpp::gui::position;
        rect.left += points.x - pos.x;
        rect.top += points.y - pos.y;

        if (pos.x >= 0 && pos.x <= llpp::gui::WIDTH && pos.y >= 0 && pos.y <= 19) {
            SetWindowPos(window, HWND_TOPMOST, rect.left, rect.top, 0, 0,
                         SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
        }
    }
        return 0;
    }

    return DefWindowProcW(window, message, wide_param, long_param);
}


namespace llpp::gui
{
    void create_window(const wchar_t* window_name, const wchar_t* class_name)
    {
        window_class.cbSize = sizeof(WNDCLASSEXA);
        window_class.style = CS_CLASSDC;
        window_class.lpfnWndProc = WindowProcess;
        window_class.cbClsExtra = 0;
        window_class.cbWndExtra = 0;
        window_class.hInstance = GetModuleHandleA(nullptr);
        window_class.hCursor = nullptr;
        window_class.hbrBackground = nullptr;
        window_class.lpszMenuName = nullptr;
        window_class.lpszClassName = class_name;
        window_class.hIconSm = nullptr;
        window_class.hIcon = static_cast<HICON>(LoadImageW(
            nullptr, L"C:/dev/chaos-clicker/assets/gui/SHARD.ico", IMAGE_ICON, 0, 0,
            LR_LOADFROMFILE | LR_DEFAULTSIZE));

        RegisterClassExW(&window_class);
        window = CreateWindowW(class_name, window_name, WS_POPUP, 100, 100, WIDTH, HEIGHT,
                               nullptr, nullptr, window_class.hInstance, nullptr);
        ShowWindow(window, SW_SHOWNORMAL);
        UpdateWindow(window);
    }

    void destroy_window()
    {
        DestroyWindow(window);
        UnregisterClassW(window_class.lpszClassName, window_class.hInstance);
    }

    bool create_device()
    {
        d3d = Direct3DCreate9(D3D_SDK_VERSION);
        if (!d3d) { return false; }

        ZeroMemory(&present_parameters, sizeof(present_parameters));
        present_parameters.Windowed = TRUE;
        present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        present_parameters.BackBufferFormat = D3DFMT_UNKNOWN;
        present_parameters.EnableAutoDepthStencil = TRUE;
        present_parameters.AutoDepthStencilFormat = D3DFMT_D16;
        present_parameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

        return (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
                                  D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                  &present_parameters, &device) >= 0);
    }

    void reset_device()
    {
        ImGui_ImplDX9_InvalidateDeviceObjects();
        if (device->Reset(&present_parameters) == D3DERR_INVALIDCALL) { IM_ASSERT(0); }
        ImGui_ImplDX9_CreateDeviceObjects();
    }

    void destroy_device()
    {
        if (device) {
            device->Release();
            device = nullptr;
        }

        if (d3d) {
            d3d->Release();
            d3d = nullptr;
        }
    }

    void create_imgui()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        font = io.Fonts->AddFontFromFileTTF("C:/dev/LLPP/LLPP/src/fonts/Ruda-Bold.ttf",
                                            16);

        io.IniFilename = nullptr;

        ImGuiStyle* style = &ImGui::GetStyle();

        style->WindowPadding = ImVec2(15, 15);
        style->WindowRounding = 5.0f;
        style->FramePadding = ImVec2(5, 5);
        style->FrameRounding = 4.0f;
        style->ItemSpacing = ImVec2(12, 8);
        style->ItemInnerSpacing = ImVec2(8, 6);
        style->IndentSpacing = 25.0f;
        style->ScrollbarSize = 15.0f;
        style->ScrollbarRounding = 9.0f;
        style->GrabMinSize = 5.0f;
        style->GrabRounding = 3.0f;

        style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
        style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        style->Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.0f, 0.3f, 0.88f);
        style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
        style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
        style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
        style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
        style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);


        //ImGui::StyleColorsDark();


        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX9_Init(device);
    }

    void destroy_imgui()
    {
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    void begin_render()
    {
        MSG message;

        while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void end_render()
    {
        ImGui::EndFrame();

        device->SetRenderState(D3DRS_ZENABLE, FALSE);
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

        device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                      D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

        if (device->BeginScene() >= 0) {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            device->EndScene();
        }

        const auto result = device->Present(nullptr, nullptr, nullptr, nullptr);
        if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() ==
            D3DERR_DEVICENOTRESET) { reset_device(); }
    }

    inline float test_float = 0.f;
    inline int test_int = 0;

    void render()
    {
        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize({WIDTH, HEIGHT});
        ImGui::Begin("Ling Ling++", &exit,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
        // ImGui::PushFont(font);
        ImGui::SetNextItemWidth(90.0f);
        ImGui::SliderInt("Speed", &test_int, 1, 10);
        ImGui::SameLine();
        // ImGui::PopFont();

        ImGui::SetNextItemWidth(90.0f);
        ImGui::SliderInt("Knockback", &test_int, -100, 100);
        ImGui::End();
    }
}
