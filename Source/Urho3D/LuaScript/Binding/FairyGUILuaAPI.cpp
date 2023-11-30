#if defined(__linux__) && !defined(__ANDROID__)
#else
#include "../../Core/Context.h"
#include "GetPush.h"
#include "../../FairyGUI/UIConfig.h"
#include "../../FairyGUI/UIPackage.h"
#include "../../FairyGUI/GComponent.h"
#include "../../FairyGUI/GButton.h"
#include "../../FairyGUI/GComboBox.h"
#include "../../FairyGUI/GGraph.h"
#include "../../FairyGUI/GGroup.h"
#include "../../FairyGUI/GImage.h"
#include "../../FairyGUI/GLabel.h"
#include "../../FairyGUI/ScrollPane.h"
#include "../../FairyGUI/GList.h"
#include "../../FairyGUI/GLoader.h"
#include "../../FairyGUI/GLoader3D.h"
#include "../../FairyGUI/GMovieClip.h"
#include "../../FairyGUI/GProgressBar.h"
#include "../../FairyGUI/GRichTextField.h"
#include "../../FairyGUI/GRoot.h"
#include "../../FairyGUI/GScrollBar.h"
#include "../../FairyGUI/GSlider.h"
#include "../../FairyGUI/GTextField.h"
#include "../../FairyGUI/GTree.h"
#include "../../FairyGUI/PopupMenu.h"
#include "../../FairyGUI/Controller.h"
#include "../../FairyGUI/event/EventContext.h"
#include "../../FairyGUI/utils/UBBParser.h"
#include "../../FairyGUI/display/TextFormat.h"
#include "../../FairyGUI/DragDropManager.h"
#include "../../FairyGUI/tween/GTween.h"
#include "../../FairyGUI/tween/GTweener.h"
#include "../../Cocos2D/base/CCDirector.h"
#include "../../Cocos2D/base/CCScheduler.h"
#include "../../Cocos2D/base/CCValue.h"
#include "../../Cocos2D/math/CCGeometry.h"
//
#include "../../GUI/Gui.h"

using namespace Urho3D;

USING_NS_FGUI;

// TODO: for examples
class EmojiParser : public UBBParser
{
public:
    static EmojiParser* getInstance() { return _inst; }
    EmojiParser();

protected:
    void onTag_Emoji(const std::string& tagName, bool end, const std::string& attr, std::string& replacement);

private:
    static EmojiParser* _inst;
};

EmojiParser* EmojiParser::_inst = new EmojiParser();

EmojiParser::EmojiParser()
{
    std::vector<std::string> tags(
        {"88", "am", "bs", "bz", "ch", "cool", "dhq", "dn", "fd", "gz", "han", "hx", "hxiao", "hxiu"});

    for (auto& str : tags)
        _handlers[":" + str] = UBB_TAG_HANDLER(EmojiParser::onTag_Emoji, this);
}

void EmojiParser::onTag_Emoji(const std::string& tagName, bool end, const std::string& attr, std::string& replacement)
{
    std::string str = tagName.substr(1);
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    replacement = "<img src='ui://Emoji/" + str + "'/>";
}

namespace sol {
	int sol_lua_push(sol::types<GObject*>, lua_State* L, const GObject* obj) {
		if (obj) {
            switch (obj->typeID) {
            case TypeID::kComponent:
                return sol::make_object(L, static_cast<const GComponent*>(obj)).push(L);
            case TypeID::kGraph:
                return sol::make_object(L, static_cast<const GGraph*>(obj)).push(L);
            case TypeID::kGroup:
                return sol::make_object(L, static_cast<const GGroup*>(obj)).push(L);
            case TypeID::kImage:
                return sol::make_object(L, static_cast<const GImage*>(obj)).push(L);
            case TypeID::kLoader:
                return sol::make_object(L, static_cast<const GLoader*>(obj)).push(L);
            case TypeID::kLoader3D:
                return sol::make_object(L, static_cast<const GLoader3D*>(obj)).push(L);
            case TypeID::kMovieClip:
                return sol::make_object(L, static_cast<const GMovieClip*>(obj)).push(L);
            case TypeID::kTextField:
                return sol::make_object(L, static_cast<const GTextField*>(obj)).push(L);
            case TypeID::kBasicTextField:
                return sol::make_object(L, static_cast<const GBasicTextField*>(obj)).push(L);
            case TypeID::kButton:
                return sol::make_object(L, static_cast<const GButton*>(obj)).push(L);
            case TypeID::kComboBox:
                return sol::make_object(L, static_cast<const GComboBox*>(obj)).push(L);
            case TypeID::kLabel:
                return sol::make_object(L, static_cast<const GLabel*>(obj)).push(L);
            case TypeID::kList:
                return sol::make_object(L, static_cast<const GList*>(obj)).push(L);
            case TypeID::kProgressBar:
                return sol::make_object(L, static_cast<const GProgressBar*>(obj)).push(L);
            case TypeID::kRoot:
                return sol::make_object(L, static_cast<const GRoot*>(obj)).push(L);
            case TypeID::kScrollBar:
                return sol::make_object(L, static_cast<const GScrollBar*>(obj)).push(L);
            case TypeID::kSlider:
                return sol::make_object(L, static_cast<const GSlider*>(obj)).push(L);
            case TypeID::kWindow:
                return sol::make_object(L, static_cast<const Window*>(obj)).push(L);
            case TypeID::kTree:
                return sol::make_object(L, static_cast<const GTree*>(obj)).push(L);
            case TypeID::kRichTextField:
                return sol::make_object(L, static_cast<const GRichTextField*>(obj)).push(L); 
            }
		}
		return sol::make_object(L, obj).push(L);
	}
}

