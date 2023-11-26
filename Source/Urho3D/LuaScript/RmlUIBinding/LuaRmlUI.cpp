#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/Input.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/Dictionary.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/SystemInterface.h>
#include <RmlUi/Core/ElementText.h>
#include <RmlUi/Core/Elements/ElementFormControl.h>
#include <RmlUi/Core/Elements/ElementForm.h>
#include <RmlUi/Core/Elements/ElementFormControlInput.h>
#include <RmlUi/Core/Elements/ElementFormControlSelect.h>
#include <RmlUi/Core/Elements/ElementFormControlTextArea.h>
#include <RmlUi/Core/Elements/ElementTabSet.h>
#include <RmlUi/Core/Tween.h>
#include <RmlUi/Core/Transform.h>
#include <RmlUi/Core/TransformPrimitive.h>
#include <RmlUi/Core/PropertyDictionary.h>
#include <RmlUi/Core/StyleSheetSpecification.h>
#include "../../Core/Context.h"
#include "../../IO/Log.h"
#include "../../RmlUI/RmlUI.h"
#include "../../RmlUI/RmlUIComponent.h"
#include "LuaEventListener.h"
#include "LuaDataModel.h"
#include "LuaDataSource.h"
#include "LuaDataFormatter.h"
#include <sol/sol.hpp>

Urho3D::Context* GetContext(lua_State* L);

#ifdef GetFirstChild
    #undef GetFirstChild
#endif
#ifdef GetNextSibling
    #undef GetNextSibling
#endif
namespace sol {
    int sol_lua_push(sol::types<Rml::Element*>, lua_State* L, const Rml::Element* el)
    {
        if (el) {
            auto typeName = rmlui_type_name(el);
            const auto& tag = el->GetTagName();
            if (tag == "form") {
                return sol::make_object(L, static_cast<const Rml::ElementForm*>(el)).push(L);
            }
            else if (tag == "input") {
                return sol::make_object(L, static_cast<const Rml::ElementFormControlInput*>(el)).push(L);
            }
            else if (tag == "select") {
                return sol::make_object(L, static_cast<const Rml::ElementFormControlSelect*>(el)).push(L);
            }
            else if (tag == "#text") {
                return sol::make_object(L, static_cast<const Rml::ElementText*>(el)).push(L);
            }
            else if (tag == "label") {
                ;// return sol::make_object(L, static_cast<const Rml::ElementLabel*>(el)).push(L);
            }
            else if (tag == "textarea") {
                return sol::make_object(L, static_cast<const Rml::ElementFormControlTextArea*>(el)).push(L);
            }
            else if (tag == "#selection") {
                ;// return sol::make_object(L, static_cast<const Rml::ElementTextSelection*>(el)).push(L);
            }
            else if (tag == "tabset") {
                return sol::make_object(L, static_cast<const Rml::ElementTabSet*>(el)).push(L);
            }
            else if (tag == "progress" || tag == "progressbar") {
                ;// return sol::make_object(L, static_cast<const Rml::ElementProgress*>(el)).push(L);
            }
            else if (tag == "datagridexpand") {
                ;// return sol::make_object(L, static_cast<const Rml::ElementDataGridExpandButton*>(el)).push(L);
            }
            else if (tag == "#rmlctl_datagridcell") {
                ;// return sol::make_object(L, static_cast<const Rml::ElementDataGridCell*>(el)).push(L);
            }
        }
        return sol::make_object(L, el).push(L);
    }

    int sol_lua_push(sol::types<Rml::Variant*>, lua_State* L, const Rml::Variant* var)
    {
        if (var) {
            switch (var->GetType()) {
            case Rml::Variant::BOOL:
                return sol::make_object(L, var->Get<bool>()).push(L);
            case Rml::Variant::BYTE:
            case Rml::Variant::CHAR:
            case Rml::Variant::INT:
                return sol::make_object(L, var->Get<int>()).push(L);
            case Rml::Variant::INT64:
                return sol::make_object(L, var->Get<int64_t>()).push(L);
            case Rml::Variant::UINT:
                return sol::make_object(L, var->Get<unsigned int>()).push(L);
            case Rml::Variant::UINT64:
                return sol::make_object(L, var->Get<uint64_t>()).push(L);
            case Rml::Variant::FLOAT:
            case Rml::Variant::DOUBLE:
                return sol::make_object(L, var->Get<double>()).push(L);
            case Rml::Variant::COLOURB:
                // LuaType<Colourb>::push(L, new Colourb(var->Get<Colourb>()), true);
                break;
            case Rml::Variant::COLOURF:
                // LuaType<Colourf>::push(L, new Colourf(var->Get<Colourf>()), true);
                break;
            case Rml::Variant::STRING:
                return sol::make_object(L, var->GetReference<Rml::String>()).push(L);
            case Rml::Variant::VECTOR2:
                // according to Variant.inl, it is going to be a Vector2f
                // LuaType<Vector2f>::push(L, new Vector2f(var->Get<Vector2f>()), true);
                break;
            case Rml::Variant::VOIDPTR:
                return sol::make_object(L, var->Get<void*>()).push(L);
            default:
                break;
            }
        }
        return sol::make_object(L, var).push(L);
    }
}
static Rml::Property PropertyFromString(const Rml::String& name, const Rml::String& value)
{
    Rml::PropertyDictionary properties;
    if (!Rml::StyleSheetSpecification::ParsePropertyDeclaration(properties, name, value)) {
        URHO3D_LOGWARNINGF("Syntax error parsing inline property declaration '%s: %s;'.", name.c_str(), value.c_str());
        return Rml::Property();
    }
    return *properties.GetProperty(Rml::StyleSheetSpecification::GetPropertyId(name));
    //return properties.GetProperties().begin()->second;
};

