#include "stdafx.h"

#include "hooking.h"
#include "lua_hook.h"
#include "editor.h"

typedef HRESULT(__stdcall* EndScene_t)(LPDIRECT3DDEVICE9 pDevice);
static EndScene_t EndScene_o = nullptr;

LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;

DWORD* vTable = nullptr;

bool g_ImGuiCaptureEnabled = false;

static HWND    g_hWnd       = nullptr;
static HWND    g_input_hWnd = nullptr;
static WNDPROC WndProc_o    = nullptr;

void toggle_imgui_capture()
{
    g_ImGuiCaptureEnabled = !g_ImGuiCaptureEnabled;
    if (g_ImGuiCaptureEnabled)
    {
        ImGui::GetIO().WantCaptureMouse    = true;
        ImGui::GetIO().WantCaptureKeyboard = true;
    }
    else
    {
        ImGui::GetIO().WantCaptureMouse    = false;
        ImGui::GetIO().WantCaptureKeyboard = false;
    }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT APIENTRY              WndProcHk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_ACTIVATE)
    {
        if (LOWORD(wParam) == WA_INACTIVE)
        {
            // Make the window stay active when unfocused
            return 0;
        }
    }

    if (g_ImGuiCaptureEnabled)
    {
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
        return TRUE;
    }

    return CallWindowProc(WndProc_o, hWnd, uMsg, wParam, lParam);
}

HRESULT __stdcall EndScene_hk(LPDIRECT3DDEVICE9 pDevice)
{
    if (g_pd3dDevice == nullptr && g_input_hWnd != nullptr)
    {
        g_pd3dDevice = pDevice;

        ImGui::CreateContext();

        // Enable input
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

        ImGuiIO& io = ImGui::GetIO();
        // io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        io.DisplaySize.x = 1920.0f;
        io.DisplaySize.y = 1080.0f;

        // Setup style
        ImGui::StyleColorsDark();

        // Setup rendering
        ImGui_ImplDX9_Init(pDevice);
        ImGui_ImplWin32_Init(g_input_hWnd);
        ImGui_ImplDX9_CreateDeviceObjects();
    }

    if (g_pd3dDevice != nullptr)
    {
        static ImGuiLuaEditor editor;

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        editor.Render();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }

    return EndScene_o(pDevice);
}

static HRESULT(__stdcall* SetCooperativeLevel_o)(IDirectInputDevice8*, HWND, DWORD) = nullptr;

// hooked setcooperativelevel
HRESULT __stdcall SetCooperativeLevel_hk(IDirectInputDevice8* pDevice, HWND hWnd, DWORD dwFlags)
{
    std::cout << "SetCooperativeLevel called with flags " << dwFlags << std::endl;

    // if (hWnd != g_hWnd)
    //{
    dwFlags &= ~DISCL_EXCLUSIVE;
    dwFlags |= DISCL_NONEXCLUSIVE;
    //}

    return SetCooperativeLevel_o(pDevice, hWnd, dwFlags);
}

void install_win32_hooks(HWND hWnd)
{
    g_input_hWnd = hWnd;
    WndProc_o    = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WndProcHk);
}

// Create a dummy Direct3D9 device to retrieve the vtable
void init(HWND hWnd)
{
    IDirectInput8* pDI = nullptr;
    if (FAILED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pDI,
                                  nullptr)))
    {
        std::cerr << "Failed to create DirectInput8 object!" << std::endl;
        return;
    }

    IDirectInputDevice8* pDIDevice = nullptr;
    if (FAILED(pDI->CreateDevice(GUID_SysMouse, &pDIDevice, nullptr)))
    {
        std::cerr << "Failed to create DirectInput8 device!" << std::endl;
        pDI->Release();
        return;
    }

    void** dinput8_vtable = *(void***)pDIDevice;
    install_hook(dinput8_vtable[13], &SetCooperativeLevel_hk, (void**)&SetCooperativeLevel_o);

    // Initialize the Direct3D9 object
    auto dummy_d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!dummy_d3d)
    {
        std::cerr << "Failed to create Direct3D9 object!" << std::endl;
        return;
    }

    // Set up the presentation parameters for the dummy device
    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed              = TRUE;
    d3dpp.SwapEffect            = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow         = hWnd;

    // Create a dummy device to get the vtable
    LPDIRECT3DDEVICE9 pDevice = nullptr;

    // Create the dummy Direct3D9 device
    if (FAILED(dummy_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                       &d3dpp, &pDevice)))
    {
        std::cerr << "Failed to create dummy Direct3D9 device!" << std::endl;
        dummy_d3d->Release();
        return;
    }

    // Get the vtable from the dummy device
    vTable = *(DWORD**)pDevice;
    std::cout << "Direct3D9 vtable located at: " << vTable << std::endl;
    std::cout << "EndScene address: " << (void*)vTable[42] << std::endl;

    // Cleanup the dummy device
    pDevice->Release();
    dummy_d3d->Release();
}

void install_dx9_hooks(HWND hWnd)
{
    g_hWnd = hWnd;
    init(hWnd);

    install_hook((void*)vTable[42], &EndScene_hk, (void**)&EndScene_o);
}