Urho3D::Context* GetContext(lua_State* L);

static void BindUIConfig(sol::state& lua)
{
    auto fairygui = lua["FairyGUI"].get<sol::table>();
    auto uiconfig = fairygui["UIConfig"].get_or_create<sol::table>();
	// uiconfig["horizontalScrollBar"] = std::ref(UIConfig::horizontalScrollBar);
	// uiconfig["verticalScrollBar"] = std::ref(UIConfig::verticalScrollBar);
	//...
}

static void RegisterUIEventTypeConst(sol::state& lua)
{
    auto fairygui  = lua["FairyGUI"].get<sol::table>();
    auto eventType = fairygui["EventType"].get_or_create<sol::table>();
    eventType["Enter"]              = UIEventType::Enter;
    eventType["Exit"]               = UIEventType::Exit;
    eventType["Changed"]            = UIEventType::Changed;
    eventType["Submit"]             = UIEventType::Submit;
    eventType["TouchBegin"]         = UIEventType::TouchBegin;
    eventType["TouchMove"]          = UIEventType::TouchMove;
    eventType["TouchEnd"]           = UIEventType::TouchEnd;
    eventType["Click"]              = UIEventType::Click;
    eventType["RollOver"]           = UIEventType::RollOver;
    eventType["RollOut"]            = UIEventType::RollOut;
    eventType["MouseWheel"]         = UIEventType::MouseWheel;
    eventType["RightClick"]         = UIEventType::RightClick;
    eventType["MiddleClick"]        = UIEventType::MiddleClick;
    eventType["PositionChange"]     = UIEventType::PositionChange;
    eventType["SizeChange"]         = UIEventType::SizeChange;
    eventType["KeyDown"]            = UIEventType::KeyDown;
    eventType["KeyUp"]              = UIEventType::KeyUp;
    eventType["Scroll"]             = UIEventType::Scroll;
    eventType["ScrollEnd"]          = UIEventType::ScrollEnd;
    eventType["PullDownRelease"]    = UIEventType::PullDownRelease;
    eventType["PullUpRelease"]      = UIEventType::PullUpRelease;
    eventType["ClickItem"]          = UIEventType::ClickItem;
    eventType["ClickLink"]          = UIEventType::ClickLink;
    eventType["ClickMenu"]          = UIEventType::ClickMenu;
    eventType["RightClickItem"]     = UIEventType::RightClickItem;
    eventType["DragStart"]          = UIEventType::DragStart;
    eventType["DragMove"]           = UIEventType::DragMove;
    eventType["DragEnd"]            = UIEventType::DragEnd;
    eventType["Drop"]               = UIEventType::Drop;
    eventType["GearStop"]           = UIEventType::GearStop;
}