static void RegisterInput(sol::state& lua)
{
    auto input = lua["rmlui"]["Input"].get_or_create<sol::table>();
    input["KI_UNKNOWN"] = Rml::Input::KI_UNKNOWN;
    input["KI_SPACE"] = Rml::Input::KI_SPACE;
    input["KI_0"] = Rml::Input::KI_0;
    input["KI_1"] = Rml::Input::KI_1;
    input["KI_2"] = Rml::Input::KI_2;
    input["KI_3"] = Rml::Input::KI_3;
    input["KI_4"] = Rml::Input::KI_4;
    input["KI_5"] = Rml::Input::KI_5;
    input["KI_6"] = Rml::Input::KI_6;
    input["KI_7"] = Rml::Input::KI_7;
    input["KI_8"] = Rml::Input::KI_8;
    input["KI_9"] = Rml::Input::KI_9;
    input["KI_A"] = Rml::Input::KI_A;
    input["KI_B"] = Rml::Input::KI_B;
    input["KI_C"] = Rml::Input::KI_C;
    input["KI_D"] = Rml::Input::KI_D;
    input["KI_E"] = Rml::Input::KI_E;
    input["KI_F"] = Rml::Input::KI_F;
    input["KI_G"] = Rml::Input::KI_G;
    input["KI_H"] = Rml::Input::KI_H;
    input["KI_I"] = Rml::Input::KI_I;
    input["KI_J"] = Rml::Input::KI_J;
    input["KI_K"] = Rml::Input::KI_K;
    input["KI_L"] = Rml::Input::KI_L;
    input["KI_M"] = Rml::Input::KI_M;
    input["KI_N"] = Rml::Input::KI_N;
    input["KI_O"] = Rml::Input::KI_O;
    input["KI_P"] = Rml::Input::KI_P;
    input["KI_Q"] = Rml::Input::KI_Q;
    input["KI_R"] = Rml::Input::KI_R;
    input["KI_S"] = Rml::Input::KI_S;
    input["KI_T"] = Rml::Input::KI_T;
    input["KI_U"] = Rml::Input::KI_U;
    input["KI_V"] = Rml::Input::KI_V;
    input["KI_W"] = Rml::Input::KI_W;
    input["KI_X"] = Rml::Input::KI_X;
    input["KI_Y"] = Rml::Input::KI_Y;
    input["KI_Z"] = Rml::Input::KI_Z;
    input["KI_BACK"] = Rml::Input::KI_BACK;
    input["KI_TAB"] = Rml::Input::KI_TAB;
    input["KI_RETURN"] = Rml::Input::KI_RETURN;
    input["KI_ESCAPE"] = Rml::Input::KI_ESCAPE;
    input["KI_PRIOR"] = Rml::Input::KI_PRIOR;
    input["KI_NEXT"] = Rml::Input::KI_NEXT;
    input["KI_END"] = Rml::Input::KI_END;
    input["KI_HOME"] = Rml::Input::KI_HOME;
    input["KI_LEFT"] = Rml::Input::KI_LEFT;
    input["KI_UP"] = Rml::Input::KI_UP;
    input["KI_RIGHT"] = Rml::Input::KI_RIGHT;
    input["KI_DOWN"] = Rml::Input::KI_DOWN;
    input["KI_INSERT"] = Rml::Input::KI_INSERT;
    input["KI_DELETE"] = Rml::Input::KI_DELETE;
    input["KI_F1"] = Rml::Input::KI_F1;
    input["KI_F2"] = Rml::Input::KI_F2;
    input["KI_F3"] = Rml::Input::KI_F3;
    input["KI_F4"] = Rml::Input::KI_F4;
    input["KI_F5"] = Rml::Input::KI_F5;
    input["KI_F6"] = Rml::Input::KI_F6;
    input["KI_F7"] = Rml::Input::KI_F7;
    input["KI_F8"] = Rml::Input::KI_F8;
    input["KI_F9"] = Rml::Input::KI_F9;
    input["KI_F10"] = Rml::Input::KI_F10;
    input["KI_F11"] = Rml::Input::KI_F11;
    input["KI_F12"] = Rml::Input::KI_F12;
    input["KI_LSHIFT"] = Rml::Input::KI_LSHIFT;
    input["KI_RSHIFT"] = Rml::Input::KI_RSHIFT;
    input["KI_LCONTROL"] = Rml::Input::KI_LCONTROL;
    input["KI_RCONTROL"] = Rml::Input::KI_RCONTROL;
}

static void RegisterTween(sol::state& lua)
{
    auto tween = lua["rmlui"]["Tween"].get_or_create<sol::table>();
    // tween type
    tween["None"] = Rml::Tween::None;
    tween["Back"] = Rml::Tween::Back;
    tween["Bounce"] = Rml::Tween::Bounce;
    tween["Circular"] = Rml::Tween::Circular;
    tween["Cubic"] = Rml::Tween::Cubic;
    tween["Elastic"] = Rml::Tween::Elastic;
    tween["Exponential"] = Rml::Tween::Exponential;
    tween["Linear"] = Rml::Tween::Linear;
    tween["Quadratic"] = Rml::Tween::Quadratic;
    tween["Quartic"] = Rml::Tween::Quartic;
    tween["Quintic"] = Rml::Tween::Quintic;
    tween["Sine"] = Rml::Tween::Sine;
    tween["Callback"] = Rml::Tween::Callback;
    tween["Count"] = Rml::Tween::Count;
    // tween direction
    tween["In"] = Rml::Tween::In;
    tween["Out"] = Rml::Tween::Out;
    tween["InOut"] = Rml::Tween::InOut;
}

