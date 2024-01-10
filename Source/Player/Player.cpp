//
// Copyright (c) 2017-2020 the rbfx project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#define URHO3D_LUA

#include "Player.h"

#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Engine/StateManager.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Plugins/PluginApplication.h>
#include <Urho3D/Plugins/PluginManager.h>
#include <Urho3D/Resource/ResourceCache.h>
#if URHO3D_SYSTEMUI
#   include <Urho3D/SystemUI/SystemUI.h>
#endif
#if URHO3D_RMLUI
#   include <Urho3D/RmlUI/RmlUI.h>
#endif

namespace Urho3D
{
bool URHO3D_API RunLua(Context* context, const ea::string& scriptFileName);

Player::Player(Context* context)
    : Application(context)
{
}

void Player::Setup()
{
    FileSystem* fs = context_->GetSubsystem<FileSystem>();
    auto& engineParameters_ = GetEngineParameters();
    engineParameters_[EP_ORGANIZATION_NAME] = "KFEngine";
    engineParameters_[EP_APPLICATION_NAME] = "KFPlayer";
#ifndef _WIN32
    engineParameters_[EP_RESOURCE_PATHS] = "Assets/Engine"; // "CoreData;Data";//
#else
    engineParameters_[EP_FULL_SCREEN] = false;
    engineParameters_[EP_WINDOW_MAXIMIZE] = false;
    engineParameters_[EP_BORDERLESS] = false;
    engineParameters_[EP_RESOURCE_PREFIX_PATHS] = ";..;../..";
#endif
// #if MOBILE
//     engineParameters_[EP_RESOURCE_PATHS] = "";
// #else
//     engineParameters_[EP_RESOURCE_PREFIX_PATHS] = fs->GetProgramDir() + ";" + fs->GetCurrentDir();
// #endif
//     engineParameters_[EP_PLUGINS] = ea::string::joined(PluginApplication::GetStaticPlugins(), ";");
// 
//     PluginApplication::RegisterStaticPlugins();
}

void Player::Start()
{
    // Check for script file name from the arguments
#if defined(DESKTOP) || defined(__EMSCRIPTEN__)
    auto* input = GetSubsystem<Input>();
    input->SetMouseVisible(true);
#endif
    get_script_filename();
    ea::string extension = GetExtension(scriptFileName_);
    if (extension == ".lua" || extension == ".luc") {
#ifdef URHO3D_LUA
        if (RunLua(context_, scriptFileName_)) {
            return;
        }
#else
        ErrorExit("Lua is not enabled!");
        return;
#endif
    }

//     auto engine = GetSubsystem<Engine>();
//     if (!engine->IsHeadless())
//     {
// #if URHO3D_SYSTEMUI
//         ui::GetIO().IniFilename = nullptr; // Disable of imgui.ini creation,
// #endif
// 
//         // TODO(editor): Support resource routing
// 
// #if URHO3D_RMLUI
//         auto* cache = GetSubsystem<ResourceCache>();
//         auto* ui = GetSubsystem<RmlUI>();
//         ea::vector<ea::string> fonts;
//         for (const char* pattern: {"*.ttf", "*.otf"})
//         {
//             ea::vector<ea::string> fonts;
//             cache->Scan(fonts, "Fonts/", pattern, SCAN_FILES | SCAN_RECURSIVE);
//             for (const ea::string& font : fonts)
//                 ui->LoadFont(Format("Fonts/{}", font));
//         }
// #endif
//     }
// 
//     const StringVector loadedPlugins = engine->GetParameter(EP_PLUGINS).GetString().split(';');
// 
//     auto pluginManager = GetSubsystem<PluginManager>();
//     pluginManager->SetPluginsLoaded(loadedPlugins);
//     pluginManager->StartApplication();
}

void Player::Stop()
{
//     auto pluginManager = GetSubsystem<PluginManager>();
//     pluginManager->StopApplication();
// 
//     auto stateManager = GetSubsystem<StateManager>();
//     stateManager->Reset();
}
void Player::get_script_filename()
{
    const ea::vector<ea::string>& arguments = GetArguments();
    scriptFileName_ = (arguments.size() && arguments[0][0] != '-') ? GetInternalPath(arguments[0]) : "Scripts/App.lua";
}

}
