/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "CCGLViewImpl.h"

#include <cmath>
#include <unordered_map>

//#include "platform/CCApplication.h"
#include "base/CCDirector.h"
#include "base/CCTouch.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventKeyboard.h"
#include "base/CCEventMouse.h"
#include "base/CCIMEDispatcher.h"
#include "base/ccUtils.h"
#include "base/ccUTF8.h"
#include "2d/CCCamera.h"
// #if CC_ICON_SET_SUPPORT
// #include "platform/CCImage.h"
// #endif /* CC_ICON_SET_SUPPORT */

#include "renderer/CCRenderer.h"

#include "../../Input/InputConstants.h"
#include "../../Core/Context.h"
#include "../../Graphics/Graphics.h"
#include "Urho3DContext.h"

NS_CC_BEGIN

//GLViewImpl* GLFWEventHandler::_view = nullptr;

const std::string GLViewImpl::EVENT_WINDOW_RESIZED = "glview_window_resized";
const std::string GLViewImpl::EVENT_WINDOW_FOCUSED = "glview_window_focused";
const std::string GLViewImpl::EVENT_WINDOW_UNFOCUSED = "glview_window_unfocused";

////////////////////////////////////////////////////

struct keyCodeItem
{
    int glfwKeyCode;
    EventKeyboard::KeyCode keyCode;
};

static std::unordered_map<int, EventKeyboard::KeyCode> g_keyCodeMap;

