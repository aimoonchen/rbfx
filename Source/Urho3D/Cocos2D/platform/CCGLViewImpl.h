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

#pragma once

#include "platform/CCGL.h" 
#include "base/CCRef.h"
#include "platform/CCGLView.h"
#include "../../Input/InputConstants.h"
// #include "glfw3.h"
// 
// #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
// #ifndef GLFW_EXPOSE_NATIVE_WIN32
// #define GLFW_EXPOSE_NATIVE_WIN32
// #endif
// #ifndef GLFW_EXPOSE_NATIVE_WGL
// #define GLFW_EXPOSE_NATIVE_WGL
// #endif
// #include "glfw3native.h"
// #endif /* (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) */
// 
// #if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
// #ifndef GLFW_EXPOSE_NATIVE_NSGL
// #define GLFW_EXPOSE_NATIVE_NSGL
// #endif
// #ifndef GLFW_EXPOSE_NATIVE_COCOA
// #define GLFW_EXPOSE_NATIVE_COCOA
// #endif
// #include "glfw3native.h"
// #endif // #if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
namespace Urho3D
{
class Graphics;
}
NS_CC_BEGIN


class CC_DLL GLViewImpl : public GLView
{
public:
    static GLViewImpl* create(const std::string& viewName);
    static GLViewImpl* create(const std::string& viewName, bool resizable);
    static GLViewImpl* createWithRect(const std::string& viewName, Rect size, float frameZoomFactor = 1.0f, bool resizable = false);
    static GLViewImpl* createWithFullScreen(const std::string& viewName);
 //   static GLViewImpl* createWithFullScreen(const std::string& viewName, const GLFWvidmode &videoMode, GLFWmonitor *monitor);

    /*
     *frameZoomFactor for frame. This method is for debugging big resolution (e.g.new ipad) app on desktop.
     */

    //void resize(int width, int height);

    float getFrameZoomFactor() const override;
    //void centerWindow();

    virtual void setViewPortInPoints(float x , float y , float w , float h) override;
    virtual void setScissorInPoints(float x , float y , float w , float h) override;
    virtual Rect getScissorRect() const override;

    bool windowShouldClose() override;
    void pollEvents() override;
//    GLFWwindow* getWindow() const { return _mainWindow; }

    bool isFullscreen() const;
    void setFullscreen();
    void setFullscreen(int monitorIndex);
//    void setFullscreen(const GLFWvidmode &videoMode, GLFWmonitor *monitor);
    void setWindowed(int width, int height);
    int getMonitorCount() const;
    Size getMonitorSize() const;

    /* override functions */
    virtual bool isOpenGLReady() override;
    virtual void end() override;
    virtual void swapBuffers() override;
    virtual void setFrameSize(float width, float height) override;
    virtual void setIMEKeyboardState(bool bOpen) override;

#if CC_ICON_SET_SUPPORT
    virtual void setIcon(const std::string& filename) const override;
    virtual void setIcon(const std::vector<std::string>& filelist) const override;
    virtual void setDefaultIcon() const override;
#endif /* CC_ICON_SET_SUPPORT */

    /*
     * Set zoom factor for frame. This method is for debugging big resolution (e.g.new ipad) app on desktop.
     */
    void setFrameZoomFactor(float zoomFactor) override;
    /**
     * Hide or Show the mouse cursor if there is one.
     */
    virtual void setCursorVisible(bool isVisible) override;
    /** Retina support is disabled by default
     *  @note This method is only available on Mac.
     */
    void enableRetina(bool enabled);
    /** Check whether retina display is enabled. */
    bool isRetinaEnabled() const { return _isRetinaEnabled; };
    
    /** Get retina factor */
    int getRetinaFactor() const override { return _retinaFactor; }
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    HWND getWin32Window() { return 0/*glfwGetWin32Window(_mainWindow)*/; }
#endif /* (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) */
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    id getCocoaWindow() override { return glfwGetCocoaWindow(_mainWindow); }
    id getNSGLContext() override { return glfwGetNSGLContext(_mainWindow); } // stevetranby: added
#endif // #if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)

//protected:
public:
    GLViewImpl(bool initglfw = true);
    virtual ~GLViewImpl();