static void RegisterRelationTypeConst(sol::state& lua)
{
    auto fairygui		= lua["FairyGUI"].get<sol::table>();
    auto relationType	= fairygui["RelationType"].get_or_create<sol::table>();
    relationType["Left_Left"]			= RelationType::Left_Left;
	relationType["Left_Center"]			= RelationType::Left_Center;
	relationType["Left_Right"]			= RelationType::Left_Right;
	relationType["Center_Center"]		= RelationType::Center_Center;
	relationType["Right_Left"]			= RelationType::Right_Left;
	relationType["Right_Center"]		= RelationType::Right_Center;
	relationType["Right_Right"]			= RelationType::Right_Right;
    relationType["Top_Top"]				= RelationType::Top_Top;
    relationType["Top_Middle"]			= RelationType::Top_Middle;
    relationType["Top_Bottom"]			= RelationType::Top_Bottom;
    relationType["Middle_Middle"]		= RelationType::Middle_Middle;
    relationType["Bottom_Top"]			= RelationType::Bottom_Top;
    relationType["Bottom_Middle"]		= RelationType::Bottom_Middle;
    relationType["Bottom_Bottom"]		= RelationType::Bottom_Bottom;
	relationType["Width"]				= RelationType::Width;
    relationType["Height"]				= RelationType::Height;
	relationType["LeftExt_Left"]		= RelationType::LeftExt_Left;
    relationType["LeftExt_Right"]		= RelationType::LeftExt_Right;
    relationType["RightExt_Left"]		= RelationType::RightExt_Left;
    relationType["RightExt_Right"]		= RelationType::RightExt_Right;
    relationType["TopExt_Top"]			= RelationType::TopExt_Top;
    relationType["TopExt_Bottom"]		= RelationType::TopExt_Bottom;
    relationType["BottomExt_Top"]		= RelationType::BottomExt_Top;
    relationType["BottomExt_Bottom"]	= RelationType::BottomExt_Bottom;
    relationType["Size"]                = RelationType::Size;
    auto tweenPropType = fairygui["TweenPropType"].get_or_create<sol::table>();
    tweenPropType["X"]          = TweenPropType::X;
    tweenPropType["Y"]          = TweenPropType::Y;
    tweenPropType["Position"]   = TweenPropType::Position;
    tweenPropType["Width"]      = TweenPropType::Width;
    tweenPropType["Height"]     = TweenPropType::Height;
    tweenPropType["Size"]       = TweenPropType::Size;
    tweenPropType["ScaleX"]     = TweenPropType::ScaleX;
    tweenPropType["ScaleY"]     = TweenPropType::ScaleY;
    tweenPropType["Scale"]      = TweenPropType::Scale;
    tweenPropType["Rotation"]   = TweenPropType::Rotation;
    tweenPropType["Alpha"]      = TweenPropType::Alpha;
    tweenPropType["Progress"]   = TweenPropType::Progress;
}

static cocos2d::Color3B Color3BFromUrhoColor(const Color& color)
{
    return {(uint8_t)Clamp(((int)(color.r_ * 255.0f)), 0, 255), (uint8_t)Clamp(((int)(color.g_ * 255.0f)), 0, 255), (uint8_t)Clamp(((int)(color.b_ * 255.0f)), 0, 255)};
}
static cocos2d::Color4B Color4BFromUrhoColor(const Color& color)
{
    return {(uint8_t)Clamp(((int)(color.r_ * 255.0f)), 0, 255), (uint8_t)Clamp(((int)(color.g_ * 255.0f)), 0, 255), (uint8_t)Clamp(((int)(color.b_ * 255.0f)), 0, 255), (uint8_t)Clamp(((int)(color.a_ * 255.0f)), 0, 255)};
}
static cocos2d::Color4F Color4FFromUrhoColor(const Color& color)
{
    return {color.r_, color.g_, color.b_, color.a_};
}
static cocos2d::Rect FromUrhoRect(const Rect& rect)
{
    return {rect.min_.x_, rect.min_.y_, rect.max_.x_ - rect.min_.x_, rect.max_.y_ - rect.min_.y_};
}
static Rect ToUrhoRect(const cocos2d::Rect& rect)
{
    return {rect.getMinX(), rect.getMinY(), rect.getMaxX(), rect.getMaxY()};
}

// for ui test
class FairyGUIScene : public cocos2d::Scene
{
public:
    CREATE_FUNC(FairyGUIScene);
    bool init() override;

    FairyGUIScene() = default;
    virtual ~FairyGUIScene()
    {
        CC_SAFE_RELEASE(_groot);
    }
    GRoot* _groot{ nullptr };
};

bool FairyGUIScene::init()
{
    if (!Scene::init()) {
        return false;
    }

    _groot = GRoot::create(this);
    _groot->retain();

    return true;
}

