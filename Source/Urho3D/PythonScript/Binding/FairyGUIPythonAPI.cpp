#if defined(__linux__) && !defined(__ANDROID__)
#else
#include <nanobind/nanobind.h>
#include "Urho3D/Core/Context.h"
#include "Urho3D/IO/Log.h"
#include "Urho3D/FairyGUI/UIConfig.h"
#include "Urho3D/FairyGUI/UIPackage.h"
#include "Urho3D/FairyGUI/GComponent.h"
#include "Urho3D/FairyGUI/GButton.h"
#include "Urho3D/FairyGUI/GComboBox.h"
#include "Urho3D/FairyGUI/GGraph.h"
#include "Urho3D/FairyGUI/GGroup.h"
#include "Urho3D/FairyGUI/GImage.h"
#include "Urho3D/FairyGUI/GLabel.h"
#include "Urho3D/FairyGUI/ScrollPane.h"
#include "Urho3D/FairyGUI/GList.h"
#include "Urho3D/FairyGUI/GLoader.h"
#include "Urho3D/FairyGUI/GLoader3D.h"
#include "Urho3D/FairyGUI/GMovieClip.h"
#include "Urho3D/FairyGUI/GProgressBar.h"
#include "Urho3D/FairyGUI/GRichTextField.h"
#include "Urho3D/FairyGUI/GRoot.h"
#include "Urho3D/FairyGUI/GScrollBar.h"
#include "Urho3D/FairyGUI/GSlider.h"
#include "Urho3D/FairyGUI/GTextField.h"
#include "Urho3D/FairyGUI/GTree.h"
#include "Urho3D/FairyGUI/PopupMenu.h"
#include "Urho3D/FairyGUI/Controller.h"
#include "Urho3D/FairyGUI/event/EventContext.h"
#include "Urho3D/FairyGUI/utils/UBBParser.h"
#include "Urho3D/FairyGUI/display/TextFormat.h"
#include "Urho3D/FairyGUI/DragDropManager.h"
#include "Urho3D/FairyGUI/tween/GTween.h"
#include "Urho3D/FairyGUI/tween/GTweener.h"
#include "Urho3D/Cocos2D/base/CCDirector.h"
#include "Urho3D/Cocos2D/base/CCScheduler.h"
#include "Urho3D/Cocos2D/base/CCValue.h"
#include "Urho3D/Cocos2D/math/CCGeometry.h"
//
#include "Urho3D/GUI/Gui.h"

#include "../PythonScript.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;

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

// namespace{
// ea::unordered_map<TypeID, ea::function<int(lua_State* L, const GObject* obj)>> fairygui_convert_map = {
//     {TypeID::kComponent,        [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GComponent*>(obj)).push(L); }},
//     {TypeID::kGraph,            [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GGraph*>(obj)).push(L); }},
//     {TypeID::kGroup,            [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GGroup*>(obj)).push(L); }},
//     {TypeID::kImage,            [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GImage*>(obj)).push(L); }},
//     {TypeID::kLoader,           [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GLoader*>(obj)).push(L); }},
//     {TypeID::kLoader3D,         [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GLoader3D*>(obj)).push(L); }},
//     {TypeID::kMovieClip,        [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GMovieClip*>(obj)).push(L); }},
//     {TypeID::kTextField,        [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GTextField*>(obj)).push(L); }},
//     {TypeID::kBasicTextField,   [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GBasicTextField*>(obj)).push(L); }},
//     {TypeID::kButton,           [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GButton*>(obj)).push(L); }},
//     {TypeID::kComboBox,         [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GComboBox*>(obj)).push(L); }},
//     {TypeID::kLabel,            [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GLabel*>(obj)).push(L); }},
//     {TypeID::kList,             [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GList*>(obj)).push(L); }},
//     {TypeID::kProgressBar,      [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GProgressBar*>(obj)).push(L); }},
//     {TypeID::kRoot,             [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GRoot*>(obj)).push(L); }},
//     {TypeID::kScrollBar,        [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GScrollBar*>(obj)).push(L); }},
//     {TypeID::kSlider,           [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GSlider*>(obj)).push(L); }},
//     {TypeID::kWindow,           [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const Window*>(obj)).push(L); }},
//     {TypeID::kTree,             [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GTree*>(obj)).push(L); }},
//     {TypeID::kRichTextField,    [](lua_State* L, const GObject* obj) { return sol::make_object(L, static_cast<const GRichTextField*>(obj)).push(L); }}
// };
// }
// namespace sol {
// 	int sol_lua_push(sol::types<GObject*>, lua_State* L, const GObject* obj) {
// 		if (obj) {
//             if (auto it = fairygui_convert_map.find(obj->typeID); it != fairygui_convert_map.end()) {
//                 auto& [typeKey, converter] = *it;
//                 return converter(L, obj);
//             } else {
//                 URHO3D_LOGERRORF("sol_lua_push error, can't find convert function for fairygui component %d.", obj->typeID);
//             }
// 		}
// 		return sol::make_object(L, obj).push(L);
// 	}
// }
// 
// Urho3D::Context* GetContext(lua_State* L);
// 
// static void BindUIConfig(sol::state& lua)
// {
//     auto fairygui = lua["FairyGUI"].get<sol::table>();
//     auto uiconfig = fairygui["UIConfig"].get_or_create<sol::table>();
// 	// uiconfig["horizontalScrollBar"] = std::ref(UIConfig::horizontalScrollBar);
// 	// uiconfig["verticalScrollBar"] = std::ref(UIConfig::verticalScrollBar);
// 	//...
// }

