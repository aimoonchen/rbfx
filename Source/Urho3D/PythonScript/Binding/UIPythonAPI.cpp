#include <nanobind/nanobind.h>
#include "../../Core/Context.h"
#include "../../UI/UI.h"
#include "../../UI/UIElement.h"
#include "../../UI/Sprite.h"
#include "../../UI/Font.h"
#include "../../UI/Text.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;

// namespace sol {
//     int sol_lua_push(sol::types<UIElement*>, lua_State* L, const UIElement* obj) {
//         if (obj) {
//             if (obj->GetTypeName() == "Sprite") {
//                 return sol::make_object(L, static_cast<const Sprite*>(obj)).push(L);
//             } else if (obj->GetTypeName() == "Text") {
//                 return sol::make_object(L, static_cast<const Text*>(obj)).push(L);
//             }
//         }
//         return sol::make_object(L, obj).push(L);
//     }
// }
// 
// Urho3D::Context* GetContext(lua_State* L);

NB_MODULE(ui, m)
{
    nb::class_<UIElement>(m, "UIElement")//, sol::constructors<UIElement(Context*)>());
        .def("SetSize", nb::overload_cast<int, int>(&UIElement::SetSize), nb::overload_cast<const IntVector2&>(&UIElement::SetSize))
        .def("SetPosition", nb::overload_cast<int, int>(&UIElement::SetPosition), nb::overload_cast<const IntVector2&>(&UIElement::SetPosition))
        .def("SetAlignment", &UIElement::SetAlignment)
        .def("CreateChild", [](UIElement* obj, StringHash typeName) { return obj->CreateChild(typeName, EMPTY_STRING, M_MAX_UNSIGNED); })
        .def_prop_rw("opacity", &UIElement::GetOpacity, &UIElement::SetOpacity)
        .def_prop_rw("horizontalAlignment", &UIElement::GetHorizontalAlignment, &UIElement::SetHorizontalAlignment)
        .def_prop_rw("verticalAlignment", &UIElement::GetVerticalAlignment, &UIElement::SetVerticalAlignment)
        .def_prop_ro("width", &UIElement::GetWidth)
        .def_prop_ro("height", &UIElement::GetHeight);
    
    nb::class_<BorderImage, UIElement>(m, "BorderImage");// , sol::constructors<BorderImage(Context*)>(), sol::base_classes, sol::bases<UIElement>());

    nb::class_<Sprite, UIElement>(m, "Sprite")//, sol::constructors<Sprite(Context*)>(), sol::base_classes, sol::bases<UIElement>());
        .def("SetTexture", &Sprite::SetTexture)
        .def("SetScale", nb::overload_cast<float>(&Sprite::SetScale))
        .def("SetScale", nb::overload_cast<float, float>(&Sprite::SetScale))
        .def("SetScale", nb::overload_cast<const Vector2&>(&Sprite::SetScale))
        .def_prop_rw("hotSpot", &Sprite::GetHotSpot, nb::overload_cast<const IntVector2&>(&Sprite::SetHotSpot))
        .def_prop_rw("opacity", &Sprite::GetOpacity, &Sprite::SetOpacity)
        .def_prop_rw("priority", &Sprite::GetPriority, &Sprite::SetPriority);
        
    nb::class_<Font, Resource>(m, "Font");// , sol::constructors<Font(Context*)>(), sol::base_classes, sol::bases<Resource>());

    nb::class_<UISelectable, UIElement>(m, "UISelectable");// , sol::base_classes, sol::bases<UIElement>());

    nb::class_<Text, UISelectable>(m, "Text")// , sol::constructors<Text(Context*)>(), sol::base_classes, sol::bases<UISelectable, UIElement>());
        .def("SetText", &Text::SetText)
        .def("SetFont", nb::overload_cast<const ea::string&, float>(&Text::SetFont))
        .def("SetFont", nb::overload_cast<Font*, float>(&Text::SetFont))//[](Text* obj, Font* font, float fontsize) { obj->SetFont(font, fontsize); },
        .def_prop_rw("textAlignment", &Text::GetTextAlignment, &Text::SetTextAlignment);
        
    nb::class_<UI>(m, "UI")//, sol::constructors<UI(Context*)>());
        .def_prop_ro("root", &UI::GetRoot)
        .def_prop_ro("focusElement", &UI::GetFocusElement);

//     auto context = GetContext(lua);
//     lua["ui"] = context->GetSubsystem<UI>();
    //
    m.attr("HA_LEFT")       = HA_LEFT;
    m.attr("HA_CENTER")     = HA_CENTER;
    m.attr("HA_RIGHT")      = HA_RIGHT;
    m.attr("HA_CUSTOM")     = HA_CUSTOM;
    m.attr("VA_TOP")        = VA_TOP;
    m.attr("VA_CENTER")     = VA_CENTER;
    m.attr("VA_BOTTOM")     = VA_BOTTOM;
    m.attr("VA_CUSTOM")     = VA_CUSTOM;
    m.attr("O_HORIZONTAL")  = O_HORIZONTAL;
    m.attr("O_VERTICAL")    = O_VERTICAL;
}
