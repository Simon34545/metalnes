
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <queue>
#include <mutex>
#include <algorithm>
#include <iostream>

#include "Application.h"
#include "Core/Log.h"
#include "Core/File.h"
#include "render/context.h"

#include "imgui_support.h"

#include "Core/Path.h"
#include "Core/StopWatch.h"
#include "Core/String.h"
#include "Core/File.h"
#include "metalnes/system.h"
#include "metalnes/nesrom.h"

#include "UnitTests.h"
#include "metalnes/wire_module.h"
#include "metalnes/handler_log.h"

using namespace metalnes;


static std::string s_resource_dir;
static std::string s_data_dir;
static std::string s_system_def_dir;

static std::vector<std::string> s_rom_dirs;
static std::vector<system_state_ptr> _system_list;


std::string getSystemDefDir()
{
    return s_system_def_dir;
}


struct RomInfo
{
    std::string path;
    std::string name;
};

static std::vector<RomInfo> s_rom_list;

static nesrom_ptr load_rom(std::string rom_name)
{
    printf("Searching for ROM: '%s'\n", rom_name.c_str());
    for (auto rom_dir : s_rom_dirs)
    {
        std::string rom_path = Core::Path::Combine(rom_dir, rom_name);
        printf("  Trying: '%s'\n", rom_path.c_str());

        nesrom_ptr rom = nesrom::LoadFromFile(rom_path);
        if (rom) {
            printf("Loaded rom from '%s'\n", rom_path.c_str());
            return rom;
        }
    
    }
    
    return nullptr;
}





bool load_system(std::string rom_name)
{
    nesrom_ptr rom = load_rom(rom_name);
    if (!rom)
    {
        printf("Could not load rom '%s'\n", rom_name.c_str());
        return false;
    }
    

    system_state_ptr system  = system_state::Create(s_system_def_dir, s_data_dir, rom);
    if (!system)
    {
        return false;
    }

    // one system at a time....
    _system_list.clear();

    _system_list.push_back(system);
    return true;
}


static bool _window_open_romselector =  true;
static int s_selected_rom = -1;

void onGuiRomSelector()
{
    
    ImGuiWindowFlags window_flags = 0; //ImGuiWindowFlags_MenuBar;
    //  ImGui::SetNextWindowPos(ImVec2(0,0)); //, ImGuiCond_Always);
    //    ImGui::SetNextWindowContentSize(ImVec2(800,600)); //, ImGuiCond_Always);
    if (!ImGui::Begin("Rom Browser", &_window_open_romselector, window_flags))
    {
        ImGui::End();
        return;
    }
    
    auto &rom_list = s_rom_list;
    
    if (rom_list.empty())
    {
        // list all roms
        for (auto dir : s_rom_dirs)
        {
            std::vector<std::string> files;
            Core::Directory::GetDirectoryFiles(dir, files, true);
            
            std::sort( files.begin(), files.end());

            for (auto file : files)
            {
                if (Core::Path::GetExtensionLower(file) == ".nes")
                {
                    std::string name = file.substr(dir.size());
                    rom_list.push_back( {file, name });
                }
            }
        }
        
    }
    
    if (ImGui::Button("Load"))
    {
        if (s_selected_rom >= 0)
        {
            load_system( rom_list[s_selected_rom].path );
        }
    }
    
    ImGui::Separator();
    
    ImGuiTableFlags flags = ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
//    const float TEXT_BASE_WIDTH = 16.0f;
    ImVec2 size = ImVec2(0,0);
    if (ImGui::BeginTable("##rom_table", 1, flags, size))
    {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
//        ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 1);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None);
        ImGui::TableHeadersRow();


        // Demonstrate using clipper for large vertical lists
        ImGuiListClipper clipper;
        clipper.Begin( (int)rom_list.size() );
        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                bool selected = (s_selected_rom == row);
                if (ImGui::Selectable( rom_list[row].name.c_str(), selected, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SpanAllColumns))
                {
                    s_selected_rom = row;
                    
                    if (ImGui::IsMouseDoubleClicked(0))
                        load_system( rom_list[s_selected_rom].path );

                }
            }
        }
        
        ImGui::EndTable();
    }

    ImGui::End();
    
}


