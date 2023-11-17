#pragma once
#include <EASTL/string.h>
#include <EASTL/unique_ptr.h>
#include <memory>
#include "../Input/InputConstants.h"

namespace fairygui
{
	class GRoot;
	class GComponent;
}
namespace cocos2d
{
    class Node;
	class Scene;
	class Renderer;
    class GLViewImpl;
}

class Joystick;

namespace Urho3D {
	class Context;
	class IntVector2;
	class FairyGUIImpl
	{
	public:
		~FairyGUIImpl();
		void Initialize(Context* context);
		void Update(float timeStep);
		void Render();
		fairygui::GRoot* GetFairyGUIRoot() const { return current_root_; }
        void SetRoot(fairygui::GRoot* root);
		void OnWindowSizeChanged();
		void OnMouseButtonDown(const IntVector2& windowCursorPos, MouseButton mouseButtons);
		void OnMouseButtonUp(const IntVector2& windowCursorPos, MouseButton mouseButtons);
		void OnMouseMove(float x, float y);
        void OnKey(unsigned int key, bool isPress);
        void OnTextInput(const eastl::string& str);
        static void UpdateIMEPosition(Urho3D::IntVector2 screenPosition);
		// TODO : implement in lua
// 		void CreateJoystick(fairygui::GComponent* view);
// 		float GetJoystickRotation() const;
//         int GetJoystickTouchID() const;
// 		bool IsJoystickCapture() const;
        bool IsFocusOnUI() const;
        bool IsInputing() const;
        void SetDesignResolutionSize(int w, int h);
	private:
        bool focus_test(const IntVector2& curorPos);
        bool focus_test(float x, float y);
		fairygui::GRoot* current_root_{ nullptr };
		cocos2d::Renderer* cocos_renderder_{ nullptr };
        cocos2d::GLViewImpl* view_impl_{nullptr};
		Context* context_{ nullptr };
		float _scaleX{ 1.0f };
		float _scaleY{ 1.0f };
		// TODO : implement in lua？
		//ea::unique_ptr<Joystick> joystick_{ nullptr };
        //Joystick* joystick_{ nullptr };
        bool is_focus_{ false };
        cocos2d::Node* test_node_{nullptr};
	};
}
