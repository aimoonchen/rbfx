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

    auto bindEventContext = fairygui.new_usertype<EventContext>("EventContext");
    bindEventContext["GetData"] = [](EventContext* obj) { return (GObject*)obj->getData(); };
    bindEventContext["GetDataAsString"] = [](EventContext* obj) { return obj->getDataValue().asString(); };
    bindEventContext["GetSender"] = [](EventContext* obj) { return (GObject*)obj->getSender(); };
    bindEventContext["GetTouchId"] = [](EventContext* self) { return self->getInput()->getTouchId(); };
    bindEventContext["PreventDefault"] = &EventContext::preventDefault;

    auto bindGController = fairygui.new_usertype<GController>("GController");
    bindGController["GetParent"] = &GController::getParent;
    bindGController["SetParent"] = &GController::setParent;
    bindGController["GetSelectedIndex"] = &GController::getSelectedIndex;
    bindGController["SetSelectedIndex"] = sol::overload(
        [](GController* self, int value) { self->setSelectedIndex(value); },
        [](GController* self, int value, bool triggerEvent) { self->setSelectedIndex(value, triggerEvent); });

    auto bindPopupMenu = fairygui.new_usertype<PopupMenu>("PopupMenu");
    bindPopupMenu["Create"] = sol::overload(
        []() { return PopupMenu::create(); },
        [](const ea::string& resourceURL) { return PopupMenu::create(resourceURL.c_str()); });
    bindPopupMenu["Show"] = sol::overload(
        [](PopupMenu* self) { self->show(); },
        [](PopupMenu* self, GObject* target, PopupDirection dir) { self->show(target, dir); });
    bindPopupMenu["AddItem"] = [](PopupMenu* self, const ea::string& caption, sol::function func) { self->addItem(caption.c_str(), [func](EventContext* context) { CALL_LUA(func, context) }); };

	auto bindGObject = fairygui.new_usertype<GObject>("GObject");
    bindGObject["name"]                 = sol::property(&GObject::name);
    bindGObject["GetParent"]            = &GObject::getParent;
    bindGObject["SetPosition"]          = &GObject::setPosition;
    bindGObject["GetPosition"]          = [](GObject* obj) {
        const auto& cv2 = obj->getPosition();
        //return IntVector2((int32_t)cv2.x, (int32_t)cv2.y);
        return Vector2(cv2.x, cv2.y); };
    bindGObject["SetTouchable"]         = &GObject::setTouchable;
    bindGObject["SetPivot"]             = [](GObject* obj, float xv, float yv) { obj->setPivot(xv, yv); };
    bindGObject["GetX"]                 = &GObject::getX;
    bindGObject["GetY"]                 = &GObject::getY;
    bindGObject["GetWidth"]             = &GObject::getWidth;
    bindGObject["GetHeight"]            = &GObject::getHeight;
    bindGObject["SetWidth"]             = &GObject::setWidth;
    bindGObject["SetHeight"]            = &GObject::setHeight;
    bindGObject["SetRotation"]          = &GObject::setRotation;
    bindGObject["GetRotation"]          = &GObject::getRotation;
    bindGObject["SetScale"]             = &GObject::setScale;
    bindGObject["SetSize"]              = sol::overload(
        [](GObject* self, const Vector2& size) { self->setSize(size.x_, size.y_); },
        [](GObject* self, float w, float h) { self->setSize(w, h); },
        [](GObject* self, float w, float h, bool ignorePivot) { self->setSize(w, h, ignorePivot); });
    bindGObject["AddRelation"]          = sol::overload(
        [](GObject* self, GObject* target, RelationType relationType) { self->addRelation(target, relationType); },
        [](GObject* self, GObject* target, RelationType relationType, bool usePercent) { self->addRelation(target, relationType, usePercent); });
    bindGObject["SetSortingOrder"]      = &GObject::setSortingOrder;
    bindGObject["SetDragBounds"]        = [](GObject* obj, const Rect& rect) { obj->setDragBounds(FromUrhoRect(rect)); };
    bindGObject["AddEventListener"]     = sol::overload(
        [](GObject* obj, UIEventType eventType, sol::function func) { obj->addEventListener(eventType, [func](EventContext* context) { CALL_LUA(func, context) }); },
        [](GObject* obj, UIEventType eventType, sol::function func, int eventTag) { obj->addEventListener(eventType, [func](EventContext* context) { CALL_LUA(func, context) }, EventTag(eventTag)); },
        [](GObject* obj, UIEventType eventType, sol::function func, void* eventTag) { obj->addEventListener(eventType, [func](EventContext* context) { CALL_LUA(func, context) }, EventTag(eventTag)); });
    bindGObject["RemoveEventListener"]  = sol::overload(
        [](GObject* obj, UIEventType eventType) { obj->removeEventListener(eventType); },
        [](GObject* obj, UIEventType eventType, int eventTag) { obj->removeEventListener(eventType, EventTag(eventTag)); },
        [](GObject* obj, UIEventType eventType, void* eventTag) { obj->removeEventListener(eventType, EventTag(eventTag)); });
    bindGObject["RemoveEventListeners"] = &GObject::removeEventListeners;
    bindGObject["AddClickListener"]     = sol::overload(
        [](GObject* obj, sol::function func) { obj->addClickListener([func](EventContext* context) { CALL_LUA(func, context) }); },
        [](GObject* obj, sol::function func, int eventTag) { obj->addClickListener([func](EventContext* context) { CALL_LUA(func, context) }, EventTag(eventTag)); },
        [](GObject* obj, sol::function func, void* eventTag) { obj->addClickListener([func](EventContext* context) { CALL_LUA(func, context) }, EventTag(eventTag)); });
    bindGObject["RemoveClickListener"]  = sol::overload(
        [](GObject* obj, int eventTag) { obj->removeClickListener(EventTag(eventTag)); },
        [](GObject* obj, void* eventTag) { obj->removeClickListener(EventTag(eventTag)); });
    bindGObject["Center"] = sol::overload(
        [](GObject* self) { self->center(); },
        [](GObject* self, bool restraint) { self->center(restraint); });
    bindGObject["SetText"]              = &GObject::setText;
    bindGObject["SetIcon"]              = &GObject::setIcon;
    bindGObject["GetText"]              = &GObject::getText;
    bindGObject["GetIcon"]              = &GObject::getIcon;
    bindGObject["SetVisible"]           = &GObject::setVisible;
    bindGObject["IsVisible"]            = &GObject::isVisible;
    bindGObject["GetGroup"]             = &GObject::getGroup;
    bindGObject["SetDraggable"]         = &GObject::setDraggable;
    bindGObject["GetSize"]              = [](GObject* self) {
        auto size = self->getSize();
        return Vector2{ size.width, size.height }; };
    bindGObject["TransformRect"]        = [](GObject* self, const Rect& rect, GObject* targetSpace) {
        auto crect = self->transformRect(FromUrhoRect(rect), targetSpace);
        return ToUrhoRect(crect); };
    bindGObject["MakeFullScreen"]       = &GObject::makeFullScreen;
    bindGObject["RemoveFromParent"]     = &GObject::removeFromParent;
    bindGObject["GetSourceSize"]        = [](GObject* obj) { return IntVector2{ (int)obj->sourceSize.width, (int)obj->sourceSize.height }; };
    bindGObject["GetInitSize"]          = [](GObject* obj) { return IntVector2{ (int)obj->initSize.width, (int)obj->initSize.height }; };
    bindGObject["TreeNode"]             = &GObject::treeNode;
	    
    auto bindGLoader = fairygui.new_usertype<GLoader>("GLoader", sol::base_classes, sol::bases<GObject>());
    bindGLoader["GetIcon"] = &GLoader::getIcon;
    bindGLoader["SetIcon"] = &GLoader::setIcon;
        
    auto bindGTextField = fairygui.new_usertype<GTextField>("GTextField", sol::base_classes, sol::bases<GObject>());
    bindGTextField["GetText"] = &GTextField::getText;
    bindGTextField["SetText"] = &GTextField::setText;
    bindGTextField["SetColor"] = [](GTextField* obj, uint8_t r, uint8_t g, uint8_t b) { obj->setColor({ r, g, b }); };
    bindGTextField["SetFontSize"] = &GTextField::setFontSize;
    bindGTextField["GetFontSize"] = &GTextField::getFontSize;
    bindGTextField["SetOutlineColor"] = [](GTextField* obj, uint8_t r, uint8_t g, uint8_t b) { obj->setOutlineColor({ r, g, b }); };
    bindGTextField["GetTextSize"] = [](GTextField* obj) { auto size = obj->getTextSize(); return IntVector2{ (int)size.width, (int)size.height }; };
        

    auto bindGGraph = fairygui.new_usertype<GGraph>("GGraph",
        sol::call_constructor, sol::factories([]() { return GGraph::create(); }),
        sol::base_classes, sol::bases<GObject>());
    bindGGraph["Create"] = &GGraph::create;
    bindGGraph["DrawRect"] = [](GGraph* obj, float aWidth, float aHeight, int lineSize, const Urho3D::Color& lineColor, const Urho3D::Color& fillColor) {
        obj->drawRect(aWidth, aHeight, lineSize, Color4FFromUrhoColor(lineColor), Color4FFromUrhoColor(fillColor)); };
        
    fairygui.new_usertype<TextFormat>("TextFormat", "face", sol::property(&TextFormat::face));

    auto bindGBasicTextField = fairygui.new_usertype<GBasicTextField>("GBasicTextField", sol::base_classes, sol::bases<GTextField, GObject>());
    bindGBasicTextField["SetAutoSize"]      = &GBasicTextField::setAutoSize;
    bindGBasicTextField["SetSingleLine"]    = &GBasicTextField::setSingleLine;
    bindGBasicTextField["GetTextFormat"]    = &GBasicTextField::getTextFormat;
    bindGBasicTextField["ApplyTextFormat"]  = &GBasicTextField::applyTextFormat;
    bindGBasicTextField["SetText"]          = &GBasicTextField::setText;
    
    auto bindGRichTextField = fairygui.new_usertype<GRichTextField>("GRichTextField", sol::base_classes, sol::bases<GTextField, GObject>());
    bindGRichTextField["SetAutoSize"]       = &GRichTextField::setAutoSize;
    bindGRichTextField["SetSingleLine"]     = &GRichTextField::setSingleLine;
    bindGRichTextField["GetTextFormat"]     = &GRichTextField::getTextFormat;
    bindGRichTextField["ApplyTextFormat"]   = &GRichTextField::applyTextFormat;
        
    auto bindTransition = fairygui.new_usertype<Transition>("Transition");
    bindTransition["Play"] = sol::overload([](Transition* obj) { obj->play(); },
        [](Transition* obj, sol::function func) { obj->play([func]() { CALL_LUA(func) }); },
        [](Transition* obj, int times, float delay) { obj->play(times, delay); },
        [](Transition* obj, int times, float delay, sol::function func) { obj->play(times, delay, [func]() { CALL_LUA(func) }); },
        [](Transition* obj, int times, float delay, float startTime, float endTime) { obj->play(times, delay, startTime, endTime); });
    bindTransition["SetHook"] = [](Transition* obj, const char* label, sol::function func) { obj->setHook(label, [func]() { CALL_LUA(func) }); };
        
	auto bindGComponent = fairygui.new_usertype<GComponent>("GComponent", sol::base_classes, sol::bases<GObject>());
    bindGComponent["AddChild"]              = &GComponent::addChild;
    bindGComponent["RemoveChild"]           = &GComponent::removeChild;
    bindGComponent["RemoveChildren"]        = sol::overload(
        [](GComponent* self) { self->removeChildren(); },
        [](GComponent* self, int beginIndex, int endIndex) { self->removeChildren(beginIndex, endIndex); });
    bindGComponent["GetChild"]              = &GComponent::getChild;
    bindGComponent["GetChildAt"]            = &GComponent::getChildAt;
    bindGComponent["GetChildById"]          = [](GComponent* self, const ea::string& id) { return self->getChildById(id.c_str()); };
    bindGComponent["GetController"]         = &GComponent::getController;
    bindGComponent["GetViewWidth"]          = &GComponent::getViewWidth;
    bindGComponent["NumChildren"]           = &GComponent::numChildren;
    bindGComponent["GetTransition"]         = &GComponent::getTransition;
    bindGComponent["IsChildInView"]         = &GComponent::isChildInView;
    bindGComponent["GetFirstChildInView"]   = &GComponent::getFirstChildInView;
    bindGComponent["EnsureBoundsCorrect"]   = &GComponent::ensureBoundsCorrect;
    bindGComponent["IsAncestorOf"]          = &GComponent::isAncestorOf;
    bindGComponent["GetScrollPane"]         = &GComponent::getScrollPane;
		
    fairygui.new_usertype<GGroup>("GGroup",
        "name", sol::property(&GObject::name),
		sol::base_classes, sol::bases<GObject>());
    
    fairygui.new_usertype<GImage>("GImage",
        sol::base_classes, sol::bases<GObject>());

    auto bindGRoot = fairygui.new_usertype<GRoot>("GRoot", sol::call_constructor,
        sol::factories([]() { return std::make_unique<GRoot>(); }),
        sol::base_classes, sol::bases<GComponent, GObject>());
    bindGRoot["ShowPopup"]      = sol::overload(
        [](GRoot* self, GObject* popup) { self->showPopup(popup); },
        [](GRoot* self, GObject* popup, GObject* target, PopupDirection dir) { self->showPopup(popup, target, dir); });
    bindGRoot["GetTouchTarget"] = &GRoot::getTouchTarget;
    bindGRoot["ShowModalWait"]  = &GRoot::showModalWait;
    bindGRoot["CloseModalWait"] = &GRoot::closeModalWait;
        
    auto bindGButton = fairygui.new_usertype<GButton>("GButton", sol::base_classes, sol::bases<GComponent, GObject>());
    bindGButton["IsChangeStateOnClick"]     = &GButton::isChangeStateOnClick;
    bindGButton["SetChangeStateOnClick"]    = &GButton::setChangeStateOnClick;
    bindGButton["SetTitle"]                 = &GButton::setTitle;
    bindGButton["SetText"]                  = &GButton::setText;
    bindGButton["SetIcon"]                  = &GButton::setIcon;
    bindGButton["GetText"]                  = &GButton::getText;
    bindGButton["GetIcon"]                  = &GButton::getIcon;
        
    auto bindGProgressBar = fairygui.new_usertype<GProgressBar>("GProgressBar", sol::base_classes, sol::bases<GComponent, GObject>());
    bindGProgressBar["SetValue"]    = &GProgressBar::setValue;
    bindGProgressBar["GetValue"]    = &GProgressBar::getValue;
    bindGProgressBar["SetMin"]      = &GProgressBar::setMin;
    bindGProgressBar["GetMin"]      = &GProgressBar::getMin;
    bindGProgressBar["SetMax"]      = &GProgressBar::setMax;
    bindGProgressBar["GetMax"]      = &GProgressBar::getMax;
        
    auto bindScrollPane = fairygui.new_usertype<ScrollPane>("ScrollPane");
    bindScrollPane["GetPosX"]       = &ScrollPane::getPosX;
    bindScrollPane["GetPosY"]       = &ScrollPane::getPosY;
    bindScrollPane["SetPosX"]       = sol::overload([](ScrollPane* obj, float value) { obj->setPosX(value); }, [](ScrollPane* obj, float value, bool ani) { obj->setPosX(value, ani); });
    bindScrollPane["SetPosY"]       = sol::overload([](ScrollPane* obj, float value) { obj->setPosY(value); }, [](ScrollPane* obj, float value, bool ani) { obj->setPosY(value, ani); });
    bindScrollPane["ScrollTop"]     = [](ScrollPane* obj) { obj->scrollTop(); };
    bindScrollPane["ScrollBottom"]  = [](ScrollPane* obj) { obj->scrollBottom(); };
    bindScrollPane["CancelDragging"] = &ScrollPane::cancelDragging;
    bindScrollPane["LockHeader"]    = &ScrollPane::lockHeader;
    bindScrollPane["LockFooter"]    = &ScrollPane::lockFooter;
    bindScrollPane["GetHeader"]     = &ScrollPane::getHeader;
    bindScrollPane["GetFooter"]     = &ScrollPane::getFooter;
    bindScrollPane["IsBottomMost"]  = &ScrollPane::isBottomMost;
    bindScrollPane["IsRightMost"]   = &ScrollPane::isRightMost;
        
    auto bindGList = fairygui.new_usertype<GList>("GList", sol::base_classes, sol::bases<GComponent, GObject>());
    bindGList["SetVirtual"]         = sol::resolve<void()>(&GList::setVirtual);
    bindGList["SetVirtualAndLoop"]  = &GList::setVirtualAndLoop;
    bindGList["SetNumItems"]        = &GList::setNumItems;
    bindGList["GetNumItems"]        = &GList::getNumItems;
    bindGList["AddSelection"]       = &GList::addSelection;
    bindGList["GetSelectedIndex"]   = &GList::getSelectedIndex;
    bindGList["SetSelectedIndex"]   = &GList::setSelectedIndex;
    bindGList["SetItemRenderer"]    = [](GList* obj, sol::function func) { obj->itemRenderer = [func](int index, GObject* item) { CALL_LUA(func, index, item) }; };
    bindGList["SetItemProvider"]    = [](GList* obj, sol::function func) { obj->itemProvider = [func](int index) { CALL_LUA(func, index) return result.get<std::string>(); }; };
    bindGList["AddItemFromPool"]    = sol::overload(sol::resolve<GObject * ()>(&GList::addItemFromPool), [](GList* obj, const char* url) { return obj->addItemFromPool(url); });
        
    auto bindWindow = fairygui.new_usertype<Window>("Window", sol::base_classes, sol::bases<GComponent, GObject>());
    bindWindow["Create"]            = &Window::create;
    bindWindow["Show"]              = &Window::show;
    bindWindow["Hide"]              = &Window::hide;
    bindWindow["HideImmediately"]   = &Window::hideImmediately;
    bindWindow["SetModal"]          = &Window::setModal;
    bindWindow["GetContentPane"]    = &Window::getContentPane;
    bindWindow["SetContentPane"]    = &Window::setContentPane;
    bindWindow["ShowModalWait"]     = sol::resolve<void()>(&Window::showModalWait);
    bindWindow["CloseModalWait"]    = sol::resolve<bool()>(&Window::closeModalWait);
        
    auto bindGTweener = fairygui.new_usertype<GTweener>("GTweener");
    bindGTweener["SetTarget"]   = sol::overload([](GTweener* obj, GObject* target) { return obj->setTarget(target); },
        [](GTweener* obj, GObject* target, TweenPropType propType) { return obj->setTarget(target, propType); });
    bindGTweener["SetRepeat"]   = [](GTweener* obj, int repeat) {obj->setRepeat(repeat); };
    bindGTweener["OnStart"]     = [](GTweener* obj, sol::function func) { obj->onStart([func](GTweener* tweener) { CALL_LUA(func, tweener) }); };
    bindGTweener["OnUpdate"]    = [](GTweener* obj, sol::function func) { obj->onUpdate([func](GTweener* tweener) { CALL_LUA(func, tweener) }); };
    bindGTweener["OnComplete"]  = [](GTweener* obj, sol::function func) { obj->onComplete([func]() { CALL_LUA(func) }); };
    bindGTweener["OnComplete1"] = [](GTweener* obj, sol::function func) { obj->onComplete1([func](GTweener* tweener) { CALL_LUA(func, tweener) }); };
    bindGTweener["value"]       = [](GTweener* obj) { return Vector4{ obj->value.x, obj->value.y, obj->value.z, obj->value.w }; };
    
    auto bindGTween = fairygui.new_usertype<GTween>("GTween");
    bindGTween["To"]            = sol::overload([](float startValue, float endValue, float duration) { return GTween::to(startValue, endValue, duration); },
        [](const Vector2& startValue, const Vector2& endValue, float duration) { return GTween::to(*(cocos2d::Vec2*)startValue.Data(), *(cocos2d::Vec2*)endValue.Data(), duration); },
        [](const Vector3& startValue, const Vector3& endValue, float duration) { return GTween::to(*(cocos2d::Vec3*)startValue.Data(), *(cocos2d::Vec3*)endValue.Data(), duration); },
        [](const Vector4& startValue, const Vector4& endValue, float duration) { return GTween::to(*(cocos2d::Vec4*)startValue.Data(), *(cocos2d::Vec4*)endValue.Data(), duration); },
        [](const Color& startValue, const Color& endValue, float duration) { return GTween::to(Color4BFromUrhoColor(startValue), Color4BFromUrhoColor(endValue), duration); });
    bindGTween["delayedCall"]   = &GTween::delayedCall;
    bindGTween["Shake"]         = [](const Vector2& startValue, float amplitude, float duration) { return GTween::shake(*(cocos2d::Vec2*)startValue.Data(), amplitude, duration); };

    auto bindGTreeNode = fairygui.new_usertype<GTreeNode>("GTreeNode");
    bindGTreeNode["Create"] = sol::overload([]() { return GTreeNode::create(); }, [](bool isFolder) { return GTreeNode::create(isFolder); });
    bindGTreeNode["AddChild"] = &GTreeNode::addChild;
    bindGTreeNode["SetData"] = &GTreeNode::setData;
    bindGTreeNode["GetData"] = &GTreeNode::getData;
    bindGTreeNode["SetTable"] = &GTreeNode::setTable;
    bindGTreeNode["GetTable"] = &GTreeNode::getTable;
    bindGTreeNode["GetCell"] = &GTreeNode::getCell;
    bindGTreeNode["IsFolder"] = &GTreeNode::isFolder;
    bindGTreeNode["GetText"] = &GTreeNode::getText;
        
    auto bindGTree = fairygui.new_usertype<GTree>("GTree", sol::base_classes, sol::bases<GList, GComponent, GObject>());
    bindGTree["GetRootNode"]        = &GTree::getRootNode;
    bindGTree["GetSelectedNode"]    = &GTree::getSelectedNode;
    bindGTree["SetItemRenderer"]    = [](GTree* obj, sol::function func) { obj->treeNodeRender = [func](GTreeNode* node, GComponent* obj) { CALL_LUA(func, node, obj) }; };
        
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

    auto bindFairyGUIScene = fairygui.new_usertype<FairyGUIScene>("FairyGUIScene",
        sol::call_constructor, sol::factories([]() { return std::unique_ptr<FairyGUIScene>(FairyGUIScene::create()); }),
        sol::base_classes, sol::bases<cocos2d::Scene>());
    bindFairyGUIScene["groot"] = sol::property(&FairyGUIScene::_groot);
    bindFairyGUIScene["Schedule"] = [](FairyGUIScene* obj, sol::function func, const char* key) { obj->schedule([func](float dt) { CALL_LUA(func, dt) }, key); };
    bindFairyGUIScene["ScheduleOnce"] = [](FairyGUIScene* obj, sol::function func, float delay, const char* key) { obj->scheduleOnce([func](float dt) { CALL_LUA(func, dt) }, delay, key); };
        
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