static keyCodeItem g_keyCodeStructArray[] = {
    /* The unknown key */
    {Urho3D::KEY_UNKNOWN, EventKeyboard::KeyCode::KEY_NONE},

    /* Printable keys */
    {Urho3D::KEY_SPACE, EventKeyboard::KeyCode::KEY_SPACE},
    {Urho3D::KEY_COMMA, EventKeyboard::KeyCode::KEY_COMMA},
    {Urho3D::KEY_MINUS, EventKeyboard::KeyCode::KEY_MINUS},
    {Urho3D::KEY_PERIOD, EventKeyboard::KeyCode::KEY_PERIOD},
    {Urho3D::KEY_SLASH, EventKeyboard::KeyCode::KEY_SLASH},
    {Urho3D::KEY_0, EventKeyboard::KeyCode::KEY_0},
    {Urho3D::KEY_1, EventKeyboard::KeyCode::KEY_1},
    {Urho3D::KEY_2, EventKeyboard::KeyCode::KEY_2},
    {Urho3D::KEY_3, EventKeyboard::KeyCode::KEY_3},
    {Urho3D::KEY_4, EventKeyboard::KeyCode::KEY_4},
    {Urho3D::KEY_5, EventKeyboard::KeyCode::KEY_5},
    {Urho3D::KEY_6, EventKeyboard::KeyCode::KEY_6},
    {Urho3D::KEY_7, EventKeyboard::KeyCode::KEY_7},
    {Urho3D::KEY_8, EventKeyboard::KeyCode::KEY_8},
    {Urho3D::KEY_9, EventKeyboard::KeyCode::KEY_9},
    {Urho3D::KEY_SEMICOLON, EventKeyboard::KeyCode::KEY_SEMICOLON},
    {Urho3D::KEY_EQUALS, EventKeyboard::KeyCode::KEY_EQUAL},
    {Urho3D::KEY_A, EventKeyboard::KeyCode::KEY_A},
    {Urho3D::KEY_B, EventKeyboard::KeyCode::KEY_B},
    {Urho3D::KEY_C, EventKeyboard::KeyCode::KEY_C},
    {Urho3D::KEY_D, EventKeyboard::KeyCode::KEY_D},
    {Urho3D::KEY_E, EventKeyboard::KeyCode::KEY_E},
    {Urho3D::KEY_F, EventKeyboard::KeyCode::KEY_F},
    {Urho3D::KEY_G, EventKeyboard::KeyCode::KEY_G},
    {Urho3D::KEY_H, EventKeyboard::KeyCode::KEY_H},
    {Urho3D::KEY_I, EventKeyboard::KeyCode::KEY_I},
    {Urho3D::KEY_J, EventKeyboard::KeyCode::KEY_J},
    {Urho3D::KEY_K, EventKeyboard::KeyCode::KEY_K},
    {Urho3D::KEY_L, EventKeyboard::KeyCode::KEY_L},
    {Urho3D::KEY_M, EventKeyboard::KeyCode::KEY_M},
    {Urho3D::KEY_N, EventKeyboard::KeyCode::KEY_N},
    {Urho3D::KEY_O, EventKeyboard::KeyCode::KEY_O},
    {Urho3D::KEY_P, EventKeyboard::KeyCode::KEY_P},
    {Urho3D::KEY_Q, EventKeyboard::KeyCode::KEY_Q},
    {Urho3D::KEY_R, EventKeyboard::KeyCode::KEY_R},
    {Urho3D::KEY_S, EventKeyboard::KeyCode::KEY_S},
    {Urho3D::KEY_T, EventKeyboard::KeyCode::KEY_T},
    {Urho3D::KEY_U, EventKeyboard::KeyCode::KEY_U},
    {Urho3D::KEY_V, EventKeyboard::KeyCode::KEY_V},
    {Urho3D::KEY_W, EventKeyboard::KeyCode::KEY_W},
    {Urho3D::KEY_X, EventKeyboard::KeyCode::KEY_X},
    {Urho3D::KEY_Y, EventKeyboard::KeyCode::KEY_Y},
    {Urho3D::KEY_Z, EventKeyboard::KeyCode::KEY_Z},
    {Urho3D::KEY_LEFTBRACKET, EventKeyboard::KeyCode::KEY_LEFT_BRACKET},
    {Urho3D::KEY_BACKSLASH, EventKeyboard::KeyCode::KEY_BACK_SLASH},
    {Urho3D::KEY_RIGHTBRACKET, EventKeyboard::KeyCode::KEY_RIGHT_BRACKET},

    /* Function keys */
    {Urho3D::KEY_ESCAPE, EventKeyboard::KeyCode::KEY_ESCAPE},
    {Urho3D::KEY_TAB, EventKeyboard::KeyCode::KEY_TAB},
    {Urho3D::KEY_BACKSPACE, EventKeyboard::KeyCode::KEY_BACKSPACE},
    {Urho3D::KEY_INSERT, EventKeyboard::KeyCode::KEY_INSERT},
    {Urho3D::KEY_DELETE, EventKeyboard::KeyCode::KEY_DELETE},
    {Urho3D::KEY_RIGHT, EventKeyboard::KeyCode::KEY_RIGHT_ARROW},
    {Urho3D::KEY_LEFT, EventKeyboard::KeyCode::KEY_LEFT_ARROW},
    {Urho3D::KEY_DOWN, EventKeyboard::KeyCode::KEY_DOWN_ARROW},
    {Urho3D::KEY_UP, EventKeyboard::KeyCode::KEY_UP_ARROW},
    {Urho3D::KEY_PAGEUP, EventKeyboard::KeyCode::KEY_PG_UP},
    {Urho3D::KEY_PAGEDOWN, EventKeyboard::KeyCode::KEY_PG_DOWN},
    {Urho3D::KEY_HOME, EventKeyboard::KeyCode::KEY_HOME},
    {Urho3D::KEY_END, EventKeyboard::KeyCode::KEY_END},
    {Urho3D::KEY_CAPSLOCK, EventKeyboard::KeyCode::KEY_CAPS_LOCK},
    {Urho3D::KEY_SCROLLLOCK, EventKeyboard::KeyCode::KEY_SCROLL_LOCK},
    {Urho3D::KEY_NUMLOCKCLEAR, EventKeyboard::KeyCode::KEY_NUM_LOCK},
    {Urho3D::KEY_PRINTSCREEN, EventKeyboard::KeyCode::KEY_PRINT},
    {Urho3D::KEY_PAUSE, EventKeyboard::KeyCode::KEY_PAUSE},
    {Urho3D::KEY_F1, EventKeyboard::KeyCode::KEY_F1},
    {Urho3D::KEY_F2, EventKeyboard::KeyCode::KEY_F2},
    {Urho3D::KEY_F3, EventKeyboard::KeyCode::KEY_F3},
    {Urho3D::KEY_F4, EventKeyboard::KeyCode::KEY_F4},
    {Urho3D::KEY_F5, EventKeyboard::KeyCode::KEY_F5},
    {Urho3D::KEY_F6, EventKeyboard::KeyCode::KEY_F6},
    {Urho3D::KEY_F7, EventKeyboard::KeyCode::KEY_F7},
    {Urho3D::KEY_F8, EventKeyboard::KeyCode::KEY_F8},
    {Urho3D::KEY_F9, EventKeyboard::KeyCode::KEY_F9},
    {Urho3D::KEY_F10, EventKeyboard::KeyCode::KEY_F10},
    {Urho3D::KEY_F11, EventKeyboard::KeyCode::KEY_F11},
    {Urho3D::KEY_F12, EventKeyboard::KeyCode::KEY_F12},
    {Urho3D::KEY_F13, EventKeyboard::KeyCode::KEY_NONE},
    {Urho3D::KEY_F14, EventKeyboard::KeyCode::KEY_NONE},
    {Urho3D::KEY_F15, EventKeyboard::KeyCode::KEY_NONE},
    {Urho3D::KEY_F16, EventKeyboard::KeyCode::KEY_NONE},
    {Urho3D::KEY_F17, EventKeyboard::KeyCode::KEY_NONE},
    {Urho3D::KEY_F18, EventKeyboard::KeyCode::KEY_NONE},
    {Urho3D::KEY_F19, EventKeyboard::KeyCode::KEY_NONE},
    {Urho3D::KEY_F20, EventKeyboard::KeyCode::KEY_NONE},
    {Urho3D::KEY_F21, EventKeyboard::KeyCode::KEY_NONE},
    {Urho3D::KEY_F22, EventKeyboard::KeyCode::KEY_NONE},
    {Urho3D::KEY_F23, EventKeyboard::KeyCode::KEY_NONE},
    {Urho3D::KEY_F24, EventKeyboard::KeyCode::KEY_NONE},
    {Urho3D::KEY_KP_0, EventKeyboard::KeyCode::KEY_0},
    {Urho3D::KEY_KP_1, EventKeyboard::KeyCode::KEY_1},
    {Urho3D::KEY_KP_2, EventKeyboard::KeyCode::KEY_2},
    {Urho3D::KEY_KP_3, EventKeyboard::KeyCode::KEY_3},
    {Urho3D::KEY_KP_4, EventKeyboard::KeyCode::KEY_4},
    {Urho3D::KEY_KP_5, EventKeyboard::KeyCode::KEY_5},
    {Urho3D::KEY_KP_6, EventKeyboard::KeyCode::KEY_6},
    {Urho3D::KEY_KP_7, EventKeyboard::KeyCode::KEY_7},
    {Urho3D::KEY_KP_8, EventKeyboard::KeyCode::KEY_8},
    {Urho3D::KEY_KP_9, EventKeyboard::KeyCode::KEY_9},
    {Urho3D::KEY_KP_DECIMAL, EventKeyboard::KeyCode::KEY_PERIOD},
    {Urho3D::KEY_KP_DIVIDE, EventKeyboard::KeyCode::KEY_KP_DIVIDE},
    {Urho3D::KEY_KP_MULTIPLY, EventKeyboard::KeyCode::KEY_KP_MULTIPLY},
    {Urho3D::KEY_KP_MINUS, EventKeyboard::KeyCode::KEY_KP_MINUS},
    {Urho3D::KEY_KP_PLUS, EventKeyboard::KeyCode::KEY_KP_PLUS},
    {Urho3D::KEY_KP_ENTER, EventKeyboard::KeyCode::KEY_KP_ENTER},
    {Urho3D::KEY_LSHIFT, EventKeyboard::KeyCode::KEY_LEFT_SHIFT},
    {Urho3D::KEY_LCTRL, EventKeyboard::KeyCode::KEY_LEFT_CTRL},
    {Urho3D::KEY_LALT, EventKeyboard::KeyCode::KEY_LEFT_ALT},
    {Urho3D::KEY_RSHIFT, EventKeyboard::KeyCode::KEY_RIGHT_SHIFT},
    {Urho3D::KEY_RCTRL, EventKeyboard::KeyCode::KEY_RIGHT_CTRL},
    {Urho3D::KEY_RALT, EventKeyboard::KeyCode::KEY_RIGHT_ALT},
    {Urho3D::KEY_MENU, EventKeyboard::KeyCode::KEY_MENU},
};

