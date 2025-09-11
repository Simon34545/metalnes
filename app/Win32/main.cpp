#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <direct.h>
#include "Core/File.h"

#include "Application.h"
#include "imgui_support.h"
#include "../external/imgui/imgui.h"
#include "render/context.h"

// Global variables
static bool g_shouldQuit = false;
static HWND g_hwnd = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        g_shouldQuit = true;
        PostQuitMessage(0);
        break;
    case WM_CLOSE:
        g_shouldQuit = true;
        DestroyWindow(hWnd);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool CreateMainWindow()
{
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(nullptr);
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = "MetalNESWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
        return false;

    g_hwnd = CreateWindow(
        "MetalNESWindowClass",
        "MetalNES - Transistor Level NES Emulator",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr);

    if (!g_hwnd)
        return false;

    ShowWindow(g_hwnd, SW_SHOWDEFAULT);
    UpdateWindow(g_hwnd);
    return true;
}

void SetCurrentThreadName(const char* threadName)
{
#ifdef _MSC_VER
    // For Visual Studio debugger
    const DWORD MS_VC_EXCEPTION = 0x406D1388;
    #pragma pack(push,8)
    typedef struct tagTHREADNAME_INFO
    {
        DWORD dwType;
        LPCSTR szName;
        DWORD dwThreadID;
        DWORD dwFlags;
    } THREADNAME_INFO;
    #pragma pack(pop)

    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = threadName;
    info.dwThreadID = GetCurrentThreadId();
    info.dwFlags = 0;

    __try
    {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info);
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
    }
#endif
}

int main(int argc, const char *argv[])
{
    std::string _resourceDir = "./";
    std::string _userDir = ".metalnes/";
    
    // Create user directory
    _mkdir(_userDir.c_str());
    
    // Create main window
    if (!CreateMainWindow())
    {
        MessageBox(nullptr, "Failed to create main window", "Error", MB_OK);
        return -1;
    }

    // Create rendering context
    render::ContextPtr _context = render::CreateNullContext();

    std::vector<std::string> args;
    for (int i = 1; i < argc; i++)
    {
        args.push_back(std::string(argv[i]));
    }

    AppInit(_context, _resourceDir, _userDir, args);

    // Main message loop
    MSG msg = {};
    while (!g_shouldQuit && !AppShouldQuit())
    {
        // Process Windows messages
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                g_shouldQuit = true;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        if (!g_shouldQuit)
        {
            AppRender(_context);
            Sleep(16); // ~60 FPS
        }
    }

    AppShutdown();
    return 0;
}