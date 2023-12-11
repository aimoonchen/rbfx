#include "FairyGUIImpl.h"
#include <map>
#include <vector>
#include "../FairyGUI/UIPackage.h"
#include "../FairyGUI/GComponent.h"
#include "../FairyGUI/GButton.h"
#include "../FairyGUI/GRoot.h"
#include "../FairyGUI/GTextInput.h"
#include "../FairyGUI/tween/GTween.h"
#include "../FairyGUI/tween/GTweener.h"
#include "../FairyGUI/event/UIEventDispatcher.h"
#include "../Cocos2D/Urho3DContext.h"
#include "../Cocos2D/platform/CCFileUtils.h"
#include "../Cocos2D/platform/CCGLViewImpl.h"
#include "../Cocos2D/2d/CCScene.h"
#include "../Cocos2D/renderer/CCRenderer.h"
#include "../Core/Context.h"
#include "../Graphics/Graphics.h"
#include "../Resource/ResourceCache.h"
#include "../IO/FileSystem.h"
#include "../FairyGUI/UIConfig.h"
#include "../RenderAPI/RenderDevice.h"

#include <SDL.h>

#undef M_PI

using namespace fairygui;

// class Joystick : public UIEventDispatcher
// {
// public:
//     static const int MOVE = 100;
//     static const int END = 101;
// 
//     static Joystick* create(GComponent* mainView);
//     float getRotation() const;
//     int getTouchID() const { return touchId; }
//     bool isCapture() const { return !_outside/*_capture*/; }
//     ~Joystick();
// private:
//     bool init(GComponent* mainView);
// 
//     void onTouchBegin(EventContext* context);
//     void onTouchMove(EventContext* context);
//     void onTouchEnd(EventContext* context);
// 
//     float _InitX;
//     float _InitY;
//     float _startStageX;
//     float _startStageY;
//     float _lastStageX;
//     float _lastStageY;
//     int _radius;
// 
//     GButton* _button{ nullptr };
//     GObject* _touchArea{ nullptr };
//     GObject* _thumb{ nullptr };
//     GObject* _center{ nullptr };
//     GTweener* _tweener{ nullptr };
//     int touchId{ -1 };
// 
//     bool _capture{false};
//     bool _outside{true};
// };
// 
// Joystick* Joystick::create(GComponent* mainView)
// {
//     Joystick* pRet = new (std::nothrow) Joystick();
//     if (pRet && pRet->init(mainView))
//     {
//         pRet->autorelease();
//         return pRet;
//     }
//     else
//     {
//         delete pRet;
//         pRet = nullptr;
//         return nullptr;
//     }
// }
// 
// float Joystick::getRotation() const { return _thumb->getRotation()/* - 90.0f*/; }
// 
// Joystick::~Joystick()
// {
//     _touchArea->removeEventListener(UIEventType::TouchBegin);
//     _touchArea->removeEventListener(UIEventType::TouchMove);
//     _touchArea->removeEventListener(UIEventType::TouchEnd);
// }
// 
// bool Joystick::init(GComponent* mainView)
// {
//     _button = mainView->getChild("joystick")->as<GButton>();
//     _button->setChangeStateOnClick(false);
// 
//     _thumb = _button->getChild("thumb");
//     _touchArea = mainView->getChild("joystick_touch");
//     _center = mainView->getChild("joystick_center");
// 
//     _InitX = _center->getX() + _center->getWidth() / 2;
//     _InitY = _center->getY() + _center->getHeight() / 2;
//     touchId = -1;
//     _radius = 100;
//     //150;
// 
//     _touchArea->addEventListener(UIEventType::TouchBegin, CC_CALLBACK_1(Joystick::onTouchBegin, this));
//     _touchArea->addEventListener(UIEventType::TouchMove, CC_CALLBACK_1(Joystick::onTouchMove, this));
//     _touchArea->addEventListener(UIEventType::TouchEnd, CC_CALLBACK_1(Joystick::onTouchEnd, this));
// 
//     _tweener = nullptr;
// 
//     return true;
// }
// 
// void Joystick::onTouchBegin(EventContext* context)
// {
//     _outside = false;
//     if (touchId == -1) // First touch
//     {
//         InputEvent* evt = (InputEvent*)context->getInput();
//         touchId = evt->getTouchId();
// 
//         if (_tweener != nullptr)
//         {
//             _tweener->kill();
//             _tweener = nullptr;
//         }
// 
//         cocos2d::Vec2 pt = UIRoot->globalToLocal(evt->getPosition());
//         float bx = pt.x;
//         float by = pt.y;
//         _button->setSelected(true);
// 
//         if (bx < 0)
//             bx = 0;
//         else if (bx > _touchArea->getWidth())
//         {
//             bx = _touchArea->getWidth();
//             _outside = true;
//         }
// 
//         if (by > UIRoot->getHeight())
//             by = UIRoot->getHeight();
//         else if (by < _touchArea->getY())
//         {
//             by = _touchArea->getY();
//             _outside = true;
//         }
// 
//         _lastStageX = bx;
//         _lastStageY = by;
//         _startStageX = bx;
//         _startStageY = by;
// 
//         _center->setVisible(true);
//         _center->setPosition(bx - _center->getWidth() / 2, by - _center->getHeight() / 2);
//         _button->setPosition(bx - _button->getWidth() / 2, by - _button->getHeight() / 2);
// 
//         float deltaX = bx - _InitX;
//         float deltaY = by - _InitY;
//         float degrees = atan2(deltaY, deltaX) * 180 / Urho3D::M_PI;
//         _thumb->setRotation(degrees + 90);
// 
//         context->captureTouch();
//         _capture = true;
//     }
// }
// 
// void Joystick::onTouchMove(EventContext* context)
// {
//     InputEvent* evt = (InputEvent*)context->getInput();
//     if (touchId != -1 && evt->getTouchId() == touchId)
//     {
//         cocos2d::Vec2 pt = UIRoot->globalToLocal(evt->getPosition());
//         float bx = pt.x;
//         float by = pt.y;
//         float moveX = bx - _lastStageX;
//         float moveY = by - _lastStageY;
//         _lastStageX = bx;
//         _lastStageY = by;
//         float buttonX = _button->getX() + moveX;
//         float buttonY = _button->getY() + moveY;
// 
//         float offsetX = buttonX + _button->getWidth() / 2 - _startStageX;
//         float offsetY = buttonY + _button->getHeight() / 2 - _startStageY;
// 
//         float rad = atan2(offsetY, offsetX);
//         float degree = rad * 180 / Urho3D::M_PI;
//         _thumb->setRotation(degree + 90);
// 
//         float maxX = _radius * cos(rad);
//         float maxY = _radius * sin(rad);
//         if (std::abs(offsetX) > std::abs(maxX))
//             offsetX = maxX;
//         if (std::abs(offsetY) > std::abs(maxY))
//             offsetY = maxY;
// 
//         buttonX = _startStageX + offsetX;
//         buttonY = _startStageY + offsetY;
//         if (buttonX < 0)
//             buttonX = 0;
//         if (buttonY > UIRoot->getHeight())
//             buttonY = UIRoot->getHeight();
// 
//         _button->setPosition(buttonX - _button->getWidth() / 2, buttonY - _button->getHeight() / 2);
// 
//         dispatchEvent(static_cast<UIEventType>(MOVE), nullptr, cocos2d::Value(degree));
//     }
// }
// 
// void Joystick::onTouchEnd(EventContext* context)
// {
//     _outside = true;
//     InputEvent* evt = (InputEvent*)context->getInput();
//     if (touchId != -1 && evt->getTouchId() == touchId)
//     {
//         touchId = -1;
//         _thumb->setRotation(_thumb->getRotation() + 180);
//         _center->setVisible(false);
// 
//         _tweener = GTween::to(_button->getPosition(),
//                               cocos2d::Vec2(_InitX - _button->getWidth() / 2, _InitY - _button->getHeight() / 2), 0.3f)
//                        ->setTarget(_button, TweenPropType::Position)
//                        ->onComplete([this]() {
//                            _button->setSelected(false);
//                            _thumb->setRotation(0);
//                            _center->setVisible(true);
//                            _center->setPosition(_InitX - _center->getWidth() / 2, _InitY - _center->getHeight() / 2);
//                        });
// 
//         dispatchEvent(static_cast<UIEventType>(END));
//         _capture = false;
//     }
// }