//////////////////////////////////////////////////////////////////////////
// implement GLViewImpl
//////////////////////////////////////////////////////////////////////////


GLViewImpl::GLViewImpl(bool initglfw)
: _captured(false)
, _supportTouch(false)
, _isInRetinaMonitor(false)
, _isRetinaEnabled(false)
, _retinaFactor(1)
, _frameZoomFactor(1.0f)
// , _mainWindow(nullptr)
// , _monitor(nullptr)
, _mouseX(0.0f)
, _mouseY(0.0f)
{
    _viewName = "cocos2dx";
    g_keyCodeMap.clear();
    for (auto& item : g_keyCodeStructArray)
    {
        g_keyCodeMap[item.glfwKeyCode] = item.keyCode;
    }

//    GLFWEventHandler::setGLViewImpl(this);
//     if (initglfw)
//     {
//         glfwSetErrorCallback(GLFWEventHandler::onGLFWError);
//         glfwInit();
//     }
}

GLViewImpl::~GLViewImpl()
{
    CCLOGINFO("deallocing GLViewImpl: %p", this);
    //GLFWEventHandler::setGLViewImpl(nullptr);
    //glfwTerminate();
}

GLViewImpl* GLViewImpl::create(const std::string& viewName)
{
    return GLViewImpl::create(viewName, false);
}

