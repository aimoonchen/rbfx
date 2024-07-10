#include "../../Core/Context.h"
#include "../../Resource/Resource.h"
#include "../../Resource/ResourceCache.h"
#include "../../Urho2D/Sprite2D.h"
#include "../../Graphics/Material.h"
#include "../../Urho2D/StaticSprite2D.h"
#include "../../Urho2D/AnimatedSprite2D.h"
#include "../../Urho2D/TileMap2D.h"
#include "../../Urho2D/TileMapLayer2D.h"
#include "../../Urho2D/TileMapDefs2D.h"
#include "../../Urho2D/StretchableSprite2D.h"

#include "GetPush.h"

using namespace Urho3D;

Urho3D::Context* GetContext(lua_State* L);

int sol2_Urho2DLuaAPI_open(sol::state& lua)
{
    auto context = GetContext(lua.lua_state());
    lua.new_enum("TileMapObjectType2D",
        "RECTANGLE",    OT_RECTANGLE,
        "ELLIPSE",      OT_ELLIPSE,
        "POLYGON",      OT_POLYGON,
        "POLYLINE",     OT_POLYLINE,
        "TILE",         OT_TILE,
        "INVALID",      OT_INVALID
    );

    auto bindDrawable2D = lua.new_usertype<Drawable2D>("Drawable2D", sol::base_classes, sol::bases<Drawable>());
    //bindDrawable2D["OnSetEnabled"]      = &Drawable2D::OnSetEnabled;
    bindDrawable2D["SetLayer"]          = &Drawable2D::SetLayer;
    bindDrawable2D["SetOrderInLayer"]   = &Drawable2D::SetOrderInLayer;
    bindDrawable2D["GetLayer"]          = &Drawable2D::GetLayer;
    bindDrawable2D["GetOrderInLayer"]   = &Drawable2D::GetOrderInLayer;

    auto bindStaticSprite2D = lua.new_usertype<StaticSprite2D>("StaticSprite2D",
        sol::base_classes, sol::bases<Drawable2D, Drawable, Component>());
    bindStaticSprite2D["id"]                = sol::var(StringHash("StaticSprite2D"));
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

    lua.new_enum("LoopMode2D",
        "DEFAULT",       LM_DEFAULT,
        "FORCE_LOOPED",  LM_FORCE_LOOPED,
        "FORCE_CLAMPED", LM_FORCE_CLAMPED
    );
    auto bindAnimatedSprite2D = lua.new_usertype<AnimatedSprite2D>("AnimatedSprite2D",
        sol::base_classes, sol::bases<StaticSprite2D, Drawable2D, Drawable, Component>());
    bindAnimatedSprite2D["id"]              = sol::var(StringHash("AnimatedSprite2D"));
    bindAnimatedSprite2D["SetAnimationSet"] = &AnimatedSprite2D::SetAnimationSet;
    bindAnimatedSprite2D["SetEntity"]       = &AnimatedSprite2D::SetEntity;
    bindAnimatedSprite2D["SetAnimation"]    = sol::overload(
        [](AnimatedSprite2D* self, const ea::string& name) { self->SetAnimation(name); },
        [](AnimatedSprite2D* self, const ea::string& name, LoopMode2D loopMode) { self->SetAnimation(name, loopMode); }
    );
    bindAnimatedSprite2D["SetLoopMode"]     = &AnimatedSprite2D::SetLoopMode;
    bindAnimatedSprite2D["SetSpeed"]        = &AnimatedSprite2D::SetSpeed;
    bindAnimatedSprite2D["GetLoopMode"]     = &AnimatedSprite2D::GetLoopMode;
    bindAnimatedSprite2D["GetSpeed"]        = &AnimatedSprite2D::GetSpeed;
    bindAnimatedSprite2D["GetAnimation"]    = &AnimatedSprite2D::GetAnimation;
    //bindAnimatedSprite2D["AnimationSet2D"]  = &AnimatedSprite2D::GetAnimationSet;

    lua.new_enum("Orientation2D",
        "ORTHOGONAL",   O_ORTHOGONAL,
        "ISOMETRIC",    O_ISOMETRIC,
        "STAGGERED",    O_STAGGERED,
        "HEXAGONAL",    O_HEXAGONAL
    );
    auto bindStretchableSprite2D = lua.new_usertype<StretchableSprite2D>("StretchableSprite2D",
        sol::base_classes, sol::bases<StaticSprite2D, Drawable2D, Drawable, Component>());
    bindStretchableSprite2D["id"]           = sol::var(StringHash("StretchableSprite2D"));
    bindStretchableSprite2D["SetBorder"]    = &StretchableSprite2D::SetBorder;
    bindStretchableSprite2D["GetBorder"]    = &StretchableSprite2D::GetBorder;

    auto bindTileMapInfo2D = lua.new_usertype<TileMapInfo2D>("TileMapInfo2D");
    bindTileMapInfo2D["width"]              = &TileMapInfo2D::width_;
    bindTileMapInfo2D["height"]             = &TileMapInfo2D::height_;
    bindTileMapInfo2D["tile_width"]         = &TileMapInfo2D::tileWidth_;
    bindTileMapInfo2D["tile_height"]        = &TileMapInfo2D::tileHeight_;
    bindTileMapInfo2D["GetMapWidth"]        = &TileMapInfo2D::GetMapWidth;
    bindTileMapInfo2D["GetMapHeight"]       = &TileMapInfo2D::GetMapHeight;
    bindTileMapInfo2D["ConvertPosition"]    = &TileMapInfo2D::ConvertPosition;
    bindTileMapInfo2D["TileIndexToPosition"] = &TileMapInfo2D::TileIndexToPosition;
    bindTileMapInfo2D["PositionToTileIndex"] = [](TileMapInfo2D* self, const Vector2& position) {
        int x = 0;
        int y = 0;
        auto ret = self->PositionToTileIndex(x, y, position);
        return std::tuple<bool, int, int>(ret, x, y);
    };

    auto bindTileMap2D = lua.new_usertype<TileMap2D>("TileMap2D", sol::base_classes, sol::bases<Component>());
    bindTileMap2D["id"]                     = sol::var(StringHash("TileMap2D"));
    bindTileMap2D["SetTmxFile"]             = &TileMap2D::SetTmxFile;
    bindTileMap2D["GetInfo"]                = &TileMap2D::GetInfo;
    bindTileMap2D["GetLayer"]               = &TileMap2D::GetLayer;
    bindTileMap2D["GetNumLayers"]           = &TileMap2D::GetNumLayers;

    auto bindTileMapObject2D = lua.new_usertype<TileMapObject2D>("TileMapObject2D");
    bindTileMapObject2D["GetObjectType"]    = &TileMapObject2D::GetObjectType;
    bindTileMapObject2D["GetName"]          = &TileMapObject2D::GetName;
    bindTileMapObject2D["GetSize"]          = &TileMapObject2D::GetSize;
    bindTileMapObject2D["GetPosition"]      = &TileMapObject2D::GetPosition;
    bindTileMapObject2D["GetTileSprite"]    = &TileMapObject2D::GetTileSprite;
    bindTileMapObject2D["HasProperty"]      = &TileMapObject2D::HasProperty;
    bindTileMapObject2D["GetProperty"]      = &TileMapObject2D::GetProperty;
    bindTileMapObject2D["GetNumPoints"]     = &TileMapObject2D::GetNumPoints;
    bindTileMapObject2D["GetPoint"]         = &TileMapObject2D::GetPoint;

    auto bindTileMapLayer2D = lua.new_usertype<TileMapLayer2D>("TileMapLayer2D", sol::base_classes, sol::bases<Component>());
    bindTileMapLayer2D["id"]                = sol::var(StringHash("TileMapLayer2D"));
    bindTileMapLayer2D["GetNumObjects"]     = &TileMapLayer2D::GetNumObjects;
    bindTileMapLayer2D["GetObject"]         = &TileMapLayer2D::GetObject;

    return 0;
}