static void RegisterProperty(sol::state& lua)
{
    // Basic types.
    auto unit = lua["rmlui"]["Unit"].get_or_create<sol::table>();
    unit["UNKNOWN"] = Rml::Unit::UNKNOWN;

    // Basic types.
    unit["KEYWORD"] = Rml::Unit::KEYWORD;
    unit["STRING"] = Rml::Unit::STRING;
    unit["COLOUR"] = Rml::Unit::COLOUR;
    unit["RATIO"] = Rml::Unit::RATIO;

    // Numeric values.
    unit["NUMBER"] = Rml::Unit::NUMBER;
    unit["PERCENT"] = Rml::Unit::PERCENT;
    unit["PX"] = Rml::Unit::PX;

    // Context-relative values.
    unit["DP"] = Rml::Unit::DP;
    unit["VW"] = Rml::Unit::VW;
    unit["VH"] = Rml::Unit::VH;
    unit["X"] = Rml::Unit::X;

    // Font-relative values.
    unit["EM"] = Rml::Unit::EM;
    unit["REM"] = Rml::Unit::REM;

    // Values based on pixels-per-inch.
    unit["INCH"] = Rml::Unit::INCH;
    unit["CM"] = Rml::Unit::CM;
    unit["MM"] = Rml::Unit::MM;
    unit["PT"] = Rml::Unit::PT;
    unit["PC"] = Rml::Unit::PC;
    unit["PPI_UNIT"] = Rml::Unit::PPI_UNIT;

    // Angles.
    unit["DEG"] = Rml::Unit::DEG;
    unit["RAD"] = Rml::Unit::RAD;

    // Values tied to specific types.
    unit["TRANSFORM"] = Rml::Unit::TRANSFORM;
    unit["TRANSITION"] = Rml::Unit::TRANSITION;
    unit["ANIMATION"] = Rml::Unit::ANIMATION;
    unit["DECORATOR"] = Rml::Unit::DECORATOR;
    unit["FONTEFFECT"] = Rml::Unit::FONTEFFECT;
    unit["COLORSTOPLIST"] = Rml::Unit::COLORSTOPLIST;
    unit["SHADOWLIST"] = Rml::Unit::SHADOWLIST;

    unit["LENGTH"] = Rml::Unit::LENGTH;
    unit["NUMBER_LENGTH_PERCENT"] = Rml::Unit::NUMBER_LENGTH_PERCENT;
    unit["NUMERIC"] = Rml::Unit::NUMERIC;
}

