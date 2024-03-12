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
#if DESKTOP
#include "Urho3D/Utility/HttpProxy.h"
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
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
    engineParameters_[EP_RENDER_BACKEND] = static_cast<int>(RenderBackend::OpenGL);
#else
    engineParameters_[EP_RENDER_BACKEND] = static_cast<int>(RenderBackend::Vulkan);
#endif
#ifndef _WIN32
    engineParameters_[EP_RESOURCE_PATHS] = "Assets/Engine"; // "CoreData;Data";//
#else
    engineParameters_[EP_FULL_SCREEN] = false;
    engineParameters_[EP_WINDOW_MAXIMIZE] = false;
    engineParameters_[EP_BORDERLESS] = false;
    engineParameters_[EP_RESOURCE_PREFIX_PATHS] = ";..;../..";
    //engineParameters_[EP_RENDER_BACKEND] = static_cast<int>(RenderBackend::OpenGL);
    engineParameters_[EP_RENDER_BACKEND] = static_cast<int>(RenderBackend::Vulkan);
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
//#if defined(DESKTOP) || defined(__EMSCRIPTEN__)
    auto* input = GetSubsystem<Input>();
    input->SetMouseMode(MM_FREE);
    input->SetMouseVisible(true);
//#endif

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

#if !defined(__EMSCRIPTEN__) && !defined(__ANDROID__) && !defined(IOS) && !defined(TVOS)
static void collect_files(const std::filesystem::path& root_path, std::vector<std::filesystem::path>& paths)
{
    for (auto& it : std::filesystem::directory_iterator(root_path))
    {
        auto path_str = it.path().string();
        std::replace(path_str.begin(), path_str.end(), '\\', '/');
        auto path = std::filesystem::path(path_str);
        if (std::filesystem::is_directory(it.status()))
        {
            collect_files(path, paths);
        }
        else
        {
            paths.emplace_back(path);
        }
    }
}
#endif
int ConsoleMain()
{
#if !defined(__EMSCRIPTEN__) && !defined(__ANDROID__) && !defined(IOS) && !defined(TVOS)
    ea::string exepath;
    #if defined(_WIN32)
    wchar_t exeName[MAX_PATH];
    exeName[0] = 0;
    GetModuleFileNameW(nullptr, exeName, MAX_PATH);
    exepath = ea::string(WideToMultiByte(exeName));
    #elif defined(__APPLE__)
    char exeName[MAX_PATH];
    memset(exeName, 0, MAX_PATH);
    unsigned size = MAX_PATH;
    _NSGetExecutablePath(exeName, &size);
    exepath = ea::string(exeName);
    #elif defined(__linux__)
    char exeName[MAX_PATH];
    memset(exeName, 0, MAX_PATH);
    pid_t pid = getpid();
    ea::string link(ea::string::CtorSprintf{}, "/proc/%d/exe", pid);
    readlink(link.c_str(), exeName, MAX_PATH);
    exepath = ea::string(exeName);
    #endif

    CurlInit();

    const StringVector& rawArguments = GetArguments();
    const auto& opt = rawArguments[0];

    auto path = std::filesystem::path(exepath.c_str()).remove_filename().string();
    std::replace(path.begin(), path.end(), '\\', '/');
    auto userfilename = path + "user.id";
    if (opt == "--login")
    {
        //         std::string jsonstr("{\"username\":\"");
        //         jsonstr += rawArguments[1].c_str();
        //         jsonstr += "\",\"password\":\"";
        //         jsonstr += rawArguments[2].c_str();
        //         jsonstr += "\"}";
        std::string jsonstr("username=");
        jsonstr += rawArguments[1].c_str();
        jsonstr += "&password=";
        jsonstr += rawArguments[2].c_str();
        char* body = nullptr;
        auto size = Post("http://kfengine.com:5001/api/login", jsonstr, &body);
        std::string bodystr(body, size);
        if (bodystr.find("\"success\":true") != std::string::npos)
        {
            auto p0 = bodystr.find("\"id\":");
            auto id = std::stoi(bodystr.substr(p0 + 5, 5));
            free(body);
            auto fd = fopen(userfilename.c_str(), "wb");
            fwrite(&id, 4, 1, fd);
            fclose(fd);
            printf("Login success, uid: %d\n", id);
        }
        else
        {
            printf("Login failed.\n");
        }
    }
    else
    {
        uint32_t id = 0;
        auto fd = fopen(userfilename.c_str(), "rb");
        auto ret = fread(&id, 4, 1, fd);
        if (ret != 1)
        {
            printf("Read id failed, please login first.\n");
        }
        else
        {
            auto url = std::string("ftp://kfengine.com/wwwdata/Games/") + std::to_string(id);
            auto localpathstr = std::filesystem::path(path + rawArguments[1].c_str() + "/" + std::to_string(id))
                                    .lexically_normal()
                                    .string();
            std::replace(localpathstr.begin(), localpathstr.end(), '\\', '/');
            auto localpath = std::filesystem::path(localpathstr);
            if (!std::filesystem::exists(localpath))
            {
                std::filesystem::create_directories(localpath);
            }
            if (opt == "--pull")
            {
                DownloadDirectory(url.c_str(), localpathstr.c_str());
                printf("Download finish.\n");
            }
            else if (opt == "--push")
            {
                std::vector<std::filesystem::path> filelist;
                std::filesystem::path root_path(localpathstr);
                collect_files(root_path, filelist);

                // update filelist.json
                std::string jsoncontent("[");
                for (int i = 0; i < filelist.size(); i++)
                {
                    auto relative = std::filesystem::relative(filelist[i], localpath).string();
                    std::replace(relative.begin(), relative.end(), '\\', '/');
                    jsoncontent += "\"" + relative + "\"";
                    if (i < filelist.size() - 1)
                    {
                        jsoncontent += ",";
                    }
                }
                jsoncontent += "]";
                auto rpstr = localpathstr + "/filelist.json";
                auto fd = fopen(rpstr.c_str(), "wt");
                auto ret = fwrite(jsoncontent.data(), jsoncontent.size(), 1, fd);
                fclose(fd);
                //
                std::vector<std::string> exclude;
                exclude.emplace_back("Models/Blockman");
                exclude.emplace_back("Effekseer/01_Suzuki01");

                UploadDirectory(localpathstr, url, filelist, exclude);
                printf("Upload finish.\n");
            }
        }
        fclose(fd);
    }
    CurlCleanup();
    system("pause");
#endif
    return 0;
}

}

#if URHO3D_CSHARP
URHO3D_DEFINE_APPLICATION_MAIN_CSHARP(Urho3D::Player);
#else
URHO3D_DEFINE_APPLICATION_MAIN(Urho3D::Player);
#endif