int sol2_FairyGUILuaAPI_open(sol::state& lua)
{
    auto fairygui	= lua["FairyGUI"].get_or_create<sol::table>();
	auto uipackage  = fairygui["UIPackage"].get_or_create<sol::table>();
	uipackage["GetById"]				= &UIPackage::getById;
	uipackage["GetByName"]				= &UIPackage::getByName;
	uipackage["AddPackage"]				= &UIPackage::addPackage;
	uipackage["RemovePackage"]			= &UIPackage::removePackage;
	uipackage["RemoveAllPackages"]		= &UIPackage::removeAllPackages;
	uipackage["CreateObject"]			= [](const std::string& pkgName, const std::string& resName) { return UIPackage::createObject(pkgName, resName); };
	uipackage["CreateObjectFromURL"]	= &UIPackage::createObjectFromURL;
    fairygui.new_usertype<EventContext>("EventContext",
        "GetData", [](EventContext* obj) { return (GObject*)obj->getData(); },
        "GetDataAsString", [](EventContext* obj){ return obj->getDataValue().asString();},
        "GetSender", [](EventContext* obj) { return (GObject*)obj->getSender(); },
        "GetTouchId", [](EventContext* self) { return self->getInput()->getTouchId(); },
        "PreventDefault", &EventContext::preventDefault
        );
    fairygui.new_usertype<GController>("GController",
        "GetParent", &GController::getParent,
        "SetParent", &GController::setParent,
        "GetSelectedIndex", &GController::getSelectedIndex,
        "SetSelectedIndex", sol::overload(
            [](GController* self, int value){ self->setSelectedIndex(value); },
            [](GController* self, int value, bool triggerEvent) { self->setSelectedIndex(value, triggerEvent); })
        );
    fairygui.new_usertype<PopupMenu>("PopupMenu",
        "Create", sol::overload(
            []() { return PopupMenu::create(); },
            [](const ea::string& resourceURL) { return PopupMenu::create(resourceURL.c_str()); }),
        "Show", sol::overload(
            [](PopupMenu* self) { self->show(); },
            [](PopupMenu* self, GObject* target, PopupDirection dir) { self->show(target, dir); }),
        "AddItem", [](PopupMenu* self, const ea::string& caption, sol::function func) { self->addItem(caption.c_str(), [func](EventContext* context){ CALL_LUA(func, context) }); }
        );
	fairygui.new_usertype<GObject>("GObject",
        "name", &GObject::name,
        "GetParent", &GObject::getParent,
		"SetPosition", &GObject::setPosition,
        "GetPosition", [](GObject* obj) {
            const auto& cv2 = obj->getPosition();
            //return IntVector2((int32_t)cv2.x, (int32_t)cv2.y);
            return Vector2(cv2.x, cv2.y);
        },
        "SetTouchable", &GObject::setTouchable,
        "SetPivot", [](GObject* obj, float xv, float yv) { obj->setPivot(xv, yv); },
        "GetX", &GObject::getX,
        "GetY", &GObject::getY,
        "GetWidth", &GObject::getWidth,
        "GetHeight", &GObject::getHeight,
        "SetWidth", &GObject::setWidth,
        "SetHeight", &GObject::setHeight,
        "SetRotation", &GObject::setRotation,
        "GetRotation", &GObject::getRotation,
        "SetScale", &GObject::setScale,
        "SetSize", sol::overload(
            [](GObject* self, const Vector2& size) { self->setSize(size.x_, size.y_); },
            [](GObject* self, float w, float h) { self->setSize(w, h); },
            [](GObject* self, float w, float h, bool ignorePivot) { self->setSize(w, h, ignorePivot); }),
		"AddRelation", sol::overload(
            [](GObject* self, GObject* target, RelationType relationType) { self->addRelation(target, relationType); },
            [](GObject* self, GObject* target, RelationType relationType, bool usePercent) { self->addRelation(target, relationType, usePercent); }),
		"SetSortingOrder", &GObject::setSortingOrder,
        "SetDragBounds", [](GObject* obj, const Rect& rect) { obj->setDragBounds(FromUrhoRect(rect)); },
        "AddEventListener", sol::overload(
            [](GObject* obj, UIEventType eventType, sol::function func) { obj->addEventListener(eventType, [func](EventContext* context) { CALL_LUA(func, context) }); },
            [](GObject* obj, UIEventType eventType, sol::function func, int eventTag) { obj->addEventListener(eventType, [func](EventContext* context) { CALL_LUA(func, context) }, EventTag(eventTag)); },
            [](GObject* obj, UIEventType eventType, sol::function func, void* eventTag) { obj->addEventListener(eventType, [func](EventContext* context) { CALL_LUA(func, context) }, EventTag(eventTag));}),
        "RemoveEventListener", sol::overload(
            [](GObject* obj, UIEventType eventType) { obj->removeEventListener(eventType); },
            [](GObject* obj, UIEventType eventType, int eventTag) { obj->removeEventListener(eventType, EventTag(eventTag)); },
            [](GObject* obj, UIEventType eventType, void* eventTag) { obj->removeEventListener(eventType, EventTag(eventTag)); }),
        "RemoveEventListeners", &GObject::removeEventListeners,
        "AddClickListener", sol::overload(
            [](GObject* obj, sol::function func) { obj->addClickListener([func](EventContext* context) { CALL_LUA(func, context) }); },
            [](GObject* obj, sol::function func, int eventTag) { obj->addClickListener([func](EventContext* context) { CALL_LUA(func, context) }, EventTag(eventTag)); },
            [](GObject* obj, sol::function func, void* eventTag) { obj->addClickListener([func](EventContext* context) { CALL_LUA(func, context) }, EventTag(eventTag)); }),
        "RemoveClickListener", sol::overload(
            [](GObject* obj, int eventTag) { obj->removeClickListener(EventTag(eventTag)); },
            [](GObject* obj, void* eventTag) { obj->removeClickListener(EventTag(eventTag)); }),
        "Center",  sol::overload(
            [](GObject* self) { self->center(); },
            [](GObject* self, bool restraint) { self->center(restraint); }),
        "SetText", &GObject::setText,
        "SetIcon", &GObject::setIcon,
        "GetText", &GObject::getText,
        "GetIcon", &GObject::getIcon,
        "SetVisible", &GObject::setVisible,
        "IsVisible", &GObject::isVisible,
        "GetGroup", &GObject::getGroup,
        "SetDraggable", &GObject::setDraggable,
        "GetSize", [](GObject* self) {
            auto size = self->getSize();
            return Vector2{size.width, size.height};
        },
        "TransformRect", [](GObject* self, const Rect& rect, GObject* targetSpace) {
            auto crect = self->transformRect(FromUrhoRect(rect), targetSpace);
            return ToUrhoRect(crect);
        },
        "MakeFullScreen", &GObject::makeFullScreen,
        "RemoveFromParent", &GObject::removeFromParent,
        "GetSourceSize", [](GObject* obj) { return IntVector2{ (int)obj->sourceSize.width, (int)obj->sourceSize.height }; },
        "GetInitSize", [](GObject* obj) { return IntVector2{ (int)obj->initSize.width, (int)obj->initSize.height }; },
        "TreeNode", &GObject::treeNode
	    );
    fairygui.new_usertype<GLoader>("GLoader",
        "GetIcon", &GLoader::getIcon,
        "SetIcon", &GLoader::setIcon,
        sol::base_classes, sol::bases<GObject>()
        );
    fairygui.new_usertype<GTextField>("GTextField",
        "GetText", &GTextField::getText,
        "SetText", &GTextField::setText,
        "SetColor", [](GTextField* obj, uint8_t r, uint8_t g, uint8_t b){ obj->setColor({r, g, b});},
        "SetFontSize", &GTextField::setFontSize,
        "GetFontSize", &GTextField::getFontSize,
        "SetOutlineColor", [](GTextField* obj, uint8_t r, uint8_t g, uint8_t b){ obj->setOutlineColor({r, g, b});},
        "GetTextSize", [](GTextField* obj) { auto size = obj->getTextSize(); return IntVector2{ (int)size.width, (int)size.height }; },
        sol::base_classes, sol::bases<GObject>());

    fairygui.new_usertype<GGraph>("GGraph",
        sol::call_constructor, sol::factories([]() { return GGraph::create(); }),
        "Create", &GGraph::create,
        "DrawRect", [] (GGraph* obj, float aWidth, float aHeight, int lineSize, const Urho3D::Color& lineColor, const Urho3D::Color& fillColor) {
            obj->drawRect(aWidth, aHeight, lineSize, Color4FFromUrhoColor(lineColor), Color4FFromUrhoColor(fillColor)); },
        sol::base_classes, sol::bases<GObject>());

    fairygui.new_usertype<TextFormat>("TextFormat",
        "face", &TextFormat::face);

    fairygui.new_usertype<GBasicTextField>("GBasicTextField",
        "SetAutoSize", &GBasicTextField::setAutoSize,
        "SetSingleLine", &GBasicTextField::setSingleLine,
        "GetTextFormat", &GBasicTextField::getTextFormat,
        "ApplyTextFormat", &GBasicTextField::applyTextFormat,
        "SetText", &GBasicTextField::setText,
        sol::base_classes, sol::bases<GTextField, GObject>());
    
    fairygui.new_usertype<GRichTextField>("GRichTextField",
        "SetAutoSize", &GRichTextField::setAutoSize,
        "SetSingleLine", &GRichTextField::setSingleLine,
        "GetTextFormat", &GRichTextField::getTextFormat,
        "ApplyTextFormat", &GRichTextField::applyTextFormat,
        sol::base_classes, sol::bases<GTextField, GObject>());
    fairygui.new_usertype<Transition>("Transition",
        "Play", sol::overload([](Transition* obj) { obj->play(); },
            [](Transition* obj, sol::function func) { obj->play([func]() { CALL_LUA(func) }); },
            [](Transition* obj, int times, float delay) { obj->play(times, delay); },
            [](Transition* obj, int times, float delay, sol::function func) { obj->play(times, delay, [func]() { CALL_LUA(func) }); },
            [](Transition* obj, int times, float delay, float startTime, float endTime) { obj->play(times, delay, startTime, endTime); }),
        "SetHook", [](Transition* obj, const char* label, sol::function func) { obj->setHook(label, [func]() { CALL_LUA(func) }); }
        );
	fairygui.new_usertype<GComponent>("GComponent",
		"AddChild",     &GComponent::addChild,
        "RemoveChild",  &GComponent::removeChild,
        "RemoveChildren", sol::overload(
            [](GComponent* self) { self->removeChildren(); },
            [](GComponent* self, int beginIndex, int endIndex) { self->removeChildren(beginIndex, endIndex); }),
        "GetChild", &GComponent::getChild,
        "GetChildAt",   &GComponent::getChildAt,
        "GetChildById", [](GComponent* self, const ea::string& id){ return self->getChildById(id.c_str()); },
        "GetController", &GComponent::getController,
        "GetViewWidth", &GComponent::getViewWidth,
        "NumChildren", &GComponent::numChildren,
        "GetTransition", &GComponent::getTransition,
        "IsChildInView", &GComponent::isChildInView,
        "GetFirstChildInView", &GComponent::getFirstChildInView,
        "EnsureBoundsCorrect", &GComponent::ensureBoundsCorrect,
        "IsAncestorOf", &GComponent::isAncestorOf,
        "GetScrollPane", &GComponent::getScrollPane,
		sol::base_classes, sol::bases<GObject>());

    fairygui.new_usertype<GGroup>("GGroup",
        "name", &GObject::name,
		sol::base_classes, sol::bases<GObject>());
    
    fairygui.new_usertype<GImage>("GImage",
        sol::base_classes, sol::bases<GObject>());

    fairygui.new_usertype<GRoot>("GRoot",
        sol::call_constructor, sol::factories([]() { return std::make_unique<GRoot>(); }),
        "ShowPopup", sol::overload(
            [](GRoot* self, GObject* popup) { self->showPopup(popup); },
            [](GRoot* self, GObject* popup, GObject* target, PopupDirection dir) { self->showPopup(popup, target, dir); }),
        "GetTouchTarget", &GRoot::getTouchTarget,
        "ShowModalWait", &GRoot::showModalWait,
        "CloseModalWait", &GRoot::closeModalWait,
        sol::base_classes, sol::bases<GComponent, GObject>()
        );
    fairygui.new_usertype<GButton>("GButton",
        "IsChangeStateOnClick",     &GButton::isChangeStateOnClick,
        "SetChangeStateOnClick",    &GButton::setChangeStateOnClick,
        "SetTitle",                 &GButton::setTitle,
        "SetText",                  &GButton::setText,
        "SetIcon",                  &GButton::setIcon,
        "GetText",                  &GButton::getText,
        "GetIcon",                  &GButton::getIcon,
        sol::base_classes,          sol::bases<GComponent, GObject>());
    fairygui.new_usertype<GProgressBar>("GProgressBar",
        "SetValue", &GProgressBar::setValue,
        "GetValue", &GProgressBar::getValue,
        "SetMin", &GProgressBar::setMin,
        "GetMin", &GProgressBar::getMin,
        "SetMax", &GProgressBar::setMax,
        "GetMax", &GProgressBar::getMax,
        sol::base_classes, sol::bases<GComponent, GObject>()
        );
    fairygui.new_usertype<ScrollPane>("ScrollPane",
        "GetPosX", &ScrollPane::getPosX,
        "GetPosY", &ScrollPane::getPosY,
        "SetPosX", sol::overload([](ScrollPane* obj, float value) { obj->setPosX(value); }, [](ScrollPane* obj, float value, bool ani) { obj->setPosX(value, ani); }),
        "SetPosY", sol::overload([](ScrollPane* obj, float value) { obj->setPosY(value); }, [](ScrollPane* obj, float value, bool ani) { obj->setPosY(value, ani); }),
        "ScrollTop", [](ScrollPane* obj) { obj->scrollTop(); },
        "ScrollBottom", [](ScrollPane* obj) { obj->scrollBottom(); },
        "CancelDragging", &ScrollPane::cancelDragging,
        "LockHeader", &ScrollPane::lockHeader,
        "LockFooter", &ScrollPane::lockFooter,
        "GetHeader", &ScrollPane::getHeader,
        "GetFooter", &ScrollPane::getFooter,
        "IsBottomMost", &ScrollPane::isBottomMost,
        "IsRightMost", & ScrollPane::isRightMost
        );
    fairygui.new_usertype<GList>("GList",
        "SetVirtual", sol::resolve<void()>(&GList::setVirtual),
        "SetVirtualAndLoop", &GList::setVirtualAndLoop,
        "SetNumItems", &GList::setNumItems,
        "GetNumItems", &GList::getNumItems,
        "AddSelection", &GList::addSelection,
        "GetSelectedIndex", &GList::getSelectedIndex,
        "SetSelectedIndex", &GList::setSelectedIndex,
        "SetItemRenderer", [](GList* obj, sol::function func) { obj->itemRenderer = [func](int index, GObject* item) { CALL_LUA(func, index, item) }; },
        "SetItemProvider", [](GList* obj, sol::function func) { obj->itemProvider = [func](int index) { CALL_LUA(func, index) return result.get<std::string>(); }; },
        "AddItemFromPool", sol::overload(sol::resolve<GObject*()>(&GList::addItemFromPool), [](GList* obj, const char* url) { return obj->addItemFromPool(url); }),
        sol::base_classes, sol::bases<GComponent, GObject>());
    fairygui.new_usertype<Window>("Window",
        "Create",           &Window::create,
        "Show",             &Window::show,
        "Hide",             &Window::hide,
        "HideImmediately",  &Window::hideImmediately,
        "SetModal",         &Window::setModal,
        "GetContentPane",   &Window::getContentPane,
        "SetContentPane",   &Window::setContentPane,
        "ShowModalWait",    sol::resolve<void()>(&Window::showModalWait),
        "CloseModalWait",   sol::resolve<bool()>(&Window::closeModalWait),
        sol::base_classes,  sol::bases<GComponent, GObject>());
    fairygui.new_usertype<GTweener>("GTweener",
        "SetTarget", sol::overload([](GTweener* obj, GObject* target) { return obj->setTarget(target); },
            [](GTweener* obj, GObject* target, TweenPropType propType) { return obj->setTarget(target, propType); }),
        "SetRepeat", [](GTweener* obj, int repeat) {obj->setRepeat(repeat); },
        "OnStart", [](GTweener* obj, sol::function func) { obj->onStart([func](GTweener* tweener) { CALL_LUA(func, tweener) }); },
        "OnUpdate", [](GTweener* obj, sol::function func) { obj->onUpdate([func](GTweener* tweener) { CALL_LUA(func, tweener) }); },
        "OnComplete", [](GTweener* obj, sol::function func) { obj->onComplete([func]() { CALL_LUA(func) }); },
        "OnComplete1", [](GTweener* obj, sol::function func) { obj->onComplete1([func](GTweener* tweener) { CALL_LUA(func, tweener) }); },
        "value", [](GTweener* obj) { return Vector4{ obj->value.x, obj->value.y, obj->value.z, obj->value.w }; }
    );
    fairygui.new_usertype<GTween>("GTween",
        "To", sol::overload([](float startValue, float endValue, float duration) { return GTween::to(startValue, endValue, duration); },
            [](const Vector2& startValue, const Vector2& endValue, float duration) { return GTween::to(*(cocos2d::Vec2*)startValue.Data(), *(cocos2d::Vec2*)endValue.Data(), duration); },
            [](const Vector3& startValue, const Vector3& endValue, float duration) { return GTween::to(*(cocos2d::Vec3*)startValue.Data(), *(cocos2d::Vec3*)endValue.Data(), duration); },
            [](const Vector4& startValue, const Vector4& endValue, float duration) { return GTween::to(*(cocos2d::Vec4*)startValue.Data(), *(cocos2d::Vec4*)endValue.Data(), duration); },
            [](const Color& startValue, const Color& endValue, float duration) { return GTween::to(Color4BFromUrhoColor(startValue), Color4BFromUrhoColor(endValue), duration); }),
        "delayedCall", &GTween::delayedCall,
        "Shake", [](const Vector2& startValue, float amplitude, float duration) { return GTween::shake(*(cocos2d::Vec2*)startValue.Data(), amplitude, duration); });
    fairygui.new_usertype<GTreeNode>("GTreeNode",
        "Create", sol::overload([]() { return GTreeNode::create(); }, [](bool isFolder) { return GTreeNode::create(isFolder); }),
        "AddChild", &GTreeNode::addChild,
        "SetData", &GTreeNode::setData,
        "GetData", &GTreeNode::getData,
        "SetTable", &GTreeNode::setTable,
        "GetTable", &GTreeNode::getTable,
        "GetCell", &GTreeNode::getCell,
        "IsFolder", &GTreeNode::isFolder,
        "GetText", &GTreeNode::getText
        );
    fairygui.new_usertype<GTree>("GTree",
        "GetRootNode", &GTree::getRootNode,
        "GetSelectedNode", &GTree::getSelectedNode,
        "SetItemRenderer", [](GTree* obj, sol::function func) { obj->treeNodeRender = [func](GTreeNode* node, GComponent* obj) { CALL_LUA(func, node, obj) }; },
        sol::base_classes, sol::bases<GList, GComponent, GObject>()
        );
    auto context = GetContext(lua.lua_state());
    fairygui["GetRoot"] = [context]() { return context->GetSubsystem<GUI>()->GetFairyGUIRoot(); };
    fairygui["CreateText"] = sol::overload(
        []() { return GBasicTextField::create(); },
        [](const std::string& text) {
            auto t = GBasicTextField::create();
            t->setText(text);
            t->setColor({255, 255, 255});
            return t;},
        [](const std::string& text, const Urho3D::Color& color) {
            auto t = GBasicTextField::create();
            t->setText(text);
            t->setColor(Color3BFromUrhoColor(color));
            return t;
        });
    fairygui["CreateRichText"] = sol::overload(
        []() { return GRichTextField::create(); },
        [](const std::string& text) {
            auto t = GRichTextField::create();
            t->setText(text);
            t->setColor({255, 255, 255});
            return t;},
        [](const std::string& text, const Urho3D::Color& color) {
            auto t = GRichTextField::create();
            t->setText(text);
            t->setColor(Color3BFromUrhoColor(color));
            return t;
        });
    fairygui["CreateButton"] = []() { return GButton::create(); };
    fairygui["RegisterFont"] = &UIConfig::registerFont;
    fairygui["IsFocusUI"] = [context]() { return context->GetSubsystem<GUI>()->IsFocusUI(); };
    fairygui["IsInputing"] = [context]() { return context->GetSubsystem<GUI>()->IsInputing(); };
	BindUIConfig(lua);
    RegisterUIEventTypeConst(lua);
	RegisterRelationTypeConst(lua);
    auto popupDirection = fairygui["PopupDirection"].get_or_create<sol::table>();
    popupDirection["AUTO"] = PopupDirection::DOWN;
    popupDirection["UP"] = PopupDirection::UP;
    popupDirection["DOWN"] = PopupDirection::DOWN;
    fairygui.new_usertype<FairyGUIScene>("FairyGUIScene",
        sol::call_constructor, sol::factories([]() { return std::unique_ptr<FairyGUIScene>(FairyGUIScene::create()); }),
        "groot", &FairyGUIScene::_groot,
        "Schedule", [](FairyGUIScene* obj, sol::function func, const char* key) { obj->schedule([func](float dt) { CALL_LUA(func, dt) }, key); },
        "ScheduleOnce", [](FairyGUIScene* obj, sol::function func, float delay, const char* key) { obj->scheduleOnce([func](float dt) { CALL_LUA(func, dt) }, delay, key); },
        sol::base_classes, sol::bases<cocos2d::Scene>());
    fairygui["SetDesignResolutionSize"] = [context](int w, int h) { return context->GetSubsystem<GUI>()->SetDesignResolutionSize(w, h); };
    fairygui["ReplaceScene"] = [context](FairyGUIScene* scene) { context->GetSubsystem<GUI>()->SetFairyGUIRoot(scene->_groot); };
    fairygui["ScheduleScriptFunc"] = [](sol::function func, float interval, bool paused) { return cocos2d::Director::getInstance()->getScheduler()->scheduleScriptFunc(func, interval, paused); };
    fairygui["UnscheduleScriptEntry"] = [](unsigned int scheduleScriptEntryID) { cocos2d::Director::getInstance()->getScheduler()->unscheduleScriptEntry(scheduleScriptEntryID); };
    fairygui["StartDrag"] = sol::overload(
        [](const ea::string& icon, const ea::string& sourceData, int touchPointID) { DragDropManager::getInstance()->startDrag(icon.c_str(), cocos2d::Value(sourceData.c_str()), touchPointID); },
        [](const ea::string& icon, const int sourceData, int touchPointID) { DragDropManager::getInstance()->startDrag(icon.c_str(), cocos2d::Value(sourceData), touchPointID); },
        [](const ea::string& icon, const float sourceData, int touchPointID) { DragDropManager::getInstance()->startDrag(icon.c_str(), cocos2d::Value(sourceData), touchPointID); },
        [](const ea::string& icon, const bool sourceData, int touchPointID) { DragDropManager::getInstance()->startDrag(icon.c_str(), cocos2d::Value(sourceData), touchPointID); });
    fairygui["EmojiParser"] = [](const char* str) {
        return EmojiParser::getInstance()->parse(str);
    };
//     auto uiconfig = fairygui["UIConfig"].get_or_create<sol::table>();
//     uiconfig["defaultFont"]         = std::ref(UIConfig::defaultFont);
//     uiconfig["buttonSound"]         = std::ref(UIConfig::buttonSound);
//     uiconfig["tooltipsWin"]         = std::ref(UIConfig::tooltipsWin);
//     uiconfig["popupMenu"]           = std::ref(UIConfig::popupMenu);
//     uiconfig["verticalScrollBar"]   = std::ref(UIConfig::verticalScrollBar);
//     uiconfig["horizontalScrollBar"] = std::ref(UIConfig::horizontalScrollBar);
//     uiconfig["globalModalWaiting"]  = std::ref(UIConfig::globalModalWaiting);
//     uiconfig["windowModalWaiting"]  = std::ref(UIConfig::windowModalWaiting);
    // TODO:
    fairygui["SetUIConfig"] = [](sol::table tb) {
        static std::string default_str{""};
        UIConfig::defaultFont = tb.get_or("defaultFont", std::string{ "default" });
        auto value = tb.get_or("buttonSound", default_str);
        if (value != default_str) {
            UIConfig::buttonSound = value;
        }
        value = tb.get_or("tooltipsWin", default_str);
        if (value != default_str) {
            UIConfig::tooltipsWin = value;
        }
        value = tb.get_or("popupMenu", default_str);
        if (value != default_str) {
            UIConfig::popupMenu = value;
        }
        value = tb.get_or("verticalScrollBar", default_str);
        if (value != default_str) {
            UIConfig::verticalScrollBar = value;
        }
        value = tb.get_or("horizontalScrollBar", default_str);
        if (value != default_str) {
            UIConfig::horizontalScrollBar = value;
        }
        value = tb.get_or("globalModalWaiting", default_str);
        if (value != default_str) {
            UIConfig::globalModalWaiting = value;
        }
        value = tb.get_or("windowModalWaiting", default_str);
        if (value != default_str) {
            UIConfig::windowModalWaiting = value;
        }
    };
	return 0;
}
#endif