static void RegisterUIEventTypeConst(nb::module_& m)
{
    nanobind::enum_<UIEventType>(m, "UIEventType")
        .value("Enter", UIEventType::Enter)
        .value("Exit", UIEventType::Exit)
        .value("Changed", UIEventType::Changed)
        .value("Submit", UIEventType::Submit)
        .value("TouchBegin", UIEventType::TouchBegin)
        .value("TouchMove", UIEventType::TouchMove)
        .value("TouchEnd", UIEventType::TouchEnd)
        .value("Click", UIEventType::Click)
        .value("RollOver", UIEventType::RollOver)
        .value("RollOut", UIEventType::RollOut)
        .value("MouseWheel", UIEventType::MouseWheel)
        .value("RightClick", UIEventType::RightClick)
        .value("MiddleClick", UIEventType::MiddleClick)
        .value("PositionChange", UIEventType::PositionChange)
        .value("SizeChange", UIEventType::SizeChange)
        .value("KeyDown", UIEventType::KeyDown)
        .value("KeyUp", UIEventType::KeyUp)
        .value("Scroll", UIEventType::Scroll)
        .value("ScrollEnd", UIEventType::ScrollEnd)
        .value("PullDownRelease", UIEventType::PullDownRelease)
        .value("PullUpRelease", UIEventType::PullUpRelease)
        .value("ClickItem", UIEventType::ClickItem)
        .value("ClickLink", UIEventType::ClickLink)
        .value("ClickMenu", UIEventType::ClickMenu)
        .value("RightClickItem", UIEventType::RightClickItem)
        .value("DragStart", UIEventType::DragStart)
        .value("DragMove", UIEventType::DragMove)
        .value("DragEnd", UIEventType::DragEnd)
        .value("Drop", UIEventType::Drop)
        .value("GearStop", UIEventType::GearStop);
}

