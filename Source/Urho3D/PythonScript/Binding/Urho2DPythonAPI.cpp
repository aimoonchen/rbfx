#include <nanobind/nanobind.h>
#include "Urho3D/Core/Context.h"
#include "Urho3D/Resource/Resource.h"
#include "Urho3D/Resource/ResourceCache.h"
#include "Urho3D/Urho2D/Sprite2D.h"
#include "Urho3D/Graphics/Material.h"
#include "Urho3D/Urho2D/StaticSprite2D.h"
#include "Urho3D/Urho2D/AnimatedSprite2D.h"
#include "Urho3D/Urho2D/TileMap2D.h"
#include "Urho3D/Urho2D/TileMapLayer2D.h"
#include "Urho3D/Urho2D/TileMapDefs2D.h"
#include "Urho3D/Urho2D/StretchableSprite2D.h"
#include "Urho3D/Urho2D/AnimationSet2D.h"
#include "Urho3D/Urho2D/TmxFile2D.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;

NB_MODULE(urho2d, m)
{
    Context* context = nullptr;
    nb::enum_<TileMapObjectType2D>(m, "TileMapObjectType2D")
        .value("RECTANGLE",    OT_RECTANGLE)
        .value("ELLIPSE",      OT_ELLIPSE)
        .value("POLYGON",      OT_POLYGON)
        .value("POLYLINE",     OT_POLYLINE)
        .value("TILE",         OT_TILE)
        .value("INVALID",      OT_INVALID);

    nb::class_<Drawable2D, Drawable>(m, "Drawable2D")
        .def("SetLayer", & Drawable2D::SetLayer)
        .def("SetOrderInLayer", & Drawable2D::SetOrderInLayer)
        .def("GetLayer", & Drawable2D::GetLayer)
        .def("GetOrderInLayer", & Drawable2D::GetOrderInLayer);

    nb::class_<StaticSprite2D, Drawable2D>(m, "StaticSprite2D")
    //bindStaticSprite2D["id"]                = sol::var(StringHash("StaticSprite2D"));
        .def("SetSprite", &StaticSprite2D::SetSprite)
        .def("SetDrawRect", &StaticSprite2D::SetDrawRect)
        .def("SetTextureRect", &StaticSprite2D::SetTextureRect)
        .def("SetBlendMode", &StaticSprite2D::SetBlendMode)
        .def("SetFlip", [](StaticSprite2D* self, bool flipX, bool flipY) { return self->SetFlip(flipX, flipY); })
        .def("SetFlip", [](StaticSprite2D* self, bool flipX, bool flipY, bool swapXY) { return self->SetFlip(flipX, flipY, swapXY); })
        .def("SetFlipX", &StaticSprite2D::SetFlipX)
        .def("SetFlipY", &StaticSprite2D::SetFlipY)
        .def("SetSwapXY", &StaticSprite2D::SetSwapXY)
        .def("SetColor", &StaticSprite2D::SetColor)
        .def("SetAlpha", &StaticSprite2D::SetAlpha)
        .def("SetUseHotSpot", &StaticSprite2D::SetUseHotSpot)
        .def("SetUseDrawRect", &StaticSprite2D::SetUseDrawRect)
        .def("SetUseTextureRect", &StaticSprite2D::SetUseTextureRect)
        .def("SetHotSpot", &StaticSprite2D::SetHotSpot)
        .def("SetCustomMaterial", &StaticSprite2D::SetCustomMaterial)
        .def("SetSpriteAttr", &StaticSprite2D::SetSpriteAttr)
        .def("SetCustomMaterialAttr", &StaticSprite2D::SetCustomMaterialAttr)
        .def("GetSprite", &StaticSprite2D::GetSprite)
        .def("GetDrawRect", &StaticSprite2D::GetDrawRect)
        .def("GetTextureRect", &StaticSprite2D::GetTextureRect)
        .def("GetBlendMode", &StaticSprite2D::GetBlendMode)
        .def("GetFlipX", &StaticSprite2D::GetFlipX)
        .def("GetFlipY", &StaticSprite2D::GetFlipY)
        .def("GetSwapXY", &StaticSprite2D::GetSwapXY)
        .def("GetColor", &StaticSprite2D::GetColor)
        .def("GetAlpha", &StaticSprite2D::GetAlpha)
        .def("GetUseHotSpot", &StaticSprite2D::GetUseHotSpot)
        .def("GetUseDrawRect", &StaticSprite2D::GetUseDrawRect)
        .def("GetUseTextureRect", &StaticSprite2D::GetUseTextureRect)
        .def("GetHotSpot", &StaticSprite2D::GetHotSpot)
        .def("GetCustomMaterial", &StaticSprite2D::GetCustomMaterial);

    nb::enum_<LoopMode2D>(m, "LoopMode2D")
        .value("DEFAULT", LM_DEFAULT)
        .value("FORCE_LOOPED", LM_FORCE_LOOPED)
        .value("FORCE_CLAMPED", LM_FORCE_CLAMPED);

    nb::class_<AnimatedSprite2D, StaticSprite2D>(m, "AnimatedSprite2D")
        //bindAnimatedSprite2D["id"]              = sol::var(StringHash("AnimatedSprite2D"));
        .def("SetAnimationSet", & AnimatedSprite2D::SetAnimationSet)
        .def("SetEntity", & AnimatedSprite2D::SetEntity)
        .def("SetAnimation", [](AnimatedSprite2D* self, const ea::string& name) { self->SetAnimation(name); })
        .def("SetAnimation", [](AnimatedSprite2D* self, const ea::string& name, LoopMode2D loopMode) { self->SetAnimation(name, loopMode); })
        .def("SetLoopMode", & AnimatedSprite2D::SetLoopMode)
        .def("SetSpeed", & AnimatedSprite2D::SetSpeed)
        .def("GetLoopMode", & AnimatedSprite2D::GetLoopMode)
        .def("GetSpeed", & AnimatedSprite2D::GetSpeed)
        .def("GetAnimation", & AnimatedSprite2D::GetAnimation);

    nb::enum_<Orientation2D>(m, "Orientation2D")
        .value("ORTHOGONAL",   O_ORTHOGONAL)
        .value("ISOMETRIC",    O_ISOMETRIC)
        .value("STAGGERED",    O_STAGGERED)
        .value("HEXAGONAL",    O_HEXAGONAL);

    nb::class_<StretchableSprite2D, StaticSprite2D>(m, "StretchableSprite2D")
    //bindStretchableSprite2D["id"]           = sol::var(StringHash("StretchableSprite2D"));
        .def("SetBorder", &StretchableSprite2D::SetBorder)
        .def("GetBorder", &StretchableSprite2D::GetBorder);

    nb::class_<TileMapInfo2D>(m, "TileMapInfo2D")
        .def_rw("width", &TileMapInfo2D::width_)
        .def_rw("height", &TileMapInfo2D::height_)
        .def_rw("tile_width", &TileMapInfo2D::tileWidth_)
        .def_rw("tile_height", &TileMapInfo2D::tileHeight_)
        .def("GetMapWidth", &TileMapInfo2D::GetMapWidth)
        .def("GetMapHeight", &TileMapInfo2D::GetMapHeight)
        .def("ConvertPosition", &TileMapInfo2D::ConvertPosition)
        .def("TileIndexToPosition", &TileMapInfo2D::TileIndexToPosition)
        .def("PositionToTileIndex", [](TileMapInfo2D* self, const Vector2& position) {
            int x = 0;
            int y = 0;
            auto ret = self->PositionToTileIndex(x, y, position);
            return std::tuple<bool, int, int>(ret, x, y);
        });

    nb::class_<TileMap2D, Component>(m, "TileMap2D")
    //bindTileMap2D["id"]                     = sol::var(StringHash("TileMap2D"));
        .def("SetTmxFile", &TileMap2D::SetTmxFile)
        .def("GetInfo", &TileMap2D::GetInfo)
        .def("GetLayer", &TileMap2D::GetLayer)
        .def("GetNumLayers", &TileMap2D::GetNumLayers);

    nb::class_<TileMapObject2D>(m, "TileMapObject2D")
        .def("GetObjectType", &TileMapObject2D::GetObjectType)
        .def("GetType", &TileMapObject2D::GetType)
        .def("GetName", &TileMapObject2D::GetName)
        .def("GetSize", &TileMapObject2D::GetSize)
        .def("GetPosition", &TileMapObject2D::GetPosition)
        .def("GetTileSprite", &TileMapObject2D::GetTileSprite)
        .def("HasProperty", &TileMapObject2D::HasProperty)
        .def("GetProperty", &TileMapObject2D::GetProperty)
        .def("GetNumPoints", &TileMapObject2D::GetNumPoints)
        .def("GetPoint", &TileMapObject2D::GetPoint);

    nb::class_<TileMapLayer2D, Component>(m, "TileMapLayer2D")
    //bindTileMapLayer2D["id"]                = sol::var(StringHash("TileMapLayer2D"));
        .def("GetNumObjects", &TileMapLayer2D::GetNumObjects)
        .def("GetObject", &TileMapLayer2D::GetObject);
}