static void RegisterTransform(sol::state& lua)
{
    auto rmlui = lua["rmlui"].get_or_create<sol::table>();
    rmlui["TranslateX"] = Rml::TransformPrimitive::TRANSLATEX;
    rmlui["TranslateY"] = Rml::TransformPrimitive::TRANSLATEY;
    rmlui["TranslateZ"] = Rml::TransformPrimitive::TRANSLATEZ;
    rmlui["Translate2D"] = Rml::TransformPrimitive::TRANSLATE2D;
    rmlui["Translate3D"] = Rml::TransformPrimitive::TRANSLATE3D;
    rmlui["ScaleX"] = Rml::TransformPrimitive::SCALEX;
    rmlui["ScaleY"] = Rml::TransformPrimitive::SCALEY;
    rmlui["ScaleZ"] = Rml::TransformPrimitive::SCALEZ;
    rmlui["Scale2D"] = Rml::TransformPrimitive::SCALE2D;
    rmlui["Scale3D"] = Rml::TransformPrimitive::SCALE3D;
    rmlui["RotateX"] = Rml::TransformPrimitive::ROTATEX;
    rmlui["RotateY"] = Rml::TransformPrimitive::ROTATEY;
    rmlui["RotateZ"] = Rml::TransformPrimitive::ROTATEZ;
    rmlui["Rotate2D"] = Rml::TransformPrimitive::ROTATE2D;
    rmlui["Rotate3D"] = Rml::TransformPrimitive::ROTATE3D;
    rmlui["SkewX"] = Rml::TransformPrimitive::SKEWX;
    rmlui["SkewY"] = Rml::TransformPrimitive::SKEWY;
    rmlui["Skew2D"] = Rml::TransformPrimitive::SKEW2D;
    rmlui.new_usertype<Rml::TransformPrimitive>("Transform", sol::call_constructor, sol::factories(
        [](Rml::TransformPrimitive::Type type, Rml::Unit unit, sol::variadic_args va) {
            switch (type) {
            case Rml::TransformPrimitive::TRANSLATEX: return Rml::TransformPrimitive(Rml::Transforms::TranslateX(va.get<float>(0), unit));
            case Rml::TransformPrimitive::TRANSLATEY: return Rml::TransformPrimitive(Rml::Transforms::TranslateY(va.get<float>(0), unit));
            case Rml::TransformPrimitive::TRANSLATEZ: return Rml::TransformPrimitive(Rml::Transforms::TranslateZ(va.get<float>(0), unit));
            case Rml::TransformPrimitive::TRANSLATE2D: return Rml::TransformPrimitive(Rml::Transforms::Translate2D(va.get<float>(0), va.get<float>(1), unit));
            case Rml::TransformPrimitive::TRANSLATE3D: return Rml::TransformPrimitive(Rml::Transforms::Translate3D(va.get<float>(0), va.get<float>(1), va.get<float>(2), unit));
            case Rml::TransformPrimitive::SCALEX: return Rml::TransformPrimitive(Rml::Transforms::ScaleX(va.get<float>(0)));
            case Rml::TransformPrimitive::SCALEY: return Rml::TransformPrimitive(Rml::Transforms::ScaleY(va.get<float>(0)));
            case Rml::TransformPrimitive::SCALEZ: return Rml::TransformPrimitive(Rml::Transforms::ScaleZ(va.get<float>(0)));
            case Rml::TransformPrimitive::SCALE2D: return (va.size() == 1) ? Rml::TransformPrimitive(Rml::Transforms::Scale2D(va.get<float>(0))) : Rml::TransformPrimitive(Rml::Transforms::Scale2D(va.get<float>(0), va.get<float>(1)));
            case Rml::TransformPrimitive::SCALE3D: return (va.size() == 1) ? Rml::TransformPrimitive(Rml::Transforms::Scale3D(va.get<float>(0))) : Rml::TransformPrimitive(Rml::Transforms::Scale3D(va.get<float>(0), va.get<float>(1), va.get<float>(2)));
            case Rml::TransformPrimitive::ROTATEX: return Rml::TransformPrimitive(Rml::Transforms::RotateX(va.get<float>(0), unit));
            case Rml::TransformPrimitive::ROTATEY: return Rml::TransformPrimitive(Rml::Transforms::RotateY(va.get<float>(0), unit));
            case Rml::TransformPrimitive::ROTATEZ: return Rml::TransformPrimitive(Rml::Transforms::RotateZ(va.get<float>(0), unit));
            case Rml::TransformPrimitive::ROTATE2D: return Rml::TransformPrimitive(Rml::Transforms::Rotate2D(va.get<float>(0), unit));
            case Rml::TransformPrimitive::ROTATE3D: return Rml::TransformPrimitive(Rml::Transforms::Rotate3D(va.get<float>(0), va.get<float>(1), va.get<float>(2), va.get<float>(3), unit));
            case Rml::TransformPrimitive::SKEWX: return Rml::TransformPrimitive(Rml::Transforms::SkewX(va.get<float>(0), unit));
            case Rml::TransformPrimitive::SKEWY: return Rml::TransformPrimitive(Rml::Transforms::SkewY(va.get<float>(0), unit));
            case Rml::TransformPrimitive::SKEW2D: return Rml::TransformPrimitive(Rml::Transforms::Skew2D(va.get<float>(0), va.get<float>(1), unit));
            default:
                break;
            }
            URHO3D_LOGERRORF("Cann't construct transform.");
            return Rml::TransformPrimitive(Rml::Transforms::TranslateX(0.0f, unit));
        }));
}