static void RegisterRelationTypeConst(nb::module_& m)
{
    nanobind::enum_<RelationType>(m, "RelationType")
        .value("Left_Left", RelationType::Left_Left)
	    .value("Left_Center", RelationType::Left_Center)
	    .value("Left_Right", RelationType::Left_Right)
	    .value("Center_Center", RelationType::Center_Center)
	    .value("Right_Left", RelationType::Right_Left)
	    .value("Right_Center", RelationType::Right_Center)
	    .value("Right_Right", RelationType::Right_Right)
        .value("Top_Top", RelationType::Top_Top)
        .value("Top_Middle", RelationType::Top_Middle)
        .value("Top_Bottom", RelationType::Top_Bottom)
        .value("Middle_Middle", RelationType::Middle_Middle)
        .value("Bottom_Top", RelationType::Bottom_Top)
        .value("Bottom_Middle", RelationType::Bottom_Middle)
        .value("Bottom_Bottom", RelationType::Bottom_Bottom)
	    .value("Width", RelationType::Width)
        .value("Height", RelationType::Height)
	    .value("LeftExt_Left", RelationType::LeftExt_Left)
        .value("LeftExt_Right", RelationType::LeftExt_Right)
        .value("RightExt_Left", RelationType::RightExt_Left)
        .value("RightExt_Right", RelationType::RightExt_Right)
        .value("TopExt_Top", RelationType::TopExt_Top)
        .value("TopExt_Bottom", RelationType::TopExt_Bottom)
        .value("BottomExt_Top", RelationType::BottomExt_Top)
        .value("BottomExt_Bottom", RelationType::BottomExt_Bottom)
        .value("Size", RelationType::Size);
    nanobind::enum_<TweenPropType>(m, "TweenPropType")
        .value("X", TweenPropType::X)
        .value("Y", TweenPropType::Y)
        .value("Position", TweenPropType::Position)
        .value("Width", TweenPropType::Width)
        .value("Height", TweenPropType::Height)
        .value("Size", TweenPropType::Size)
        .value("ScaleX", TweenPropType::ScaleX)
        .value("ScaleY", TweenPropType::ScaleY)
        .value("Scale", TweenPropType::Scale)
        .value("Rotation", TweenPropType::Rotation)
        .value("Alpha", TweenPropType::Alpha)
        .value("Progress", TweenPropType::Progress);
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
NB_MODULE(fairygui, m)
{
    m.def_submodule("UIPackage")
	    .def("GetById", &UIPackage::getById)
	    .def("GetByName", &UIPackage::getByName)
	    .def("AddPackage", &UIPackage::addPackage)
	    .def("RemovePackage", &UIPackage::removePackage)
	    .def("RemoveAllPackages", &UIPackage::removeAllPackages)
	    .def("CreateObject", [](const std::string& pkgName, const std::string& resName) { return UIPackage::createObject(pkgName, resName); })
	    .def("CreateObjectFromURL", &UIPackage::createObjectFromURL);

    nb::class_<EventContext>(m, "EventContext")
        .def("GetData", [](EventContext* obj) { return (GObject*)obj->getData(); })
        .def("GetDataAsString", [](EventContext* obj) { return obj->getDataValue().asString(); })
        .def("GetSender", [](EventContext* obj) { return (GObject*)obj->getSender(); })
        .def("GetTouchId", [](EventContext* self) { return self->getInput()->getTouchId(); })
        .def("PreventDefault", &EventContext::preventDefault);

    nb::class_<GController>(m, "GController")
        .def("GetParent", &GController::getParent)
        .def("SetParent", &GController::setParent)
        .def("GetSelectedIndex", &GController::getSelectedIndex)
        .def("SetSelectedIndex", [](GController* self, int value) { self->setSelectedIndex(value); })
        .def("SetSelectedIndex", [](GController* self, int value, bool triggerEvent) { self->setSelectedIndex(value, triggerEvent); });

    nb::class_<PopupMenu>(m, "PopupMenu")
        .def("Create", []() { return PopupMenu::create(); })
        .def("Create", [](const ea::string& resourceURL) { return PopupMenu::create(resourceURL.c_str()); })
        .def("Show", [](PopupMenu* self) { self->show(); })
        .def("Show", [](PopupMenu* self, GObject* target, PopupDirection dir) { self->show(target, dir); })
        .def("AddItem", [](PopupMenu* self, const ea::string& caption, nb::callable func) { self->addItem(caption.c_str(), [func](EventContext* context) { CALL_PYTHON(func, context) }); });

    nb::class_<GObject>(m, "GObject")
        .def_rw("name", &GObject::name)
        .def("GetParent", &GObject::getParent)
        .def("SetPosition", &GObject::setPosition)
        .def("GetPosition", [](GObject* obj) {
            const auto& cv2 = obj->getPosition();
            //return IntVector2((int32_t)cv2.x, (int32_t)cv2.y);
            return Vector2(cv2.x, cv2.y); })
        .def("SetTouchable", &GObject::setTouchable)
        .def("SetPivot", [](GObject* obj, float xv, float yv) { obj->setPivot(xv, yv); })
        .def("GetX", &GObject::getX)
        .def("GetY", &GObject::getY)
        .def("GetWidth", &GObject::getWidth)
        .def("GetHeight", &GObject::getHeight)
        .def("SetWidth", &GObject::setWidth)
        .def("SetHeight", &GObject::setHeight)
        .def("SetRotation", &GObject::setRotation)
        .def("GetRotation", &GObject::getRotation)
        .def("SetScale", &GObject::setScale)
        .def("SetSize", [](GObject* self, const Vector2& size) { self->setSize(size.x_, size.y_); })
        .def("SetSize", [](GObject* self, float w, float h) { self->setSize(w, h); })
        .def("SetSize", [](GObject* self, float w, float h, bool ignorePivot) { self->setSize(w, h, ignorePivot); })
        .def("AddRelation", [](GObject* self, GObject* target, RelationType relationType) { self->addRelation(target, relationType); })
        .def("AddRelation", [](GObject* self, GObject* target, RelationType relationType, bool usePercent) { self->addRelation(target, relationType, usePercent); })
        .def("SetSortingOrder", &GObject::setSortingOrder)
        .def("SetDragBounds", [](GObject* obj, const Rect& rect) { obj->setDragBounds(FromUrhoRect(rect)); })
        .def("AddEventListener", [](GObject* obj, UIEventType eventType, nb::callable func) { obj->addEventListener(eventType, [func](EventContext* context) { CALL_PYTHON(func, context) }); })
        .def("AddEventListener", [](GObject* obj, UIEventType eventType, nb::callable func, int eventTag) { obj->addEventListener(eventType, [func](EventContext* context) { CALL_PYTHON(func, context) }, EventTag(eventTag)); })
        .def("AddEventListener", [](GObject* obj, UIEventType eventType, nb::callable func, void* eventTag) { obj->addEventListener(eventType, [func](EventContext* context) { CALL_PYTHON(func, context) }, EventTag(eventTag)); })
        .def("RemoveEventListener", [](GObject* obj, UIEventType eventType) { obj->removeEventListener(eventType); })
        .def("RemoveEventListener", [](GObject* obj, UIEventType eventType, int eventTag) { obj->removeEventListener(eventType, EventTag(eventTag)); })
        .def("RemoveEventListener", [](GObject* obj, UIEventType eventType, void* eventTag) { obj->removeEventListener(eventType, EventTag(eventTag)); })
        .def("RemoveEventListeners", &GObject::removeEventListeners)
        .def("AddClickListener", [](GObject* obj, nb::callable func) { obj->addClickListener([func](EventContext* context) { CALL_PYTHON(func, context) }); })
        .def("AddClickListener", [](GObject* obj, nb::callable func, int eventTag) { obj->addClickListener([func](EventContext* context) { CALL_PYTHON(func, context) }, EventTag(eventTag)); })
        .def("AddClickListener", [](GObject* obj, nb::callable func, void* eventTag) { obj->addClickListener([func](EventContext* context) { CALL_PYTHON(func, context) }, EventTag(eventTag)); })
        .def("RemoveClickListener", [](GObject* obj, int eventTag) { obj->removeClickListener(EventTag(eventTag)); })
        .def("RemoveClickListener", [](GObject* obj, void* eventTag) { obj->removeClickListener(EventTag(eventTag)); })
        .def("Center", [](GObject* self) { self->center(); })
        .def("Center", [](GObject* self, bool restraint) { self->center(restraint); })
        .def("SetText", &GObject::setText)
        .def("SetIcon", &GObject::setIcon)
        .def("GetText", &GObject::getText)
        .def("GetIcon", &GObject::getIcon)
        .def("SetVisible", &GObject::setVisible)
        .def("IsVisible", &GObject::isVisible)
        .def("GetGroup", &GObject::getGroup)
        .def("SetDraggable", &GObject::setDraggable)
        .def("GetSize", [](GObject* self) {
            auto size = self->getSize();
            return Vector2{ size.width, size.height }; })
        .def("TransformRect", [](GObject* self, const Rect& rect, GObject* targetSpace) {
            auto crect = self->transformRect(FromUrhoRect(rect), targetSpace);
            return ToUrhoRect(crect); })
        .def("MakeFullScreen", &GObject::makeFullScreen)
        .def("RemoveFromParent", &GObject::removeFromParent)
        .def("GetSourceSize", [](GObject* obj) { return IntVector2{ (int)obj->sourceSize.width, (int)obj->sourceSize.height }; })
        .def("GetInitSize", [](GObject* obj) { return IntVector2{ (int)obj->initSize.width, (int)obj->initSize.height }; })
        .def("TreeNode", &GObject::treeNode);
	    
    nb::class_<GLoader, GObject>(m, "GLoader")
        .def("GetIcon", &GLoader::getIcon)
        .def("SetIcon", &GLoader::setIcon);
        
    nb::class_<GTextField, GObject>(m, "GTextField")
        .def("GetText", &GTextField::getText)
        .def("SetText", &GTextField::setText)
        .def("SetColor", [](GTextField* obj, uint8_t r, uint8_t g, uint8_t b) { obj->setColor({ r, g, b }); })
        .def("SetFontSize", &GTextField::setFontSize)
        .def("GetFontSize", &GTextField::getFontSize)
        .def("SetOutlineColor", [](GTextField* obj, uint8_t r, uint8_t g, uint8_t b) { obj->setOutlineColor({ r, g, b }); })
        .def("GetTextSize", [](GTextField* obj) { auto size = obj->getTextSize(); return IntVector2{ (int)size.width, (int)size.height }; });
        

    nb::class_<GGraph, GObject>(m, "GGraph")
//         sol::call_constructor, sol::factories([]() { return GGraph::create(); }),
//         sol::base_classes, sol::bases<GObject>());
        .def("Create", &GGraph::create)
        .def("DrawRect", [](GGraph* obj, float aWidth, float aHeight, int lineSize, const Urho3D::Color& lineColor, const Urho3D::Color& fillColor) {
            obj->drawRect(aWidth, aHeight, lineSize, Color4FFromUrhoColor(lineColor), Color4FFromUrhoColor(fillColor)); });
        
    nb::class_<TextFormat>(m, "TextFormat")
        .def_rw("face", &TextFormat::face);

    nb::class_<GBasicTextField, GTextField>(m, "GBasicTextField")
        .def("SetAutoSize", &GBasicTextField::setAutoSize)
        .def("SetSingleLine", &GBasicTextField::setSingleLine)
        .def("GetTextFormat", &GBasicTextField::getTextFormat)
        .def("ApplyTextFormat", &GBasicTextField::applyTextFormat)
        .def("SetText", &GBasicTextField::setText);
    
    nb::class_<GRichTextField, GTextField>(m, "GRichTextField")
        .def("SetAutoSize", &GRichTextField::setAutoSize)
        .def("SetSingleLine", &GRichTextField::setSingleLine)
        .def("GetTextFormat", &GRichTextField::getTextFormat)
        .def("ApplyTextFormat", &GRichTextField::applyTextFormat);
        
    nb::class_<Transition>(m, "Transition")
        .def("Play", [](Transition* obj) { obj->play(); })
        .def("Play", [](Transition* obj, nb::callable func) { obj->play([func]() { CALL_PYTHON(func) }); })
        .def("Play", [](Transition* obj, int times, float delay) { obj->play(times, delay); })
        .def("Play", [](Transition* obj, int times, float delay, nb::callable func) { obj->play(times, delay, [func]() { CALL_PYTHON(func) }); })
        .def("Play", [](Transition* obj, int times, float delay, float startTime, float endTime) { obj->play(times, delay, startTime, endTime); })
        .def("SetHook", [](Transition* obj, const char* label, nb::callable func) { obj->setHook(label, [func]() { CALL_PYTHON(func) }); });
        
    nb::class_<GComponent, GObject>(m, "GComponent")
        .def("AddChild", &GComponent::addChild)
        .def("RemoveChild", &GComponent::removeChild)
        .def("RemoveChildren", [](GComponent* self) { self->removeChildren(); })
        .def("RemoveChildren", [](GComponent* self, int beginIndex, int endIndex) { self->removeChildren(beginIndex, endIndex); })
        .def("GetChild", &GComponent::getChild)
        .def("GetChildAt", &GComponent::getChildAt)
        .def("GetChildById", [](GComponent* self, const ea::string& id) { return self->getChildById(id.c_str()); })
        .def("GetController", &GComponent::getController)
        .def("GetViewWidth", &GComponent::getViewWidth)
        .def("NumChildren", &GComponent::numChildren)
        .def("GetTransition", &GComponent::getTransition)
        .def("IsChildInView", &GComponent::isChildInView)
        .def("GetFirstChildInView", &GComponent::getFirstChildInView)
        .def("EnsureBoundsCorrect", &GComponent::ensureBoundsCorrect)
        .def("IsAncestorOf", &GComponent::isAncestorOf)
        .def("GetScrollPane", &GComponent::getScrollPane);
		
    nb::class_<GGroup, GObject>(m, "GGroup")
        .def_rw("name", &GObject::name);
    
    nb::class_<GImage, GObject>(m, "GImage");

    nb::class_<GRoot, GComponent>(m, "GRoot")
//         sol::factories([]() { return std::make_unique<GRoot>(); }),
//         sol::base_classes, sol::bases<GComponent, GObject>());
        .def("ShowPopup", [](GRoot* self, GObject* popup) { self->showPopup(popup); })
        .def("ShowPopup", [](GRoot* self, GObject* popup, GObject* target, PopupDirection dir) { self->showPopup(popup, target, dir); })
        .def("GetTouchTarget", &GRoot::getTouchTarget)
        .def("ShowModalWait", &GRoot::showModalWait)
        .def("CloseModalWait", &GRoot::closeModalWait);
        
    nb::class_<GButton, GComponent>(m, "GButton")
        .def("IsChangeStateOnClick", &GButton::isChangeStateOnClick)
        .def("SetChangeStateOnClick", &GButton::setChangeStateOnClick)
        .def("SetTitle", &GButton::setTitle)
        .def("SetText", &GButton::setText)
        .def("SetIcon", &GButton::setIcon)
        .def("GetText", &GButton::getText)
        .def("GetIcon", &GButton::getIcon);

        nb::class_<GProgressBar, GComponent>(m, "GProgressBar")
        .def("SetValue", &GProgressBar::setValue)
        .def("GetValue", &GProgressBar::getValue)
        .def("SetMin", &GProgressBar::setMin)
        .def("GetMin", &GProgressBar::getMin)
        .def("SetMax", &GProgressBar::setMax)
        .def("GetMax", &GProgressBar::getMax);
        
    nb::class_<ScrollPane>(m, "ScrollPane")
        .def("GetPosX", &ScrollPane::getPosX)
        .def("GetPosY", &ScrollPane::getPosY)
        .def("SetPosX", [](ScrollPane* obj, float value) { obj->setPosX(value); })
        .def("SetPosX", [](ScrollPane* obj, float value, bool ani) { obj->setPosX(value, ani); })
        .def("SetPosY", [](ScrollPane* obj, float value) { obj->setPosY(value); })
        .def("SetPosY", [](ScrollPane* obj, float value, bool ani) { obj->setPosY(value, ani); })
        .def("ScrollTop", [](ScrollPane* obj) { obj->scrollTop(); })
        .def("ScrollBottom", [](ScrollPane* obj) { obj->scrollBottom(); })
        .def("CancelDragging", &ScrollPane::cancelDragging)
        .def("LockHeader", &ScrollPane::lockHeader)
        .def("LockFooter", &ScrollPane::lockFooter)
        .def("GetHeader", &ScrollPane::getHeader)
        .def("GetFooter", &ScrollPane::getFooter)
        .def("IsBottomMost", &ScrollPane::isBottomMost)
        .def("IsRightMost", &ScrollPane::isRightMost);
        
    nb::class_<GList, GComponent>(m, "GList")
        .def("SetVirtual", sol::resolve<void()>(&GList::setVirtual))
        .def("SetVirtualAndLoop", &GList::setVirtualAndLoop)
        .def("SetNumItems", &GList::setNumItems)
        .def("GetNumItems", &GList::getNumItems)
        .def("AddSelection", &GList::addSelection)
        .def("GetSelectedIndex", &GList::getSelectedIndex)
        .def("SetSelectedIndex", &GList::setSelectedIndex)
        .def("SetItemRenderer", [](GList* obj, nb::callable func) { obj->itemRenderer = [func](int index, GObject* item) { CALL_PYTHON(func, index, item) }; })
        .def("SetItemProvider", [](GList* obj, nb::callable func) { obj->itemProvider = [func](int index) { CALL_PYTHON(func, index) return nb::cast<std::string>(result); }; })
        .def("AddItemFromPool", sol::resolve<GObject * ()>(&GList::addItemFromPool))
        .def("AddItemFromPool", [](GList* obj, const char* url) { return obj->addItemFromPool(url); });
        
    nb::class_<Window, GComponent>(m, "Window")
        .def("Create", &Window::create)
        .def("Show", &Window::show)
        .def("Hide", &Window::hide)
        .def("HideImmediately", &Window::hideImmediately)
        .def("SetModal", &Window::setModal)
        .def("GetContentPane", &Window::getContentPane)
        .def("SetContentPane", &Window::setContentPane)
        .def("ShowModalWait", sol::resolve<void()>(&Window::showModalWait))
        .def("CloseModalWait", sol::resolve<bool()>(&Window::closeModalWait));
        
    nb::class_<GTweener>(m, "GTweener")
        .def("SetTarget", [](GTweener* obj, GObject* target) { return obj->setTarget(target); })
        .def("SetTarget", [](GTweener* obj, GObject* target, TweenPropType propType) { return obj->setTarget(target, propType); })
        .def("SetRepeat", [](GTweener* obj, int repeat) {obj->setRepeat(repeat); })
        .def("OnStart", [](GTweener* obj, sol::function func) { obj->onStart([func](GTweener* tweener) { CALL_PYTHON(func, tweener) }); })
        .def("OnUpdate", [](GTweener* obj, sol::function func) { obj->onUpdate([func](GTweener* tweener) { CALL_PYTHON(func, tweener) }); })
        .def("OnComplete", [](GTweener* obj, sol::function func) { obj->onComplete([func]() { CALL_PYTHON(func) }); })
        .def("OnComplete1", [](GTweener* obj, sol::function func) { obj->onComplete1([func](GTweener* tweener) { CALL_PYTHON(func, tweener) }); })
        .def("value", [](GTweener* obj) { return Vector4{ obj->value.x, obj->value.y, obj->value.z, obj->value.w }; });
    
    nb::class_<GTween>(m, "GTween")
        .def("To", [](float startValue, float endValue, float duration) { return GTween::to(startValue, endValue, duration); })
        .def("To", [](const Vector2& startValue, const Vector2& endValue, float duration) { return GTween::to(*(cocos2d::Vec2*)startValue.Data(), *(cocos2d::Vec2*)endValue.Data(), duration); })
        .def("To", [](const Vector3& startValue, const Vector3& endValue, float duration) { return GTween::to(*(cocos2d::Vec3*)startValue.Data(), *(cocos2d::Vec3*)endValue.Data(), duration); })
        .def("To", [](const Vector4& startValue, const Vector4& endValue, float duration) { return GTween::to(*(cocos2d::Vec4*)startValue.Data(), *(cocos2d::Vec4*)endValue.Data(), duration); })
        .def("To", [](const Color& startValue, const Color& endValue, float duration) { return GTween::to(Color4BFromUrhoColor(startValue), Color4BFromUrhoColor(endValue), duration); })
        .def("delayedCall", &GTween::delayedCall)
        .def("Shake", [](const Vector2& startValue, float amplitude, float duration) { return GTween::shake(*(cocos2d::Vec2*)startValue.Data(), amplitude, duration); });

    nb::class_<GTreeNode>(m, "GTreeNode")
        .def("Create", []() { return GTreeNode::create(); })
        .def("Create", [](bool isFolder) { return GTreeNode::create(isFolder); })
        .def("AddChild", &GTreeNode::addChild)
        .def("SetData", &GTreeNode::setData)
        .def("GetData", &GTreeNode::getData)
        .def("SetTable", &GTreeNode::setTable)
        .def("GetTable", &GTreeNode::getTable)
        .def("GetCell", &GTreeNode::getCell)
        .def("IsFolder", &GTreeNode::isFolder)
        .def("GetText", &GTreeNode::getText);
        
    nb::class_<GTree, GList>(m, "GTree")
        .def("GetRootNode", &GTree::getRootNode)
        .def("GetSelectedNode", &GTree::getSelectedNode)
        .def("SetItemRenderer", [](GTree* obj, sol::function func) { obj->treeNodeRender = [func](GTreeNode* node, GComponent* obj) { CALL_PYTHON(func, node, obj) }; });
        
    Context* context = nullptr;
    m.def("GetRoot", [context]() { return context->GetSubsystem<GUI>()->GetFairyGUIRoot(); });
    m.def("CreateText", []() { return GBasicTextField::create(); });
    m.def("CreateText", [](const std::string& text) {
        auto t = GBasicTextField::create();
        t->setText(text);
        t->setColor({ 255, 255, 255 });
        return t; });
    m.def("CreateText", [](const std::string& text, const Urho3D::Color& color) {
        auto t = GBasicTextField::create();
        t->setText(text);
        t->setColor(Color3BFromUrhoColor(color));
        return t;
    });
    m.def("CreateRichText", []() { return GRichTextField::create(); });
    m.def("CreateRichText", [](const std::string& text) {
        auto t = GRichTextField::create();
        t->setText(text);
        t->setColor({ 255, 255, 255 });
        return t; });
    m.def("CreateRichText", [](const std::string& text, const Urho3D::Color& color) {
            auto t = GRichTextField::create();
            t->setText(text);
            t->setColor(Color3BFromUrhoColor(color));
            return t;
        });
    m.def("CreateButton", []() { return GButton::create(); });
    m.def("RegisterFont", &UIConfig::registerFont);
    m.def("IsFocusUI", [context]() { return context->GetSubsystem<GUI>()->IsFocusUI(); });
    m.def("IsInputing", [context]() { return context->GetSubsystem<GUI>()->IsInputing(); });
	//BindUIConfig(lua);
    RegisterUIEventTypeConst(m);
	RegisterRelationTypeConst(m);
    nb::enum_<PopupDirection>(m, "PopupDirection")
        .value("AUTO", PopupDirection::DOWN)
        .value("UP", PopupDirection::UP)
        .value("DOWN", PopupDirection::DOWN);

    nb::class_<FairyGUIScene>(m, "FairyGUIScene")
        //         sol::call_constructor, sol::factories([]() { return std::unique_ptr<FairyGUIScene>(FairyGUIScene::create()); }),
        //         sol::base_classes, sol::bases<cocos2d::Scene>());
        .def_prop_ro("groot", [](FairyGUIScene* self) { return self->_groot; })
        .def("Schedule", [](FairyGUIScene* self, nb::callable func, const char* key) { self->schedule([func](float dt) { CALL_PYTHON(func, dt) }, key); })
        .def("ScheduleOnce", [](FairyGUIScene* self, nb::callable func, float delay, const char* key) { self->scheduleOnce([func](float dt) { CALL_PYTHON(func, dt) }, delay, key); });

    m.def("SetDesignResolutionSize", [context](int w, int h) { return context->GetSubsystem<GUI>()->SetDesignResolutionSize(w, h); });
    m.def("ReplaceScene", [context](FairyGUIScene* scene) { context->GetSubsystem<GUI>()->SetFairyGUIRoot(scene->_groot); });
    m.def("ScheduleScriptFunc", [](nb::callable func, float interval, bool paused) { return cocos2d::Director::getInstance()->getScheduler()->scheduleScriptFunc(func, interval, paused); });
    m.def("UnscheduleScriptEntry", [](unsigned int scheduleScriptEntryID) { cocos2d::Director::getInstance()->getScheduler()->unscheduleScriptEntry(scheduleScriptEntryID); });
    m.def("StartDrag", [](const ea::string& icon, const ea::string& sourceData, int touchPointID) { DragDropManager::getInstance()->startDrag(icon.c_str(), cocos2d::Value(sourceData.c_str()), touchPointID); });
    m.def("StartDrag", [](const ea::string& icon, const int sourceData, int touchPointID) { DragDropManager::getInstance()->startDrag(icon.c_str(), cocos2d::Value(sourceData), touchPointID); });
    m.def("StartDrag", [](const ea::string& icon, const float sourceData, int touchPointID) { DragDropManager::getInstance()->startDrag(icon.c_str(), cocos2d::Value(sourceData), touchPointID); });
    m.def("StartDrag", [](const ea::string& icon, const bool sourceData, int touchPointID) { DragDropManager::getInstance()->startDrag(icon.c_str(), cocos2d::Value(sourceData), touchPointID); });
    m.def("EmojiParser", [](const char* str) {
        return EmojiParser::getInstance()->parse(str);
    });
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
    m.def("SetUIConfig", [](sol::table tb) {
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
    });
}
#endif