namespace fairygui {
    extern void (*updateIMEPosition)(int, int);
}

namespace Urho3D {
	//static cocos2d::Size designResolutionSize = cocos2d::Size(1136, 640);
    static cocos2d::Size designResolutionSize = cocos2d::Size(1280, 720);
	static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
	static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
	static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);
    void FairyGUIImpl::UpdateIMEPosition(int px, int py)
    {
        //IntVector2 screenPosition = ElementToScreen(cursor_->GetPosition());
        SDL_Rect rect = { px, py, 32, 32/*cursor_->GetSize().x_, cursor_->GetSize().y_*/};
        SDL_SetTextInputRect(&rect);
    }
    FairyGUIImpl::~FairyGUIImpl()
    {
//         if (joystick_) {
//             delete joystick_;
//         }
    }

	void FairyGUIImpl::Initialize(Context* context)
	{
		context_ = context;
		SetUrho3DContext(context);
		
		//cocos_renderder_ = new cocos2d::Renderer(nullptr);
        auto frameSize = context_->GetSubsystem<Urho3D::Graphics>()->GetSize();
        view_impl_ = cocos2d::GLViewImpl::createWithRect("FairyGUI", cocos2d::Rect(0, 0, frameSize.x_, frameSize.y_));
		//view_impl_->setFrameSize(frameSize.x_, frameSize.y_);
		// Set the design resolution
		view_impl_->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::SHOW_ALL);
		/*auto frameSize = glview->getFrameSize();
		// if the frame's height is larger than the height of medium size.
		if (frameSize.height > mediumResolutionSize.height)
		{
			director->setContentScaleFactor(std::min(largeResolutionSize.height / designResolutionSize.height, largeResolutionSize.width / designResolutionSize.width));
		}
		// if the frame's height is larger than the height of small size.
		else if (frameSize.height > smallResolutionSize.height)
		{
			director->setContentScaleFactor(std::min(mediumResolutionSize.height / designResolutionSize.height, mediumResolutionSize.width / designResolutionSize.width));
		}
		// if the frame's height is smaller than the height of medium size.
		else
		{
			director->setContentScaleFactor(std::min(smallResolutionSize.height / designResolutionSize.height, smallResolutionSize.width / designResolutionSize.width));
		}*/