    bool initGlew();

    bool initWithRect(const std::string& viewName, Rect rect, float frameZoomFactor, bool resizable);
    bool initWithFullScreen(const std::string& viewName);
    //bool initWithFullscreen(const std::string& viewname, const GLFWvidmode &videoMode, GLFWmonitor *monitor);

    void updateFrameSize();

    // GLFW callbacks
    void onGLFWError(int errorID, const char* errorDesc);
    void onGLFWMouseCallBack(Urho3D::MouseButton button, bool isPressed, int modify);
    void onGLFWMouseMoveCallBack(double x, double y);
    void onGLFWMouseScrollCallback(double x, double y);
    void onGLFWKeyCallback(int key, int scancode, bool isPressed, int mods);
    void onGLFWCharCallback(unsigned int character);
    void onGLFWCharCallback(const char* utf8String, size_t character);
    void onGLFWWindowPosCallback(int x, int y);
    void onGLFWframebuffersize(int w, int h);
    void onGLFWWindowSizeFunCallback(int width, int height);
    void onGLFWWindowIconifyCallback(int iconified);
    void onGLFWWindowFocusCallback(int focused);

    bool _captured;
    bool _supportTouch;
    bool _isInRetinaMonitor;
    bool _isRetinaEnabled;
    int  _retinaFactor;  // Should be 1 or 2

    float _frameZoomFactor;

//     GLFWwindow* _mainWindow;
//     GLFWmonitor* _monitor;
    Urho3D::Graphics* _graphics{ nullptr };
    std::string _glfwError;

    float _mouseX;
    float _mouseY;

    friend class GLFWEventHandler;
    
public:
    // View will trigger an event when window is resized, gains or loses focus
    static const std::string EVENT_WINDOW_RESIZED;
    static const std::string EVENT_WINDOW_FOCUSED;
    static const std::string EVENT_WINDOW_UNFOCUSED;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(GLViewImpl);
};


// class CC_DLL GLFWEventHandler
// {
// public:
//     static void onGLFWError(int errorID, const char* errorDesc)
//     {
//         if (_view)
//             _view->onGLFWError(errorID, errorDesc);
//     }
// 
//     static void onGLFWMouseCallBack(Urho3D::MouseButton button, int action, int modify)
//     {
//         if (_view)
//             _view->onGLFWMouseCallBack(button, action, modify);
//     }
// 
//     static void onGLFWMouseMoveCallBack(double x, double y)
//     {
//         if (_view)
//             _view->onGLFWMouseMoveCallBack(x, y);
//     }
// 
//     static void onGLFWMouseScrollCallback(double x, double y)
//     {
//         if (_view)
//             _view->onGLFWMouseScrollCallback(x, y);
//     }
// 
//     static void onGLFWKeyCallback(int key, int scancode, int action, int mods)
//     {
//         if (_view)
//             _view->onGLFWKeyCallback(key, scancode, action, mods);
//     }
// 
//     static void onGLFWCharCallback(unsigned int character)
//     {
//         if (_view)
//             _view->onGLFWCharCallback(character);
//     }
// 
//     static void onGLFWWindowPosCallback(int x, int y)
//     {
//         if (_view)
//             _view->onGLFWWindowPosCallback(x, y);
//     }
// 
//     static void onGLFWWindowSizeFunCallback(int width, int height)
//     {
//         if (_view)
//             _view->onGLFWWindowSizeFunCallback(width, height);
//     }
// 
// 
//     static void onGLFWframebuffersize(int width, int height)
//     {
//         if (_view)
//             _view->onGLFWframebuffersize(width, height);
//     }
// 
//     static void setGLViewImpl(GLViewImpl* view)
//     {
//         _view = view;
//     }
// 
//     static void onGLFWWindowIconifyCallback(int iconified)
//     {
//         if (_view)
//         {
//             _view->onGLFWWindowIconifyCallback(iconified);
//         }
//     }
// 
//     static void onGLFWWindowFocusCallback(int focused)
//     {
//         if (_view)
//         {
//             _view->onGLFWWindowFocusCallback(focused);
//         }
//     }
// 
// 
// private:
//     static GLViewImpl* _view;
// };

NS_CC_END   // end of namespace   cocos2d