GLViewImpl* GLViewImpl::create(const std::string& viewName, bool resizable)
{
    auto ret = new (std::nothrow) GLViewImpl;
    if(ret && ret->initWithRect(viewName, Rect(0, 0, 960, 640), 1.0f, resizable)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GLViewImpl* GLViewImpl::createWithRect(const std::string& viewName, Rect rect, float frameZoomFactor, bool resizable)
{
    auto ret = new (std::nothrow) GLViewImpl;
    if(ret && ret->initWithRect(viewName, rect, frameZoomFactor, resizable)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GLViewImpl* GLViewImpl::createWithFullScreen(const std::string& viewName)
{
    auto ret = new (std::nothrow) GLViewImpl();
    if(ret && ret->initWithFullScreen(viewName)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

// GLViewImpl* GLViewImpl::createWithFullScreen(const std::string& viewName, const GLFWvidmode &videoMode, GLFWmonitor *monitor)
// {
//     auto ret = new (std::nothrow) GLViewImpl();
//     if(ret && ret->initWithFullscreen(viewName, videoMode, monitor)) {
//         ret->autorelease();
//         return ret;
//     }
//     CC_SAFE_DELETE(ret);
//     return nullptr;
// }

bool GLViewImpl::initWithRect(const std::string& viewName, Rect rect, float frameZoomFactor, bool resizable)
{
    setViewName(viewName);

    _frameZoomFactor = frameZoomFactor;
    _graphics = GetUrho3DContext()->GetSubsystem<Urho3D::Graphics>();
//     glfwWindowHint(GLFW_RESIZABLE,resizable?GL_TRUE:GL_FALSE);
//     glfwWindowHint(GLFW_RED_BITS,_glContextAttrs.redBits);
//     glfwWindowHint(GLFW_GREEN_BITS,_glContextAttrs.greenBits);
//     glfwWindowHint(GLFW_BLUE_BITS,_glContextAttrs.blueBits);
//     glfwWindowHint(GLFW_ALPHA_BITS,_glContextAttrs.alphaBits);
//     glfwWindowHint(GLFW_DEPTH_BITS,_glContextAttrs.depthBits);
//     glfwWindowHint(GLFW_STENCIL_BITS,_glContextAttrs.stencilBits);
//     
//     glfwWindowHint(GLFW_SAMPLES, _glContextAttrs.multisamplingCount);
    
// #if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
//     // Don't create gl context.
//     glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
// #endif

    int neededWidth = (int)(rect.size.width * _frameZoomFactor);
    int neededHeight = (int)(rect.size.height * _frameZoomFactor);

//     _mainWindow = glfwCreateWindow(neededWidth, neededHeight, _viewName.c_str(), _monitor, nullptr);
// 
//     if (_mainWindow == nullptr)
//     {
//         std::string message = "Can't create window";
//         if (!_glfwError.empty())
//         {
//             message.append("\nMore info: \n");
//             message.append(_glfwError);
//         }
// 
//         ccMessageBox(message.c_str(), "Error launch application");
//         return false;
//     }

    /*
    *  Note that the created window and context may differ from what you requested,
    *  as not all parameters and hints are
    *  [hard constraints](@ref window_hints_hard).  This includes the size of the
    *  window, especially for full screen windows.  To retrieve the actual
    *  attributes of the created window and context, use queries like @ref
    *  glfwGetWindowAttrib and @ref glfwGetWindowSize.
    *
    *  see declaration glfwCreateWindow
    */
    auto frameSize = _graphics->GetSize();
    int realW = frameSize.x_, realH = frameSize.y_;
    //glfwGetWindowSize(_mainWindow, &realW, &realH);
    if (realW != neededWidth)
    {
        rect.size.width = realW / _frameZoomFactor;
    }
    if (realH != neededHeight)
    {
        rect.size.height = realH / _frameZoomFactor;
    }

//     glfwMakeContextCurrent(_mainWindow);
// 
//     glfwSetMouseButtonCallback(_mainWindow, GLFWEventHandler::onGLFWMouseCallBack);
//     glfwSetCursorPosCallback(_mainWindow, GLFWEventHandler::onGLFWMouseMoveCallBack);
//     glfwSetScrollCallback(_mainWindow, GLFWEventHandler::onGLFWMouseScrollCallback);
//     glfwSetCharCallback(_mainWindow, GLFWEventHandler::onGLFWCharCallback);
//     glfwSetKeyCallback(_mainWindow, GLFWEventHandler::onGLFWKeyCallback);
//     glfwSetWindowPosCallback(_mainWindow, GLFWEventHandler::onGLFWWindowPosCallback);
//     glfwSetFramebufferSizeCallback(_mainWindow, GLFWEventHandler::onGLFWframebuffersize);
//     glfwSetWindowSizeCallback(_mainWindow, GLFWEventHandler::onGLFWWindowSizeFunCallback);
//     glfwSetWindowIconifyCallback(_mainWindow, GLFWEventHandler::onGLFWWindowIconifyCallback);
//     glfwSetWindowFocusCallback(_mainWindow, GLFWEventHandler::onGLFWWindowFocusCallback);

    setFrameSize(rect.size.width, rect.size.height);

// #if (CC_TARGET_PLATFORM != CC_PLATFORM_MAC)
//     
//     // check OpenGL version at first
//     const GLubyte* glVersion = glGetString(GL_VERSION);
// 
//     if ( utils::atof((const char*)glVersion) < 1.5 )
//     {
//         char strComplain[256] = {0};
//         sprintf(strComplain,
//                 "OpenGL 1.5 or higher is required (your version is %s). Please upgrade the driver of your video card.",
//                 glVersion);
//         ccMessageBox(strComplain, "OpenGL version too old");
//         return false;
//     }
// 
//     initGlew();
// 
//     // Enable point size by default.
//     glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
//     
//     if(_glContextAttrs.multisamplingCount > 0)
//         glEnable(GL_MULTISAMPLE);
// 
// //    // GLFW v3.2 no longer emits "onGLFWWindowSizeFunCallback" at creation time. Force default viewport:
// //    setViewPortInPoints(0, 0, neededWidth, neededHeight);
// //
// #endif
    return true;
}

bool GLViewImpl::initWithFullScreen(const std::string& viewName)
{
    //Create fullscreen window on primary monitor at its current video mode.
//     _monitor = glfwGetPrimaryMonitor();
//     if (nullptr == _monitor)
//         return false;
// 
//     const GLFWvidmode* videoMode = glfwGetVideoMode(_monitor);
//     return initWithRect(viewName, Rect(0, 0, (float)videoMode->width, (float)videoMode->height), 1.0f, false);
    return false;
}

// bool GLViewImpl::initWithFullscreen(const std::string &viewname, const GLFWvidmode &videoMode, GLFWmonitor *monitor)
// {
//     //Create fullscreen on specified monitor at the specified video mode.
//     _monitor = monitor;
//     if (nullptr == _monitor)
//         return false;
//     
//     //These are soft constraints. If the video mode is retrieved at runtime, the resulting window and context should match these exactly. If invalid attribs are passed (eg. from an outdated cache), window creation will NOT fail but the actual window/context may differ.
//     glfwWindowHint(GLFW_REFRESH_RATE, videoMode.refreshRate);
//     glfwWindowHint(GLFW_RED_BITS, videoMode.redBits);
//     glfwWindowHint(GLFW_BLUE_BITS, videoMode.blueBits);
//     glfwWindowHint(GLFW_GREEN_BITS, videoMode.greenBits);
//     
//     return initWithRect(viewname, Rect(0, 0, (float)videoMode.width, (float)videoMode.height), 1.0f, false);
// }

bool GLViewImpl::isOpenGLReady()
{
    return true;
}

void GLViewImpl::end()
{
//     if(_mainWindow)
//     {
//         glfwSetWindowShouldClose(_mainWindow,1);
//         _mainWindow = nullptr;
//     }
    // Release self. Otherwise, GLViewImpl could not be freed.
    release();
}

void GLViewImpl::swapBuffers()
{
//     if(_mainWindow)
//         glfwSwapBuffers(_mainWindow);
}

bool GLViewImpl::windowShouldClose()
{
//     if(_mainWindow)
//         return glfwWindowShouldClose(_mainWindow) ? true : false;
//     else
        return false;
}

void GLViewImpl::pollEvents()
{
//    glfwPollEvents();
}

void GLViewImpl::enableRetina(bool enabled)
{
// #if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
//     _isRetinaEnabled = enabled;
//     if (_isRetinaEnabled)
//     {
//         _retinaFactor = 1;
//     }
//     else
//     {
//         _retinaFactor = 2;
//     }
//     updateFrameSize();
// #endif
}


void GLViewImpl::setIMEKeyboardState(bool /*bOpen*/)
{

}

#if CC_ICON_SET_SUPPORT
void GLViewImpl::setIcon(const std::string& filename) const {
    std::vector<std::string> vec = {filename};
    this->setIcon(vec);
}

void GLViewImpl::setIcon(const std::vector<std::string>& filelist) const {
//     if (filelist.empty()) return;
//     std::vector<Image*> icons;
//     for (auto const& filename: filelist) {
//         Image* icon = new (std::nothrow) Image();
//         if (icon && icon->initWithImageFile(filename)) {
//             icons.push_back(icon);
//         } else {
//             CC_SAFE_DELETE(icon);
//         }
//     }
// 
//     if (icons.empty()) return; // No valid images
//     size_t iconsCount = icons.size();
//     auto images = new GLFWimage[iconsCount];
//     for (size_t i = 0; i < iconsCount; i++) {
//         auto& image = images[i];
//         auto& icon = icons[i];
//         image.width = icon->getWidth();
//         image.height = icon->getHeight();
//         image.pixels = icon->getData();
//     };
// 
//     GLFWwindow* window = this->getWindow();
//     glfwSetWindowIcon(window, iconsCount, images);
// 
//     CC_SAFE_DELETE_ARRAY(images);
//     for (auto& icon: icons) {
//         CC_SAFE_DELETE(icon);
//     }
}

void GLViewImpl::setDefaultIcon() const {
//     GLFWwindow* window = this->getWindow();
//     glfwSetWindowIcon(window, 0, nullptr);
}
#endif /* CC_ICON_SET_SUPPORT */

void GLViewImpl::setCursorVisible( bool isVisible )
{
//     if( _mainWindow == NULL )
//         return;
//     
//     if( isVisible )
//         glfwSetInputMode(_mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//     else
//         glfwSetInputMode(_mainWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void GLViewImpl::setFrameZoomFactor(float zoomFactor)
{
    CCASSERT(zoomFactor > 0.0f, "zoomFactor must be larger than 0");

    if (std::abs(_frameZoomFactor - zoomFactor) < FLT_EPSILON)
    {
        return;
    }

    _frameZoomFactor = zoomFactor;
    updateFrameSize();
}

float GLViewImpl::getFrameZoomFactor() const
{
    return _frameZoomFactor;
}

bool GLViewImpl::isFullscreen() const {
    return false; //(_monitor != nullptr);
}

void GLViewImpl::setFullscreen() {
    if (this->isFullscreen()) {
        return;
    }
//     _monitor = glfwGetPrimaryMonitor();
//     if (nullptr == _monitor) {
//         return;
//     }
//     const GLFWvidmode* videoMode = glfwGetVideoMode(_monitor);
//     this->setFullscreen(*videoMode, _monitor);
}

void GLViewImpl::setFullscreen(int monitorIndex) {
//     // set fullscreen on specific monitor
//     int count = 0;
//     GLFWmonitor** monitors = glfwGetMonitors(&count);
//     if (monitorIndex < 0 || monitorIndex >= count) {
//         return;
//     }
//     GLFWmonitor* monitor = monitors[monitorIndex];
//     if (nullptr == monitor) {
//         return;
//     }
//     const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
//     this->setFullscreen(*videoMode, monitor);
}

// void GLViewImpl::setFullscreen(const GLFWvidmode &videoMode, GLFWmonitor *monitor) {
//     _monitor = monitor;
//     glfwSetWindowMonitor(_mainWindow, _monitor, 0, 0, videoMode.width, videoMode.height, videoMode.refreshRate);
// }

void GLViewImpl::setWindowed(int width, int height) {
    if (!this->isFullscreen()) {
        this->setFrameSize((float)width, (float)height);
    } else {
//         const GLFWvidmode* videoMode = glfwGetVideoMode(_monitor);
//         int xpos = 0, ypos = 0;
//         glfwGetMonitorPos(_monitor, &xpos, &ypos);
//         xpos += (int)((videoMode->width - width) * 0.5f);
//         ypos += (int)((videoMode->height - height) * 0.5f);
//         _monitor = nullptr;
//         glfwSetWindowMonitor(_mainWindow, nullptr, xpos, ypos, width, height, GLFW_DONT_CARE);
// #if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
//         // on mac window will sometimes lose title when windowed
//         glfwSetWindowTitle(_mainWindow, _viewName.c_str());
// #endif
    }
}

int GLViewImpl::getMonitorCount() const {
    int count = 0;
    //glfwGetMonitors(&count);
    return count;
}

Size GLViewImpl::getMonitorSize() const {
//     GLFWmonitor* monitor = _monitor;
//     if (nullptr == monitor) {
//         GLFWwindow* window = this->getWindow();
//         monitor = glfwGetWindowMonitor(window);
//     }
//     if (nullptr == monitor) {
//         monitor = glfwGetPrimaryMonitor();
//     }
//     if (nullptr != monitor) {
//         const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
//         Size size = Size((float)videoMode->width, (float)videoMode->height);
//         return size;
//     }
    return Size::ZERO;
}

void GLViewImpl::updateFrameSize()
{
    if (_screenSize.width > 0 && _screenSize.height > 0)
    {
        int w = 0, h = 0;
        //glfwGetWindowSize(_mainWindow, &w, &h);

        int frameBufferW = 0, frameBufferH = 0;
        //glfwGetFramebufferSize(_mainWindow, &frameBufferW, &frameBufferH);

        // if (frameBufferW == 2 * w && frameBufferH == 2 * h)
        // {
        //     if (_isRetinaEnabled)
        //     {
        //         _retinaFactor = 1;
        //     }
        //     else
        //     {
        //         _retinaFactor = 2;
        //     }
        //     glfwSetWindowSize(_mainWindow, _screenSize.width/2 * _retinaFactor * _frameZoomFactor, _screenSize.height/2 * _retinaFactor * _frameZoomFactor);

        //     _isInRetinaMonitor = true;
        // }
        // else
        {
            if (_isInRetinaMonitor)
            {
                _retinaFactor = 1;
            }
            //glfwSetWindowSize(_mainWindow, (int)(_screenSize.width * _retinaFactor * _frameZoomFactor), (int)(_screenSize.height *_retinaFactor * _frameZoomFactor));

            _isInRetinaMonitor = false;
        }
    }
}

void GLViewImpl::setFrameSize(float width, float height)
{
    GLView::setFrameSize(width, height);
    updateFrameSize();
}

void GLViewImpl::setViewPortInPoints(float x , float y , float w , float h)
{
    Viewport vp;
    vp.x = (int)(x * _scaleX * _retinaFactor * _frameZoomFactor + _viewPortRect.origin.x * _retinaFactor * _frameZoomFactor);
    vp.y = (int)(y * _scaleY * _retinaFactor  * _frameZoomFactor + _viewPortRect.origin.y * _retinaFactor * _frameZoomFactor);
    vp.w = (unsigned int)(w * _scaleX * _retinaFactor * _frameZoomFactor);
    vp.h = (unsigned int)(h * _scaleY * _retinaFactor * _frameZoomFactor);
    Camera::setDefaultViewport(vp);
}

void GLViewImpl::setScissorInPoints(float x , float y , float w , float h)
{
    auto x1 = (int)(x * _scaleX * _retinaFactor * _frameZoomFactor + _viewPortRect.origin.x * _retinaFactor * _frameZoomFactor);
    auto y1 = (int)(y * _scaleY * _retinaFactor  * _frameZoomFactor + _viewPortRect.origin.y * _retinaFactor * _frameZoomFactor);
    auto width1 = (unsigned int)(w * _scaleX * _retinaFactor * _frameZoomFactor);
    auto height1 = (unsigned int)(h * _scaleY * _retinaFactor * _frameZoomFactor);
    auto renderer = Director::getInstance()->getRenderer();
    renderer->setScissorRect(x1, y1, width1, height1);

}

Rect GLViewImpl::getScissorRect() const
{
    auto renderer = Director::getInstance()->getRenderer();
    auto& rect = renderer->getScissorRect();

    float x = (rect.x - _viewPortRect.origin.x * _retinaFactor * _frameZoomFactor) / (_scaleX * _retinaFactor * _frameZoomFactor);
    float y = (rect.y - _viewPortRect.origin.y * _retinaFactor * _frameZoomFactor) / (_scaleY * _retinaFactor  * _frameZoomFactor);
    float w = rect.width / (_scaleX * _retinaFactor * _frameZoomFactor);
    float h = rect.height / (_scaleY * _retinaFactor  * _frameZoomFactor);
    return Rect(x, y, w, h);
}

void GLViewImpl::onGLFWError(int errorID, const char* errorDesc)
{
//     if (_mainWindow)
//     {
//         _glfwError = StringUtils::format("GLFWError #%d Happen, %s", errorID, errorDesc);
//     }
//     else
//     {
//         _glfwError.append(StringUtils::format("GLFWError #%d Happen, %s\n", errorID, errorDesc));
//     }
//     CCLOGERROR("%s", _glfwError.c_str());
}

void GLViewImpl::onGLFWMouseCallBack(Urho3D::MouseButton button, bool isPressed, int /*modify*/)
{
    if(Urho3D::MOUSEB_LEFT == button)
    {
        if(isPressed)
        {
            _captured = true;
            if (this->getViewPortRect().equals(Rect::ZERO) || this->getViewPortRect().containsPoint(Vec2(_mouseX,_mouseY)))
            {
                intptr_t id = 0;
                this->handleTouchesBegin(1, &id, &_mouseX, &_mouseY);
            }
        }
        else
        {
            if (_captured)
            {
                _captured = false;
                intptr_t id = 0;
                this->handleTouchesEnd(1, &id, &_mouseX, &_mouseY);
            }
        }
    }
    
    //Because OpenGL and cocos2d-x uses different Y axis, we need to convert the coordinate here
    float cursorX = (_mouseX - _viewPortRect.origin.x) / _scaleX;
    float cursorY = (_viewPortRect.origin.y + _viewPortRect.size.height - _mouseY) / _scaleY;

    if(isPressed)
    {
        EventMouse event(EventMouse::MouseEventType::MOUSE_DOWN);
        event.setCursorPosition(cursorX, cursorY);
        event.setMouseButton(static_cast<cocos2d::EventMouse::MouseButton>(button));
        Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    }
    else
    {
        EventMouse event(EventMouse::MouseEventType::MOUSE_UP);
        event.setCursorPosition(cursorX, cursorY);
        event.setMouseButton(static_cast<cocos2d::EventMouse::MouseButton>(button));
        Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    }
}

void GLViewImpl::onGLFWMouseMoveCallBack(double x, double y)
{
    _mouseX = (float)x;
    _mouseY = (float)y;

    _mouseX /= this->getFrameZoomFactor();
    _mouseY /= this->getFrameZoomFactor();

    if (_isInRetinaMonitor)
    {
        if (_retinaFactor == 1)
        {
            _mouseX *= 2;
            _mouseY *= 2;
        }
    }

    if (_captured)
    {
        intptr_t id = 0;
        this->handleTouchesMove(1, &id, &_mouseX, &_mouseY);
    }
    
    //Because OpenGL and cocos2d-x uses different Y axis, we need to convert the coordinate here
    float cursorX = (_mouseX - _viewPortRect.origin.x) / _scaleX;
    float cursorY = (_viewPortRect.origin.y + _viewPortRect.size.height - _mouseY) / _scaleY;

    EventMouse event(EventMouse::MouseEventType::MOUSE_MOVE);
    // Set current button
//     if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
//     {
//         event.setMouseButton(static_cast<cocos2d::EventMouse::MouseButton>(GLFW_MOUSE_BUTTON_LEFT));
//     }
//     else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
//     {
//         event.setMouseButton(static_cast<cocos2d::EventMouse::MouseButton>(GLFW_MOUSE_BUTTON_RIGHT));
//     }
//     else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
//     {
//         event.setMouseButton(static_cast<cocos2d::EventMouse::MouseButton>(GLFW_MOUSE_BUTTON_MIDDLE));
//     }
    event.setCursorPosition(cursorX, cursorY);
    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void GLViewImpl::onGLFWMouseScrollCallback(double x, double y)
{
    EventMouse event(EventMouse::MouseEventType::MOUSE_SCROLL);
    //Because OpenGL and cocos2d-x uses different Y axis, we need to convert the coordinate here
    float cursorX = (_mouseX - _viewPortRect.origin.x) / _scaleX;
    float cursorY = (_viewPortRect.origin.y + _viewPortRect.size.height - _mouseY) / _scaleY;
    event.setScrollData((float)x, -(float)y);
    event.setCursorPosition(cursorX, cursorY);
    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void GLViewImpl::onGLFWKeyCallback(int key, int /*scancode*/, bool isPressed, int /*mods*/)
{
    if (true)
    {
        EventKeyboard event(g_keyCodeMap[key], isPressed);
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->dispatchEvent(&event);
    }

    if (isPressed)
    {
        switch (g_keyCodeMap[key])
        {
        case EventKeyboard::KeyCode::KEY_BACKSPACE:
            IMEDispatcher::sharedDispatcher()->dispatchDeleteBackward();
            break;
        case EventKeyboard::KeyCode::KEY_HOME:
        case EventKeyboard::KeyCode::KEY_KP_HOME:
        case EventKeyboard::KeyCode::KEY_DELETE:
        case EventKeyboard::KeyCode::KEY_KP_DELETE:
        case EventKeyboard::KeyCode::KEY_END:
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case EventKeyboard::KeyCode::KEY_ESCAPE:
            IMEDispatcher::sharedDispatcher()->dispatchControlKey(g_keyCodeMap[key]);
            break;
        default:
            break;
        }
    }
}

void GLViewImpl::onGLFWCharCallback(unsigned int character)
{
    char16_t wcharString[2] = { (char16_t) character, 0 };
    std::string utf8String;

    StringUtils::UTF16ToUTF8( wcharString, utf8String );
    static std::set<std::string> controlUnicode = {
        "\xEF\x9C\x80", // up
        "\xEF\x9C\x81", // down
        "\xEF\x9C\x82", // left
        "\xEF\x9C\x83", // right
        "\xEF\x9C\xA8", // delete
        "\xEF\x9C\xA9", // home
        "\xEF\x9C\xAB", // end
        "\xEF\x9C\xAC", // pageup
        "\xEF\x9C\xAD", // pagedown
        "\xEF\x9C\xB9"  // clear
    };
    // Check for send control key
    if (controlUnicode.find(utf8String) == controlUnicode.end())
    {
        IMEDispatcher::sharedDispatcher()->dispatchInsertText( utf8String.c_str(), utf8String.size() );
    }
}

void GLViewImpl::onGLFWCharCallback(const char* utf8String, size_t len)
{
    static std::set<std::string> controlUnicode = {
        "\xEF\x9C\x80", // up
        "\xEF\x9C\x81", // down
        "\xEF\x9C\x82", // left
        "\xEF\x9C\x83", // right
        "\xEF\x9C\xA8", // delete
        "\xEF\x9C\xA9", // home
        "\xEF\x9C\xAB", // end
        "\xEF\x9C\xAC", // pageup
        "\xEF\x9C\xAD", // pagedown
        "\xEF\x9C\xB9" // clear
    };
    // Check for send control key
    if (controlUnicode.find(utf8String) == controlUnicode.end())
    {
        IMEDispatcher::sharedDispatcher()->dispatchInsertText(utf8String, len);
    }
}

void GLViewImpl::onGLFWWindowPosCallback(int /*x*/, int /*y*/)
{
    Director::getInstance()->setViewport();
}

void GLViewImpl::onGLFWframebuffersize(int w, int h)
{
    float frameSizeW = _screenSize.width;
    float frameSizeH = _screenSize.height;
    float factorX = frameSizeW / w * _retinaFactor * _frameZoomFactor;
    float factorY = frameSizeH / h * _retinaFactor * _frameZoomFactor;

    if (std::abs(factorX - 0.5f) < FLT_EPSILON && std::abs(factorY - 0.5f) < FLT_EPSILON)
    {
        _isInRetinaMonitor = true;
        if (_isRetinaEnabled)
        {
            _retinaFactor = 1;
        }
        else
        {
            _retinaFactor = 2;
        }

        //glfwSetWindowSize(window, static_cast<int>(frameSizeW * 0.5f * _retinaFactor * _frameZoomFactor), static_cast<int>(frameSizeH * 0.5f * _retinaFactor * _frameZoomFactor));
    }
    else if (std::abs(factorX - 2.0f) < FLT_EPSILON && std::abs(factorY - 2.0f) < FLT_EPSILON)
    {
        _isInRetinaMonitor = false;
        _retinaFactor = 1;
        //glfwSetWindowSize(window, static_cast<int>(frameSizeW * _retinaFactor * _frameZoomFactor), static_cast<int>(frameSizeH * _retinaFactor * _frameZoomFactor));
    }
}

void GLViewImpl::onGLFWWindowSizeFunCallback(int width, int height)
{
    if (width && height && _resolutionPolicy != ResolutionPolicy::UNKNOWN)
    {
        Size baseDesignSize = _designResolutionSize;
        ResolutionPolicy baseResolutionPolicy = _resolutionPolicy;

        float frameWidth = width / _frameZoomFactor;
        float frameHeight = height / _frameZoomFactor;
        setFrameSize(frameWidth, frameHeight);
        setDesignResolutionSize(baseDesignSize.width, baseDesignSize.height, baseResolutionPolicy);
        Director::getInstance()->setViewport();
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(GLViewImpl::EVENT_WINDOW_RESIZED, nullptr);
    }
}

void GLViewImpl::onGLFWWindowIconifyCallback(int iconified)
{
//     if (iconified == GL_TRUE)
//     {
//         Application::getInstance()->applicationDidEnterBackground();
//     }
//     else
//     {
//         Application::getInstance()->applicationWillEnterForeground();
//     }
}

void GLViewImpl::onGLFWWindowFocusCallback(int focused)
{
    if (focused)
    {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(GLViewImpl::EVENT_WINDOW_FOCUSED, nullptr);
    }
    else
    {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(GLViewImpl::EVENT_WINDOW_UNFOCUSED, nullptr);
    }
}

// #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
// static bool glew_dynamic_binding()
// {
//     const char *gl_extensions = (const char*)glGetString(GL_EXTENSIONS);
// 
//     // If the current opengl driver doesn't have framebuffers methods, check if an extension exists
//     if (glGenFramebuffers == nullptr)
//     {
//         log("OpenGL: glGenFramebuffers is nullptr, try to detect an extension");
//         if (strstr(gl_extensions, "ARB_framebuffer_object"))
//         {
//             log("OpenGL: ARB_framebuffer_object is supported");
// 
//             glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) wglGetProcAddress("glIsRenderbuffer");
//             glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) wglGetProcAddress("glBindRenderbuffer");
//             glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) wglGetProcAddress("glDeleteRenderbuffers");
//             glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) wglGetProcAddress("glGenRenderbuffers");
//             glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) wglGetProcAddress("glRenderbufferStorage");
//             glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) wglGetProcAddress("glGetRenderbufferParameteriv");
//             glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) wglGetProcAddress("glIsFramebuffer");
//             glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) wglGetProcAddress("glBindFramebuffer");
//             glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) wglGetProcAddress("glDeleteFramebuffers");
//             glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) wglGetProcAddress("glGenFramebuffers");
//             glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) wglGetProcAddress("glCheckFramebufferStatus");
//             glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC) wglGetProcAddress("glFramebufferTexture1D");
//             glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) wglGetProcAddress("glFramebufferTexture2D");
//             glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) wglGetProcAddress("glFramebufferTexture3D");
//             glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) wglGetProcAddress("glFramebufferRenderbuffer");
//             glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) wglGetProcAddress("glGetFramebufferAttachmentParameteriv");
//             glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) wglGetProcAddress("glGenerateMipmap");
//         }
//         else
//         if (strstr(gl_extensions, "EXT_framebuffer_object"))
//         {
//             log("OpenGL: EXT_framebuffer_object is supported");
//             glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) wglGetProcAddress("glIsRenderbufferEXT");
//             glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) wglGetProcAddress("glBindRenderbufferEXT");
//             glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) wglGetProcAddress("glDeleteRenderbuffersEXT");
//             glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) wglGetProcAddress("glGenRenderbuffersEXT");
//             glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) wglGetProcAddress("glRenderbufferStorageEXT");
//             glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) wglGetProcAddress("glGetRenderbufferParameterivEXT");
//             glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) wglGetProcAddress("glIsFramebufferEXT");
//             glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) wglGetProcAddress("glBindFramebufferEXT");
//             glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) wglGetProcAddress("glDeleteFramebuffersEXT");
//             glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) wglGetProcAddress("glGenFramebuffersEXT");
//             glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) wglGetProcAddress("glCheckFramebufferStatusEXT");
//             glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC) wglGetProcAddress("glFramebufferTexture1DEXT");
//             glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) wglGetProcAddress("glFramebufferTexture2DEXT");
//             glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) wglGetProcAddress("glFramebufferTexture3DEXT");
//             glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) wglGetProcAddress("glFramebufferRenderbufferEXT");
//             glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
//             glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) wglGetProcAddress("glGenerateMipmapEXT");
//         }
//         else
//         {
//             log("OpenGL: No framebuffers extension is supported");
//             log("OpenGL: Any call to Fbo will crash!");
//             return false;
//         }
//     }
//     return true;
// }
// #endif

// helper
bool GLViewImpl::initGlew()
{

// #if (CC_TARGET_PLATFORM != CC_PLATFORM_MAC)
//     GLenum GlewInitResult = glewInit();
//     if (GLEW_OK != GlewInitResult)
//     {
//         ccMessageBox((char *)glewGetErrorString(GlewInitResult), "OpenGL error");
//         return false;
//     }
// 
//     if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
//     {
//         log("Ready for GLSL");
//     }
//     else
//     {
//         log("Not totally ready :(");
//     }
// 
//     if (glewIsSupported("GL_VERSION_2_0"))
//     {
//         log("Ready for OpenGL 2.0");
//     }
//     else
//     {
//         log("OpenGL 2.0 not supported");
//     }
// 
// 
// #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//     if(glew_dynamic_binding() == false)
//     {
//         ccMessageBox("No OpenGL framebuffer support. Please upgrade the driver of your video card.", "OpenGL error");
//         return false;
//     }
// #endif //#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
// 
// #endif //#if (CC_TARGET_PLATFORM != CC_PLATFORM_MAC)

    return true;
}

NS_CC_END // end of namespace cocos2d;