//        fairygui::UIConfig::registerFont("default", "fonts/FZY3JW.TTF");
		cocos2d::Director::getInstance()->setOpenGLView(view_impl_);
		auto scene = cocos2d::Scene::create();
        scene->retain();
//         auto test_sprite = cocos2d::Sprite::create("Textures/UrhoIcon.png");
//         //test_sprite->setPosition(cocos2d::Vec2(frameSize.x_ / 2, frameSize.y_ / 2));
//         scene->addChild(test_sprite);
//         cocos2d::TTFConfig fontConfig;
//         fontConfig.fontFilePath = "fonts/FZY3JW.TTF";
//         fontConfig.fontSize = 36;
//         auto test_label0 = cocos2d::Label::create();
//         test_label0->setTTFConfig(fontConfig);
//         test_label0->setString("HelloWorld 中文测试");
//         test_label0->setPosition(cocos2d::Vec2(frameSize.x_ / 2, frameSize.y_ / 2));
//         test_label0->setColor({255,0,0});
//         scene->addChild(test_label0);
//         auto drawNode = cocos2d::DrawNode::create();
//         scene->addChild(drawNode);
//         drawNode->drawRect(cocos2d::Vec2(150, 150), cocos2d::Vec2(200, 200), cocos2d::Color4F(1, 1, 0, 1));
//         drawNode->drawSolidRect(cocos2d::Vec2(50.0f, 50.0f), cocos2d::Vec2(100.0f, 100.0f), cocos2d::Color4F(1.0f, 1.0f, 0.0f, 1.0f));
//         DrawNodeTest(cocos_scene_);
        current_root_ = fairygui::GRoot::create(scene);
		cocos2d::Director::getInstance()->runWithScene(scene);

        fairygui::updateIMEPosition = &UpdateIMEPosition;
	}
	void FairyGUIImpl::Update(float timeStep)
	{

	}
	void FairyGUIImpl::Render()
    {
        //context_->GetSubsystem<Urho3D::Graphics>()->SetUIMode(true);
		cocos2d::Director::getInstance()->drawScene();
        //context_->GetSubsystem<Urho3D::Graphics>()->SetUIMode(false);
	}
    void FairyGUIImpl::SetRoot(fairygui::GRoot* root)
    {
        current_root_ = root;
        if (root) {
            auto scene = root->displayObject()->getScene();
            cocos2d::Director::getInstance()->replaceScene(scene);
        }
    }

	void FairyGUIImpl::OnWindowSizeChanged()
	{
        if (!current_root_) {
            return;
        }
        auto frameSize = context_->GetSubsystem<Urho3D::Graphics>()->GetSize();
        view_impl_->onGLFWWindowSizeFunCallback(frameSize.x_, frameSize.y_);
        current_root_->onWindowSizeChanged();
	}
	
	void FairyGUIImpl::OnKey(unsigned int key, bool isPress)
	{
//         if (key == KEY_BACKSPACE && isPress) {
//             //TODO: not here
//             GTextInput::deleteChar();
//         }
		view_impl_->onGLFWKeyCallback(key, 0, isPress, 0);
	}

    void FairyGUIImpl::OnTextInput(const eastl::string& str)
	{
//        GTextInput::appendString(str.c_str());
		view_impl_->onGLFWCharCallback(str.c_str(), str.size());
	}

	void FairyGUIImpl::OnMouseButtonDown(const IntVector2& windowCursorPos, MouseButton mouseButtons)
	{
        GTextInput::onLastFocus();
		view_impl_->onGLFWMouseMoveCallBack(windowCursorPos.x_, windowCursorPos.y_);
		view_impl_->onGLFWMouseCallBack(mouseButtons, true, 0);
        // TODO: remove this
        is_focus_ = focus_test(windowCursorPos);
	}

	void FairyGUIImpl::OnMouseButtonUp(const IntVector2& windowCursorPos, MouseButton mouseButtons)
	{
		view_impl_->onGLFWMouseCallBack(mouseButtons, false, 0);
        // TODO: remove this
        is_focus_ = focus_test(windowCursorPos);
	}

	void FairyGUIImpl::OnMouseMove(float x, float y)
	{
		view_impl_->onGLFWMouseMoveCallBack(x, y);
        // TODO: remove this
        is_focus_ = focus_test(x, y);
	}

