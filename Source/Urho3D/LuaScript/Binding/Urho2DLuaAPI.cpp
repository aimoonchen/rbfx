#include "../../Core/Context.h"
#include "../../Resource/Resource.h"
#include "../../Resource/ResourceCache.h"
#include "../../Urho2D/Sprite2D.h"
#include "../../Graphics/Material.h"
#include "../../Urho2D/StaticSprite2D.h"
#include "../../Urho2D/AnimatedSprite2D.h"
#include "GetPush.h"

using namespace Urho3D;

Urho3D::Context* GetContext(lua_State* L);

int sol2_Urho2DLuaAPI_open(sol::state& lua)
{
    auto context = GetContext(lua.lua_state());
//     auto bindSprite2D = lua.new_usertype<Sprite2D>("Sprite2D",
//         sol::base_classes, sol::bases<Resource>());
    auto bindStaticSprite2D = lua.new_usertype<StaticSprite2D>("StaticSprite2D",
        sol::base_classes, sol::bases<Drawable2D, Drawable, Component>());
    bindStaticSprite2D["id"] = sol::var(StringHash("StaticSprite2D"));
    bindStaticSprite2D["SetSprite"]         = &StaticSprite2D::SetSprite;
    bindStaticSprite2D["SetDrawRect"]       = &StaticSprite2D::SetDrawRect;
    bindStaticSprite2D["SetTextureRect"]    = &StaticSprite2D::SetTextureRect;
    bindStaticSprite2D["SetBlendMode"]      = &StaticSprite2D::SetBlendMode;
    bindStaticSprite2D["SetFlip"]           = sol::overload(
        [](StaticSprite2D* self, bool flipX, bool flipY) { return self->SetFlip(flipX, flipY); },
        [](StaticSprite2D* self, bool flipX, bool flipY, bool swapXY) { return self->SetFlip(flipX, flipY, swapXY); });
    bindStaticSprite2D["SetFlipX"]          = &StaticSprite2D::SetFlipX;
    bindStaticSprite2D["SetFlipY"]          = &StaticSprite2D::SetFlipY;
    bindStaticSprite2D["SetSwapXY"]         = &StaticSprite2D::SetSwapXY;
    bindStaticSprite2D["SetColor"]          = &StaticSprite2D::SetColor;
    bindStaticSprite2D["SetAlpha"]          = &StaticSprite2D::SetAlpha;
    bindStaticSprite2D["SetUseHotSpot"]     = &StaticSprite2D::SetUseHotSpot;
    bindStaticSprite2D["SetUseDrawRect"]    = &StaticSprite2D::SetUseDrawRect;
    bindStaticSprite2D["SetUseTextureRect"] = &StaticSprite2D::SetUseTextureRect;
    bindStaticSprite2D["SetHotSpot"]        = &StaticSprite2D::SetHotSpot;
    bindStaticSprite2D["SetCustomMaterial"] = &StaticSprite2D::SetCustomMaterial;
    bindStaticSprite2D["SetSpriteAttr"]     = &StaticSprite2D::SetSpriteAttr;
    bindStaticSprite2D["SetCustomMaterialAttr"] = &StaticSprite2D::SetCustomMaterialAttr;

    bindStaticSprite2D["GetSprite"]         = &StaticSprite2D::GetSprite;
    bindStaticSprite2D["GetDrawRect"]       = &StaticSprite2D::GetDrawRect;
    bindStaticSprite2D["GetTextureRect"]    = &StaticSprite2D::GetTextureRect;
    bindStaticSprite2D["GetBlendMode"]      = &StaticSprite2D::GetBlendMode;
    bindStaticSprite2D["GetFlipX"]          = &StaticSprite2D::GetFlipX;
    bindStaticSprite2D["GetFlipY"]          = &StaticSprite2D::GetFlipY;
    bindStaticSprite2D["GetSwapXY"]         = &StaticSprite2D::GetSwapXY;
    bindStaticSprite2D["GetColor"]          = &StaticSprite2D::GetColor;
    bindStaticSprite2D["GetAlpha"]          = &StaticSprite2D::GetAlpha;
    bindStaticSprite2D["GetUseHotSpot"]     = &StaticSprite2D::GetUseHotSpot;
    bindStaticSprite2D["GetUseDrawRect"]    = &StaticSprite2D::GetUseDrawRect;
    bindStaticSprite2D["GetUseTextureRect"] = &StaticSprite2D::GetUseTextureRect;
    bindStaticSprite2D["GetHotSpot"]        = &StaticSprite2D::GetHotSpot;
    bindStaticSprite2D["GetCustomMaterial"] = &StaticSprite2D::GetCustomMaterial;

    auto bindAnimatedSprite2D = lua.new_usertype<AnimatedSprite2D>("AnimatedSprite2D",
        sol::base_classes, sol::bases<AnimatedSprite2D, StaticSprite2D, Drawable2D, Drawable, Component>());
    
    return 0;
}