extern bool _demo_window_open;

void AppOnGui(render::ContextPtr context)
{
    // show rom selector
    onGuiRomSelector();
    
//    onGuiUnitTests();
    
    // ongui all the systems
    int to_erase = -1;
    int i = 0;
    for (auto system : _system_list)
    {
        if (!system->onGui(context))
        {
            to_erase = i;
        }
        i++;
    }
    
    if (to_erase >= 0)
    {
        // erase to_erase from system list
        _system_list.erase(_system_list.begin() + to_erase);

    }
    
    
    if (_demo_window_open) {
        ImGui::ShowDemoWindow(&_demo_window_open);
    }
}

namespace metalnes {
void run_tests();
}

void AppInit(render::ContextPtr context, std::string resource_dir, std::string data_dir, const std::vector<std::string> &args)
{
    ImGuiSupport_Init(context, resource_dir, data_dir );

    
    s_resource_dir = resource_dir;
    s_data_dir = data_dir;
    s_system_def_dir = Core::Path::Combine(resource_dir, "data/system-def");
    
    // Look for ROMs locally first
    s_rom_dirs.push_back( Core::Path::Combine(resource_dir, "roms/") );
    s_rom_dirs.push_back( "./roms/" );
    s_rom_dirs.push_back( Core::Path::Combine(resource_dir, "data/roms/") );
    
    // Then check user directory as fallback
    const char* homeDir = getenv("HOME");
    if (!homeDir) homeDir = getenv("USERPROFILE"); // Windows fallback
    if (homeDir) {
        s_rom_dirs.push_back( Core::Path::Combine( homeDir,  "dev/nes/roms/") );
    }
    
    // Debug: Print ROM search directories
    printf("ROM search directories:\n");
    for (auto dir : s_rom_dirs) {
        printf("  %s\n", dir.c_str());
    }

#if 1

    if (!args.empty())
    {
        for (auto arg : args)
        {
            load_system(arg);
        }
    }
    
    if (args.empty())
    {
        // Try to load the first available ROM file in any of the search directories
        for (auto rom_dir : s_rom_dirs)
        {
            std::vector<std::string> files;
            Core::Directory::GetDirectoryFiles(rom_dir, files, false);
            
            for (auto file : files)
            {
                if (file.find(".nes") != std::string::npos)
                {
                    // Extract just the filename from the full path
                    size_t pos = file.find_last_of("/\\");
                    std::string filename = (pos != std::string::npos) ? file.substr(pos + 1) : file;
                    
                    printf("Found ROM file: %s (filename: %s)\n", file.c_str(), filename.c_str());
                    if (load_system(filename))
                    {
                        printf("Successfully loaded ROM: %s\n", filename.c_str());
                        break;
                    }
                }
            }
            
            if (!_system_list.empty())
                break;
        }
        
        if (_system_list.empty())
        {
            printf("No ROM files found in search directories. Place .nes files in ./roms/ directory.\n");
        }
    }
#else
//    run_tests();
    RunUnitTests();
#endif
}

void AppShutdown()
{
    _system_list.clear();
 
    ImGuiSupport_Shutdown();

}


bool AppShouldQuit()
{
    for (auto system : _system_list)
    {
        if (system->shouldQuit())
            return true;
    }
    return false;
}

void AppRender(render::ContextPtr context)
{
    if (context)
    {
        ImGuiSupport_NewFrame();
        AppOnGui(context);
        context->SetRenderTarget(nullptr, "ImGui", render::LoadAction::Clear, Color4F(0,0,0,0));
        ImGuiSupport_Render();
    }
}









