#include "../../Core/Context.h"
#include "../../UI/UI.h"
#include "../../UI/UIElement.h"
#include "../../UI/Sprite.h"
#include "../../UI/Font.h"
#include "../../UI/Text.h"
#include <sol/sol.hpp>
#include "GetPush.h"

using namespace Urho3D;

namespace sol {
    int sol_lua_push(sol::types<UIElement*>, lua_State* L, const UIElement* obj) {
        if (obj) {
            if (obj->GetTypeName() == "Sprite") {
                return sol::make_object(L, static_cast<const Sprite*>(obj)).push(L);
            } else if (obj->GetTypeName() == "Text") {
                return sol::make_object(L, static_cast<const Text*>(obj)).push(L);
            }
        }
        return sol::make_object(L, obj).push(L);
    }
}

Urho3D::Context* GetContext(lua_State* L);

int sol2_UILuaAPI_open(sol::state& lua)
{
    auto bindUIElement = lua.new_usertype<UIElement>("UIElement", sol::constructors<UIElement(Context*)>());
    bindUIElement["SetSize"]                = sol::overload(sol::resolve<void(int, int)>(&UIElement::SetSize), sol::resolve<void(const IntVector2&)>(&UIElement::SetSize));
    bindUIElement["SetPosition"]            = sol::overload(sol::resolve<void(int, int)>(&UIElement::SetPosition), sol::resolve<void(const IntVector2&)>(&UIElement::SetPosition));
    bindUIElement["SetAlignment"]           = &UIElement::SetAlignment;
    bindUIElement["CreateChild"]            = [](UIElement* obj, StringHash typeName) { return obj->CreateChild(typeName, EMPTY_STRING, M_MAX_UNSIGNED); };
    bindUIElement["opacity"]                = sol::property(&UIElement::GetOpacity, &UIElement::SetOpacity);
    bindUIElement["horizontalAlignment"]    = sol::property(&UIElement::GetHorizontalAlignment, &UIElement::SetHorizontalAlignment);
    bindUIElement["verticalAlignment"]      = sol::property(&UIElement::GetVerticalAlignment, &UIElement::SetVerticalAlignment);
    bindUIElement["width"]                  = sol::property(&UIElement::GetWidth);
    bindUIElement["height"]                 = sol::property(&UIElement::GetHeight);
    
    lua.new_usertype<BorderImage>("BorderImage", sol::constructors<BorderImage(Context*)>(), sol::base_classes, sol::bases<UIElement>());

    auto bindSprite = lua.new_usertype<Sprite>("Sprite", sol::constructors<Sprite(Context*)>(), sol::base_classes, sol::bases<UIElement>());
    bindSprite["SetTexture"]    = &Sprite::SetTexture;
    bindSprite["SetScale"]      = sol::overload(sol::resolve<void(float)>(&Sprite::SetScale),
        sol::resolve<void(float, float)>(&Sprite::SetScale),
        sol::resolve<void(const Vector2&)>(&Sprite::SetScale));
    bindSprite["hotSpot"]       = sol::property(&Sprite::GetHotSpot, sol::resolve<void(const IntVector2&)>(&Sprite::SetHotSpot));
    bindSprite["opacity"]       = sol::property(&Sprite::GetOpacity, &Sprite::SetOpacity);
    bindSprite["priority"]      = sol::property(&Sprite::GetPriority, &Sprite::SetPriority);
        
    lua.new_usertype<Font>("Font", sol::constructors<Font(Context*)>(), sol::base_classes, sol::bases<Resource>());

    lua.new_usertype<UISelectable>("UISelectable", sol::base_classes, sol::bases<UIElement>());

    auto bindText = lua.new_usertype<Text>("Text", sol::constructors<Text(Context*)>(), sol::base_classes, sol::bases<UISelectable, UIElement>());
    bindText["SetText"]         = &Text::SetText;
    bindText["SetFont"]         = sol::overload(sol::resolve<bool(const ea::string&, float)>(&Text::SetFont), sol::resolve<bool(Font*, float)>(&Text::SetFont));//[](Text* obj, Font* font, float fontsize) { obj->SetFont(font, fontsize); },
    bindText["textAlignment"]   = sol::property(&Text::GetTextAlignment, &Text::SetTextAlignment);
        
    auto bindUI = lua.new_usertype<UI>("UI", sol::constructors<UI(Context*)>());
    bindUI["root"]              = sol::property(&UI::GetRoot);
    bindUI["focusElement"]      = sol::property(&UI::GetFocusElement);

    auto context = GetContext(lua);
    lua["ui"] = context->GetSubsystem<UI>();
    //
    lua["HA_LEFT"]      = HA_LEFT;
    lua["HA_CENTER"]    = HA_CENTER;
    lua["HA_RIGHT"]     = HA_RIGHT;
    lua["HA_CUSTOM"]    = HA_CUSTOM;
    //
    lua["VA_TOP"]       = VA_TOP;
    lua["VA_CENTER"]    = VA_CENTER;
    lua["VA_BOTTOM"]    = VA_BOTTOM;
    lua["VA_CUSTOM"]    = VA_CUSTOM;
    //
    lua["O_HORIZONTAL"] = O_HORIZONTAL;
    lua["O_VERTICAL"]   = O_VERTICAL;
    return 0;
}
