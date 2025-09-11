#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <direct.h>
#include "Core/File.h"

#include "Application.h"
#include "imgui_support.h"
#include "../external/imgui/imgui.h"

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
    render::ContextPtr _context = nullptr;

    // Create user directory
    _mkdir(_userDir.c_str());

    std::vector<std::string> args;
    for (int i = 1; i < argc; i++)
    {
        args.push_back(std::string(argv[i]));
    }

    AppInit(_context, _resourceDir, _userDir, args);

    while (!AppShouldQuit())
    {
        AppRender(_context);
    }

    AppShutdown();
    return 0;
}