//     void FairyGUIImpl::CreateJoystick(fairygui::GComponent* view)
//     {
//         //joystick_.reset(Joystick::create(view));
//         joystick_ = Joystick::create(view);
//     }
// 
//     float FairyGUIImpl::GetJoystickRotation() const
//     {
//         return joystick_ ? joystick_->getRotation() : 0;
//     }
//     int FairyGUIImpl::GetJoystickTouchID() const
//     {
//         return joystick_ ? joystick_->getTouchID() : -1;
//     }
//     bool FairyGUIImpl::IsJoystickCapture() const
//     {
//         return joystick_ ? joystick_->isCapture() : false;
//     }
    void FairyGUIImpl::SetDesignResolutionSize(int w, int h) {
        view_impl_->setDesignResolutionSize(w, h, ResolutionPolicy::SHOW_ALL);
    }

    bool FairyGUIImpl::IsFocusOnUI() const
    {
        return is_focus_;
    }
    bool FairyGUIImpl::IsInputing() const
    {
        return GTextInput::focus_input_ != nullptr;
    }
    bool FairyGUIImpl::focus_test(const IntVector2& curorPos) { return focus_test(curorPos.x_, curorPos.y_); }

    bool FairyGUIImpl::focus_test(float x, float y)
    {
        if (!current_root_) {
            return false;
        }
        static auto viewPortRect = view_impl_->getViewPortRect();
        static auto scaleX = view_impl_->getScaleX();
        static auto scaleY = view_impl_->getScaleY();
        float cursorX = (x - viewPortRect.origin.x) / scaleX;
        float cursorY = (viewPortRect.origin.y + viewPortRect.size.height - y) / scaleY;
        auto hit = current_root_->hitTest({cursorX, cursorY}, nullptr);
        return hit && hit != current_root_->getChildAt(0);
    }
}