int sol2_RmlUI_open(sol::state& lua)
{
    auto rmlui = lua["rmlui"].get_or_create<sol::table>();
    rmlui["uicontext"].get_or_create<sol::table>();
    rmlui["script"].get_or_create<sol::table>();
    rmlui.new_enum("ModalFlag",
        "None", Rml::ModalFlag::None,
        "Modal", Rml::ModalFlag::Modal,
        "Keep", Rml::ModalFlag::Keep);
    rmlui.new_enum("FocusFlag",
        "None", Rml::FocusFlag::None,
        "Document", Rml::FocusFlag::Document,
        "Keep", Rml::FocusFlag::Keep,
        "Auto", Rml::FocusFlag::Auto);
    rmlui.new_usertype<Rml::Variant>("Variant", "Get", [](Rml::Variant* self) { return self; });

    rmlui.new_usertype<Rml::Context>("Context",
        "dimensions", sol::property(
            [](Rml::Context* self) { auto dim = self->GetDimensions(); return Urho3D::IntVector2{dim.x, dim.y}; },
            [](Rml::Context* self, const Urho3D::IntVector2& dim) { self->SetDimensions({ dim.x_, dim.y_ }); }),
        "dp_ratio", sol::property(&Rml::Context::GetDensityIndependentPixelRatio, &Rml::Context::SetDensityIndependentPixelRatio),
        "focus_element", sol::property(&Rml::Context::GetFocusElement),
        "hover_element", sol::property(&Rml::Context::GetHoverElement),
        "name", sol::property(&Rml::Context::GetName),
        "root_element", sol::property(&Rml::Context::GetRootElement),
        "GetNumDocuments", &Rml::Context::GetNumDocuments,
        "GetDocument", sol::overload(
            [](Rml::Context* self, const Rml::String& id) { return self->GetDocument(id); },
            [](Rml::Context* self, int index) { return self->GetDocument(index); }),
        "AddEventListener", [](Rml::Context* self, const Rml::String& event, sol::function function, bool capture, Rml::Element* element) {
            if (element) {
                element->AddEventListener(event, new Rml::Lua::LuaEventListener(function, element), capture);
            } else {
                self->AddEventListener(event, new Rml::Lua::LuaEventListener(function, nullptr), capture);
            }
        },
        "CreateDocument", &Rml::Context::CreateDocument,
        "LoadDocument", [](Rml::Context* self, const Rml::String& document_path) { return self->LoadDocument(document_path); },
        "Render", &Rml::Context::Render,
        "UnloadAllDocuments", &Rml::Context::UnloadAllDocuments,
        "UnloadDocument", &Rml::Context::UnloadDocument,
        "Update", &Rml::Context::Update,
        "IsMouseInteracting", &Rml::Context::IsMouseInteracting,
        "OpenDataModel", [&lua](Rml::Context* self) {
            auto L = lua.lua_state();
            return Rml::Lua::OpenLuaDataModel(L, self, 2, 3) ? sol::make_object(L, sol::stack::get<sol::userdata>(L, -1)) : sol::make_object(L, sol::lua_nil);
        },
        "CloseDataModel", [&lua](Rml::Context* self) { Rml::Lua::CloseLuaDataModel(lua.lua_state(), self); });

    rmlui.new_usertype<Rml::Event>("Event",
        "type", sol::property(&Rml::Event::GetType),
        "target_element", sol::property(&Rml::Event::GetTargetElement),
        "current_element", sol::property(&Rml::Event::GetCurrentElement),
        "parameters", sol::property(&Rml::Event::GetParameters),
        "GetParameter", [](Rml::Event* self, const Rml::String& key) {
            const auto& parameters = self->GetParameters();
            auto it = parameters.find(key);
            return (it != parameters.end()) ? (Rml::Variant*)&it->second : nullptr;
        },
        "StopPropagation", &Rml::Event::StopPropagation,
        "StopImmediatePropagation", &Rml::Event::StopImmediatePropagation);

    rmlui.new_usertype<Rml::Element>("Element",
        "attributes", sol::property(&Rml::Element::GetAttributes),
        //TODO: "child_nodes", Proxy: lua pairs()
        "class_name", sol::property(&Rml::Element::GetClassNames, &Rml::Element::SetClassNames),
        "client_left", sol::property(&Rml::Element::GetClientLeft),
        "client_height", sol::property(&Rml::Element::GetClientHeight),
        "client_top", sol::property(&Rml::Element::GetClientTop),
        "client_width", sol::property(&Rml::Element::GetClientWidth),
        "first_child", sol::property(&Rml::Element::GetFirstChild),
        "id", sol::property(&Rml::Element::GetId, &Rml::Element::SetId),
        "inner_rml", sol::property(sol::resolve<Rml::String(void) const>(&Rml::Element::GetInnerRML), &Rml::Element::SetInnerRML),
        "last_child", sol::property(&Rml::Element::GetLastChild),
        "next_sibling", sol::property(&Rml::Element::GetNextSibling),
        "offset_height", sol::property(&Rml::Element::GetOffsetHeight),
        "offset_left", sol::property(&Rml::Element::GetOffsetLeft),
        "offset_parent", sol::property(&Rml::Element::GetOffsetParent),
        "offset_top", sol::property(&Rml::Element::GetOffsetTop),
        "offset_width", sol::property(&Rml::Element::GetOffsetWidth),
        "owner_document", sol::property(&Rml::Element::GetOwnerDocument),
        "parent_node", sol::property(&Rml::Element::GetParentNode),
        "previous_sibling", sol::property(&Rml::Element::GetPreviousSibling),
        "scroll_height", sol::property(&Rml::Element::GetScrollHeight),
        "scroll_left", sol::property(&Rml::Element::GetScrollLeft, &Rml::Element::SetScrollLeft),
        "scroll_top", sol::property(&Rml::Element::GetScrollTop, &Rml::Element::SetScrollTop),
        "scroll_width", sol::property(&Rml::Element::GetScrollWidth),
        "style", sol::property([](Rml::Element* self, const Rml::String& name) { auto pro = self->GetProperty(name); return pro ? pro->ToString() : ""; }),
        "tag_name", sol::property(&Rml::Element::GetTagName),
        "GetAbsoluteOffset", [](Rml::Element* self) { auto offset = self->GetAbsoluteOffset(); return std::make_tuple(offset.x, offset.y); },
        "GetProperty", [](Rml::Element* self, const Rml::String& name) { auto pro = self->GetProperty(name); return pro ? pro->ToString() : ""; },
        "SetProperty", [](Rml::Element* self, const Rml::String& name, const Rml::String& value) { return self->SetProperty(name, value); },
        "RemoveProperty", sol::resolve<void(const Rml::String&)>(&Rml::Element::RemoveProperty),
        "SetAttribute", sol::overload(
            [](Rml::Element* self, const Rml::String& name, bool value) { self->SetAttribute(name, value); },
            [](Rml::Element* self, const Rml::String& name, int value) { self->SetAttribute(name, value); },
            [](Rml::Element* self, const Rml::String& name, float value) { self->SetAttribute(name, value); },
            [](Rml::Element* self, const Rml::String& name, const Rml::String& value) { self->SetAttribute(name, value); }),
        "AddEventListener", sol::overload(
            [](Rml::Element* self, const Rml::String& event, sol::function function) {
                self->AddEventListener(event, new Rml::Lua::LuaEventListener(function, self));
            },
            [](Rml::Element* self, const Rml::String& event, sol::function function, bool capture) {
                self->AddEventListener(event, new Rml::Lua::LuaEventListener(function, self), capture);
            }),
        "AppendChild", sol::overload(
            [](Rml::Element* self, Rml::ElementPtr& child) { return self->AppendChild(std::move(child)); },
            [](Rml::Element* self, Rml::ElementPtr& child, bool dom_element) { return self->AppendChild(std::move(child), dom_element); }),
        "Blur", &Rml::Element::Blur,
        "Click", &Rml::Element::Click,
        "DispatchEvent", sol::overload(
            [](Rml::Element* self, const Rml::String& type, const Rml::Dictionary& parameters) { return self->DispatchEvent(type, parameters); },
            [](Rml::Element* self, const Rml::String& type, const Rml::Dictionary& parameters, bool interruptible, bool bubbles) { return self->DispatchEvent(type, parameters, interruptible, bubbles); }),
        "Focus", &Rml::Element::Focus,
        "GetAttribute", [](Rml::Element* self, const Rml::String& name) { return self->GetAttribute(name); },
        "GetElementById", &Rml::Element::GetElementById,
        "GetElementsByTagName", &Rml::Element::GetElementsByTagName,
        "QuerySelector", &Rml::Element::QuerySelector,
        "QuerySelectorAll", &Rml::Element::QuerySelectorAll,
        "HasAttribute", &Rml::Element::HasAttribute,
        "HasChildNodes", &Rml::Element::HasChildNodes,
        "InsertBefore", [](Rml::Element* self, Rml::ElementPtr& element, Rml::Element* adjacent_element) { return self->InsertBefore(std::move(element), adjacent_element); },
        "IsClassSet", &Rml::Element::IsClassSet,
        "RemoveAttribute", &Rml::Element::RemoveAttribute,
        "RemoveChild", &Rml::Element::RemoveChild,
        "ReplaceChild", [](Rml::Element* self, Rml::ElementPtr& inserted_element, Rml::Element* replaced_element) { return self->ReplaceChild(std::move(inserted_element), replaced_element); },
        "ScrollIntoView", sol::resolve<void(bool)>(&Rml::Element::ScrollIntoView),
        "SetClass", &Rml::Element::SetClass,
        "GetNumChildren", &Rml::Element::GetNumChildren,
        "GetChild", &Rml::Element::GetChild,
        "Animate", sol::overload(
            [](Rml::Element* self, const Rml::String& name, sol::nested<Rml::Vector<Rml::TransformPrimitive>> trans, float duration) { return self->Animate(name, Rml::Transform::MakeProperty(trans.value()), duration); },
            [](Rml::Element* self, const Rml::String& name, sol::nested<Rml::Vector<Rml::TransformPrimitive>> trans, float duration, int tween_type, int tween_dir) { return self->Animate(name, Rml::Transform::MakeProperty(trans.value()), duration, Rml::Tween{ (Rml::Tween::Type)tween_type, (Rml::Tween::Direction)tween_dir }); },
            [](Rml::Element* self, const Rml::String& name, sol::nested<Rml::Vector<Rml::TransformPrimitive>> trans, float duration, int tween_type, int tween_dir, int num_iterations) { return self->Animate(name, Rml::Transform::MakeProperty(trans.value()), duration, Rml::Tween{ (Rml::Tween::Type)tween_type, (Rml::Tween::Direction)tween_dir }, num_iterations); },
            [](Rml::Element* self, const Rml::String& name, sol::nested<Rml::Vector<Rml::TransformPrimitive>> trans, float duration, int tween_type, int tween_dir, int num_iterations, bool alternate_direction) { return self->Animate(name, Rml::Transform::MakeProperty(trans.value()), duration, Rml::Tween{ (Rml::Tween::Type)tween_type, (Rml::Tween::Direction)tween_dir }, num_iterations, alternate_direction); },
            [](Rml::Element* self, const Rml::String& name, sol::nested<Rml::Vector<Rml::TransformPrimitive>> trans, float duration, int tween_type, int tween_dir, int num_iterations, bool alternate_direction, float delay) { return self->Animate(name, Rml::Transform::MakeProperty(trans.value()), duration, Rml::Tween{ (Rml::Tween::Type)tween_type, (Rml::Tween::Direction)tween_dir }, num_iterations, alternate_direction, delay); },
            [](Rml::Element* self, const Rml::String& name, const Rml::String& value, float duration) { return self->Animate(name, PropertyFromString(name, value), duration); },
            [](Rml::Element* self, const Rml::String& name, const Rml::String& value, float duration, int tween_type, int tween_dir) { return self->Animate(name, PropertyFromString(name, value), duration, Rml::Tween{ (Rml::Tween::Type)tween_type, (Rml::Tween::Direction)tween_dir }); },
            [](Rml::Element* self, const Rml::String& name, const Rml::String& value, float duration, int tween_type, int tween_dir, int num_iterations) { return self->Animate(name, PropertyFromString(name, value), duration, Rml::Tween{ (Rml::Tween::Type)tween_type, (Rml::Tween::Direction)tween_dir }, num_iterations); },
            [](Rml::Element* self, const Rml::String& name, const Rml::String& value, float duration, int tween_type, int tween_dir, int num_iterations, bool alternate_direction) { return self->Animate(name, PropertyFromString(name, value), duration, Rml::Tween{ (Rml::Tween::Type)tween_type, (Rml::Tween::Direction)tween_dir }, num_iterations, alternate_direction); },
            [](Rml::Element* self, const Rml::String& name, const Rml::String& value, float duration, int tween_type, int tween_dir, int num_iterations, bool alternate_direction, float delay) { return self->Animate(name, PropertyFromString(name, value), duration, Rml::Tween{ (Rml::Tween::Type)tween_type, (Rml::Tween::Direction)tween_dir }, num_iterations, alternate_direction, delay); },
            [](Rml::Element* self, const Rml::String& name, const Rml::String& value, float duration, int tween_type, int tween_dir, int num_iterations, bool alternate_direction, float delay, const Rml::String& start_value) {
                auto start = PropertyFromString(name, start_value);
                return self->Animate(name, PropertyFromString(name, value), duration, Rml::Tween{ (Rml::Tween::Type)tween_type, (Rml::Tween::Direction)tween_dir }, num_iterations, alternate_direction, delay, &start);
            }),
        "AddAnimationKey", sol::overload(
            [](Rml::Element* self, const Rml::String& name, sol::nested<Rml::Vector<Rml::TransformPrimitive>> trans, float duration) { return self->AddAnimationKey(name, Rml::Transform::MakeProperty(trans.value()), duration); },
            [](Rml::Element* self, const Rml::String& name, sol::nested<Rml::Vector<Rml::TransformPrimitive>> trans, float duration, int tween_type, int tween_dir) { return self->AddAnimationKey(name, Rml::Transform::MakeProperty(trans.value()), duration, Rml::Tween{ (Rml::Tween::Type)tween_type, (Rml::Tween::Direction)tween_dir }); },
            [](Rml::Element* self, const Rml::String& name, const Rml::String& value, float duration) { return self->AddAnimationKey(name, PropertyFromString(name, value), duration); },
            [](Rml::Element* self, const Rml::String& name, const Rml::String& value, float duration, int tween_type, int tween_dir) { return self->AddAnimationKey(name, PropertyFromString(name, value), duration, Rml::Tween{ (Rml::Tween::Type)tween_type, (Rml::Tween::Direction)tween_dir }); }),
        "GetRealElement", [](Rml::Element* self) { return self; }
        );
    rmlui.new_usertype<Rml::ElementDocument>("Document",
        "title", sol::property(&Rml::ElementDocument::GetTitle, &Rml::ElementDocument::SetTitle),
        "context", sol::property(&Rml::ElementDocument::GetContext),
        "PullToFront", &Rml::ElementDocument::PullToFront,
        "PushToBack", &Rml::ElementDocument::PushToBack,
        "Show", sol::overload(
            [](Rml::ElementDocument* self) { self->Show(); },
            [](Rml::ElementDocument* self, Rml::ModalFlag modal_flag) { self->Show(modal_flag); },
            [](Rml::ElementDocument* self, Rml::ModalFlag modal_flag, Rml::FocusFlag focus_flag) { self->Show(modal_flag, focus_flag); }),
        "Hide", &Rml::ElementDocument::Hide,
        "Close", &Rml::ElementDocument::Close,
        "IsModal", &Rml::ElementDocument::IsModal,
        "CreateElement", &Rml::ElementDocument::CreateElement,
        "CreateTextNode", &Rml::ElementDocument::CreateTextNode,
        "GetSourceURL", &Rml::ElementDocument::GetSourceURL,
        "UpdateDocument", &Rml::ElementDocument::UpdateDocument,
        //"SetStyleSheetContainer", &Rml::ElementDocument::SetStyleSheetContainer,
        sol::base_classes, sol::bases<Rml::Element>());

    rmlui.new_usertype<Rml::ElementTabSet>("ElementTabSet",
        "active_tab", sol::property(&Rml::ElementTabSet::GetActiveTab, &Rml::ElementTabSet::SetActiveTab),
        "num_tabs", sol::property(&Rml::ElementTabSet::GetNumTabs),
        "SetPanel", sol::resolve<void(int, const Rml::String&)>(&Rml::ElementTabSet::SetPanel),
        "SetTab", sol::resolve<void(int, const Rml::String&)>(&Rml::ElementTabSet::SetTab),
        sol::base_classes, sol::bases<Rml::Element>());

    rmlui.new_usertype<Rml::ElementText>("ElementText",
        "text", sol::property(&Rml::ElementText::GetText, &Rml::ElementText::SetText),
        sol::base_classes, sol::bases<Rml::Element>());

    rmlui.new_usertype<Rml::ElementForm>("ElementForm",
        "Submit", &Rml::ElementForm::Submit,
        sol::base_classes, sol::bases<Rml::Element>());

    rmlui.new_usertype<Rml::ElementFormControl>("ElementFormControl",
        "name", sol::property(&Rml::ElementFormControl::GetName, &Rml::ElementFormControl::SetName),
        "disabled", sol::property(&Rml::ElementFormControl::IsDisabled, &Rml::ElementFormControl::SetDisabled),
        "value", sol::property(&Rml::ElementFormControl::GetValue, &Rml::ElementFormControl::SetValue),
        sol::base_classes, sol::bases<Rml::Element>());

    rmlui.new_usertype<Rml::ElementFormControlInput>("ElementFormControlInput",
        "checked", sol::property([](Rml::ElementFormControlInput* self) { return self->GetAttribute("checked"); }, [](Rml::ElementFormControlInput* self, bool value) { self->SetAttribute("checked", value); }),
        "maxlength", sol::property([](Rml::ElementFormControlInput* self) { return self->GetAttribute("maxlength"); }, [](Rml::ElementFormControlInput* self, int value) { self->SetAttribute("maxlength", value); }),
        "size", sol::property([](Rml::ElementFormControlInput* self) { return self->GetAttribute("size"); }, [](Rml::ElementFormControlInput* self, int value) { self->SetAttribute("size", value); }),
        "max", sol::property([](Rml::ElementFormControlInput* self) { return self->GetAttribute("max"); }, [](Rml::ElementFormControlInput* self, int value) { self->SetAttribute("max", value); }),
        "min", sol::property([](Rml::ElementFormControlInput* self) { return self->GetAttribute("min"); }, [](Rml::ElementFormControlInput* self, int value) { self->SetAttribute("min", value); }),
        "step", sol::property([](Rml::ElementFormControlInput* self) { return self->GetAttribute("step"); }, [](Rml::ElementFormControlInput* self, int value) { self->SetAttribute("step", value); }),
        "value", sol::property(&Rml::ElementFormControlInput::GetValue, &Rml::ElementFormControlInput::SetValue),
        "Select", &Rml::ElementFormControlInput::Select,
        "SetSelection", &Rml::ElementFormControlInput::SetSelectionRange,
        "GetSelection", [](Rml::ElementFormControlInput* self) {
            int selection_start = 0;
            int selection_end = 0;
            Rml::String selected_text;
            self->GetSelection(&selection_start, &selection_end, &selected_text);
            return std::make_tuple(selection_start, selection_end, selected_text);
        },
        sol::base_classes, sol::bases<Rml::ElementFormControl, Rml::Element>());

    rmlui.new_usertype<Rml::ElementFormControlSelect>("ElementFormControlSelect",
        "selection", sol::property(&Rml::ElementFormControlSelect::GetSelection, &Rml::ElementFormControlSelect::SetSelection),
        "value", sol::property(&Rml::ElementFormControlSelect::GetValue, &Rml::ElementFormControlSelect::SetValue),
        //TODO: options proxy: pairs()
        "GetOption", &Rml::ElementFormControlSelect::GetOption,
        "GetNumOptions", &Rml::ElementFormControlSelect::GetNumOptions,
        "Add", sol::overload(
            [](Rml::ElementFormControlSelect* self, const Rml::String& rml, const Rml::String& value) { return self->Add(rml, value); },
            [](Rml::ElementFormControlSelect* self, const Rml::String& rml, const Rml::String& value, int before) { return self->Add(rml, value, before); },
            [](Rml::ElementFormControlSelect* self, const Rml::String& rml, const Rml::String& value, int before, bool selectable) { return self->Add(rml, value, before, selectable); } ),
        "Remove", &Rml::ElementFormControlSelect::Remove,
        "RemoveAll", &Rml::ElementFormControlSelect::RemoveAll,
        sol::base_classes, sol::bases<Rml::ElementFormControl, Rml::Element>());

    rmlui.new_usertype<Rml::ElementFormControlTextArea>("ElementFormControlTextArea",
        "maxlength", sol::property(&Rml::ElementFormControlTextArea::GetMaxLength, &Rml::ElementFormControlTextArea::SetMaxLength),
        "wordwrap", sol::property(&Rml::ElementFormControlTextArea::GetWordWrap, &Rml::ElementFormControlTextArea::SetWordWrap),
        "rows", sol::property(&Rml::ElementFormControlTextArea::GetNumRows, &Rml::ElementFormControlTextArea::SetNumRows),
        "cols", sol::property(&Rml::ElementFormControlTextArea::GetNumColumns, &Rml::ElementFormControlTextArea::SetNumColumns),
        "value", sol::property(&Rml::ElementFormControlTextArea::GetValue, &Rml::ElementFormControlTextArea::SetValue),
        "Select", &Rml::ElementFormControlTextArea::Select,
        "SetSelection", &Rml::ElementFormControlTextArea::SetSelectionRange,
        "GetSelection", [](Rml::ElementFormControlTextArea* self) {
            int selection_start = 0;
            int selection_end = 0;
            Rml::String selected_text;
            self->GetSelection(&selection_start, &selection_end, &selected_text);
            return std::make_tuple(selection_start, selection_end, selected_text);
        },
        sol::base_classes, sol::bases<Rml::ElementFormControl, Rml::Element>());

    RegisterInput(lua);
    RegisterProperty(lua);
    RegisterTween(lua);
    RegisterTransform(lua);
    auto rml = GetContext(lua.lua_state())->GetSubsystem<Urho3D::RmlUI>();
    rmlui["LoadFont"] = sol::overload(
        [rml](const Rml::String& filename, bool fallback) { return rml->LoadFont(filename.c_str(), fallback); },
        //[rml](const Rml::String& familyname, const Rml::String& filename) { return rml->LoadFont(ea::string{familyname.c_str()}, ea::string{filename.c_str()}); },
        [rml](const Rml::String& familyname, const Rml::String& filename, bool italic, bool bold, bool fallback) { return rml->LoadFont(familyname.c_str(), filename.c_str(), italic, bold, fallback); });
    auto rmlctx = rml->GetRmlContext();
    rmlui["GetRmlUIComponent"] = sol::overload(
        [rmlctx](const Rml::String& id) {
            auto doc = rmlctx->GetDocument(id);
            return doc ? Urho3D::RmlUIComponent::FromDocument(doc) : (Urho3D::RmlUIComponent*)nullptr;
        },
        [](Rml::ElementDocument* doc) { return Urho3D::RmlUIComponent::FromDocument(doc); });
    rmlui["context"] = rmlctx;
    rmlui["EncodeRml"] = [](const Rml::String& str) { return Rml::StringUtilities::EncodeRml(str); };
    rmlui["GetElapsedTime"] = []() { return Rml::GetSystemInterface()->GetElapsedTime(); };
    return 0;
}
