#include <sol/sol.hpp>
#include "../../Math/BoundingBox.h"
#include "../../Math/Rect.h"
#include "../../Math/Plane.h"
#include "../../Math/Ray.h"
using namespace Urho3D;

int sol2_MathLuaAPI2_open(sol::state& lua)
{
    auto math3d = lua["math3d"].get_or_create<sol::table>();
	math3d.new_usertype<BoundingBox>("BoundingBox",
		sol::call_constructor, sol::factories([]() { return BoundingBox(); }, [](float min, float max) { return BoundingBox(min, max); }, [](const Vector3& min, const Vector3& max) { return BoundingBox(min, max); }),
		"min", &BoundingBox::min_,
		"max", &BoundingBox::max_,
        "Define", sol::overload([](BoundingBox* self, const Vector3& point) { self->Define(point); }, sol::resolve<void(const Vector3&, const Vector3&)>(&BoundingBox::Define)),
        "Merge", sol::overload([](BoundingBox* self, const Vector3& point) { self->Merge(point); }, sol::resolve<void(const BoundingBox&)>(&BoundingBox::Merge)),
        "Clip", &BoundingBox::Clip,
        "Size", &BoundingBox::Size,
        "Center", &BoundingBox::Center,
        "IsInside", sol::overload(
            [](BoundingBox* self, const BoundingBox& box) { return self->IsInside(box); },
            [](BoundingBox* self, const Vector3& point) { return self->IsInside(point); })
        );

    math3d.new_usertype<Rect>("Rect",
        sol::call_constructor, sol::factories(
            []() { return Rect(); },
            [](float left, float top, float right, float bottom) { return Rect(left, top, right, bottom); },
            [](const Vector2& min, const Vector2& max) { return Rect(min, max); }),
        "Size",     &Rect::Size,
        "Center",   &Rect::Center,
        "Left",     &Rect::Left,
        "Top",      &Rect::Top,
        "Right",    &Rect::Right,
        "Bottom",   &Rect::Bottom,
        "min",      &Rect::min_,
        "max",      &Rect::max_);

    math3d.new_usertype<Plane>("Plane",
        sol::call_constructor, sol::factories(
            []() { return Plane(); },
            [](const Vector3& v0, const Vector3& v1, const Vector3& v2) { return Plane(v0, v1, v2); },
            [](const Vector3& normal, const Vector3& point) { return Plane(normal, point); })
        );

    math3d.new_usertype<Ray>("Ray",
        sol::call_constructor, sol::factories(
            []() { return Ray(); },
            [](const Vector3& origin, const Vector3& direction) { return Ray(origin, direction); }),
        "HitDistance", sol::overload(
            sol::resolve<float(const Plane&) const>(&Ray::HitDistance),
            sol::resolve<float(const BoundingBox&) const>(&Ray::HitDistance),
            //sol::resolve<float(const Frustum&, bool) const>(&Ray::HitDistance),
            //sol::resolve<float(const Sphere&) const>(&Ray::HitDistance),
            [](Ray* self, const Vector3& v0, const Vector3& v1, const Vector3& v2) { return self->HitDistance(v0, v1, v2); }),
        "origin",       &Ray::origin_,
        "direction",    &Ray::direction_);
	return 0;
}
