#include <sol/sol.hpp>
#include "../../Math/Vector2.h"
#include "../../Math/Vector3.h"
#include "../../Math/Vector4.h"
#include "../../Math/Matrix3x4.h"
#include "../../Math/Quaternion.h"
#include "../../Math/Color.h"
#include "../../Math/BoundingBox.h"
#include "../../Math/Rect.h"
#include "../../Math/Plane.h"
#include "../../Math/Ray.h"
#include "../../Math/Transform.h"

using namespace Urho3D;

int sol2_MathLuaAPI_open(sol::state& lua)
{
    auto math3d = lua["math3d"].get_or_create<sol::table>();

    math3d.new_enum("Intersection",
        "OUTSIDE", OUTSIDE,
        "INTERSECTS", INTERSECTS,
        "INSIDE", INSIDE);

    auto bindIntVector2 = math3d.new_usertype<IntVector2>("IntVector2",
        sol::call_constructor,
        sol::constructors<IntVector2(), IntVector2(int, int)>());
	bindIntVector2["x"]     = &IntVector2::x_;
    bindIntVector2["y"]     = &IntVector2::y_;
    bindIntVector2["ZERO"]  = sol::var(std::ref(IntVector2::ZERO));
    bindIntVector2["LEFT"]  = sol::var(std::ref(IntVector2::LEFT));
    bindIntVector2["RIGHT"] = sol::var(std::ref(IntVector2::RIGHT));
    bindIntVector2["UP"]    = sol::var(std::ref(IntVector2::UP));
    bindIntVector2["DOWN"]  = sol::var(std::ref(IntVector2::DOWN));
    bindIntVector2["ONE"]   = sol::var(std::ref(IntVector2::ONE));
    bindIntVector2[sol::meta_function::addition]        = &IntVector2::operator+;
    bindIntVector2[sol::meta_function::subtraction]     = sol::resolve<IntVector2(const IntVector2&) const>(&IntVector2::operator-);
    bindIntVector2[sol::meta_function::multiplication]  = sol::overload(
        sol::resolve<IntVector2(const IntVector2&) const>(&IntVector2::operator*),
        sol::resolve<IntVector2(int) const>(&IntVector2::operator*));
    bindIntVector2[sol::meta_function::division]        = sol::overload(
        sol::resolve<IntVector2(const IntVector2&) const>(&IntVector2::operator/),
        sol::resolve<IntVector2(int) const>(&IntVector2::operator/));

    auto bindVector2 = math3d.new_usertype<Vector2>("Vector2",
        sol::call_constructor,
        sol::constructors<Vector2(), Vector2(float, float)>());
    bindVector2["x"]        = &Vector2::x_;
    bindVector2["y"]        = &Vector2::y_;
    bindVector2["ZERO"]     = sol::var(std::ref(Vector2::ZERO));
    bindVector2["LEFT"]     = sol::var(std::ref(Vector2::LEFT));
    bindVector2["RIGHT"]    = sol::var(std::ref(Vector2::RIGHT));
    bindVector2["UP"]       = sol::var(std::ref(Vector2::UP));
    bindVector2["DOWN"]     = sol::var(std::ref(Vector2::DOWN));
    bindVector2["ONE"]      = sol::var(std::ref(Vector2::ONE));
    bindVector2[sol::meta_function::addition]       = &Vector2::operator+;
    bindVector2[sol::meta_function::subtraction]    = sol::resolve<Vector2(const Vector2&) const>(&Vector2::operator-);
    bindVector2[sol::meta_function::multiplication] = sol::overload(
        sol::resolve<Vector2(const Vector2&) const>(&Vector2::operator*),
        sol::resolve<Vector2(float) const>(&Vector2::operator*));
    bindVector2[sol::meta_function::division]       = sol::overload(
        sol::resolve<Vector2(const Vector2&) const>(&Vector2::operator/),
        sol::resolve<Vector2(float) const>(&Vector2::operator/));

    auto bindVector3 = math3d.new_usertype<Vector3>("Vector3",
        sol::call_constructor,
        sol::constructors<Vector3(), Vector3(float, float), Vector3(float, float, float)>());
	bindVector3["Normalize"]            = &Vector3::Normalize;
	bindVector3["Length"]               = &Vector3::Length;
	bindVector3["LengthSquared"]        = &Vector3::LengthSquared;
	bindVector3["DotProduct"]           = &Vector3::DotProduct;
	bindVector3["AbsDotProduct"]        = &Vector3::AbsDotProduct;
	bindVector3["ProjectOntoAxis"]      = &Vector3::ProjectOntoAxis;
	bindVector3["ProjectOntoPlane"]     = &Vector3::ProjectOntoPlane;
	bindVector3["ProjectOntoLine"]      = &Vector3::ProjectOntoLine;
	bindVector3["DistanceToPoint"]      = &Vector3::DistanceToPoint;
	bindVector3["DistanceToPlane"]      = &Vector3::DistanceToPlane;
	bindVector3["Orthogonalize"]        = &Vector3::Orthogonalize;
	bindVector3["CrossProduct"]         = &Vector3::CrossProduct;
	bindVector3["Abs"]                  = &Vector3::Abs;
	bindVector3["Lerp"]                 = &Vector3::Lerp;
	bindVector3["Equals"]               = &Vector3::Equals;
	bindVector3["Angle"]                = &Vector3::Angle;
	bindVector3["IsNaN"]                = &Vector3::IsNaN;
	bindVector3["IsInf"]                = &Vector3::IsInf;
	bindVector3["Normalized"]           = &Vector3::Normalized,
	bindVector3["NormalizedOrDefault"]  = &Vector3::NormalizedOrDefault;
	bindVector3["ReNormalized"]         = &Vector3::ReNormalized;
	//"ToString",				&Vector3::ToString,
    bindVector3["ToHash"]               = &Vector3::ToHash;
	//sol::meta_function::to_string, &Vector3::ToString,
	bindVector3[sol::meta_function::addition]       = &Vector3::operator+;
	bindVector3[sol::meta_function::subtraction]    = sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator-);
	bindVector3[sol::meta_function::multiplication] = sol::overload(
        sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator*),
        sol::resolve<Vector3(float) const>(&Vector3::operator*));
    bindVector3[sol::meta_function::division]       = sol::overload(
        sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator/),
        sol::resolve<Vector3(float) const>(&Vector3::operator/));
	bindVector3["x"]        = &Vector3::x_;
	bindVector3["y"]        = &Vector3::y_;
	bindVector3["z"]        = &Vector3::z_;
    bindVector3["ZERO"]     = sol::var(std::ref(Vector3::ZERO));
    bindVector3["LEFT"]     = sol::var(std::ref(Vector3::LEFT));
    bindVector3["RIGHT"]    = sol::var(std::ref(Vector3::RIGHT));
    bindVector3["UP"]       = sol::var(std::ref(Vector3::UP));
    bindVector3["DOWN"]     = sol::var(std::ref(Vector3::DOWN));
    bindVector3["FORWARD"]  = sol::var(std::ref(Vector3::FORWARD));
    bindVector3["BACK"]     = sol::var(std::ref(Vector3::BACK));
    bindVector3["ONE"]      = sol::var(std::ref(Vector3::ONE));

    auto bindVector4 = math3d.new_usertype<Vector4>("Vector4",
        sol::call_constructor,
        sol::constructors<Vector4(), Vector4(float, float, float, float), Vector4(const Vector3&, float)>());
    bindVector4["x"] = &Vector4::x_;
    bindVector4["y"] = &Vector4::y_;
    bindVector4["z"] = &Vector4::z_;
    bindVector4["w"] = &Vector4::w_;

    auto bindQuaternion = math3d.new_usertype<Quaternion>("Quaternion",
        sol::call_constructor,
        sol::constructors<Quaternion(),
        Quaternion(float, float, float, float),
        Quaternion(float, const Vector3&),
        Quaternion(float),
        Quaternion(float, float, float),
        Quaternion(const Vector3&),
        Quaternion(const Vector3&, const Vector3&),
        Quaternion(const Vector3&, const Vector3&, const Vector3&)>());

    bindQuaternion["w"]                 = &Quaternion::w_;
    bindQuaternion["x"]                 = &Quaternion::x_;
    bindQuaternion["y"]                 = &Quaternion::y_;
    bindQuaternion["z"]                 = &Quaternion::z_;
    bindQuaternion["YawAngle"]          = &Quaternion::YawAngle;
    bindQuaternion["PitchAngle"]        = &Quaternion::PitchAngle;
    bindQuaternion["RollAngle"]         = &Quaternion::RollAngle;
    bindQuaternion["EulerAngles"]       = &Quaternion::EulerAngles;
    bindQuaternion["FromRotationTo"]    = &Quaternion::FromRotationTo;
    bindQuaternion["FromLookRotation"]  = sol::overload(
        [](Quaternion* self, const Vector3& direction) { return self->FromLookRotation(direction); },
        [](Quaternion* self, const Vector3& direction, const Vector3& up) { return self->FromLookRotation(direction, up); });
    bindQuaternion["Slerp"]             = &Quaternion::Slerp;
    bindQuaternion["Nlerp"]             = sol::overload(
        [](Quaternion* self, const Quaternion& rhs, float t) { return self->Nlerp(rhs, t); },
        [](Quaternion* self, const Quaternion& rhs, float t, bool shortestPath) { return self->Nlerp(rhs, t, shortestPath); });
    bindQuaternion["IDENTITY"]          = sol::var(std::ref(Quaternion::IDENTITY));
    bindQuaternion[sol::meta_function::multiplication] = sol::overload(
        sol::resolve<Quaternion(float) const>(&Quaternion::operator*),
        sol::resolve<Quaternion(const Quaternion&) const>(&Quaternion::operator*),
        sol::resolve<Vector3(const Vector3&) const>(&Quaternion::operator*));

    auto bindColor = math3d.new_usertype<Color>("Color",
        sol::call_constructor,
        sol::constructors<Color(),
        Color(float, float, float),
        Color(float, float, float, float),
        Color(uint8_t, uint8_t, uint8_t),
        Color(uint8_t, uint8_t, uint8_t, uint8_t)>());

	bindColor["r"]                  = &Color::r_;
	bindColor["g"]                  = &Color::g_;
	bindColor["b"]                  = &Color::b_;
    bindColor["a"]                  = &Color::a_;
    bindColor["WHITE"]              = sol::var(std::ref(Color::WHITE));
    bindColor["GRAY"]               = sol::var(std::ref(Color::GRAY));
    bindColor["BLACK"]              = sol::var(std::ref(Color::BLACK));
    bindColor["RED"]                = sol::var(std::ref(Color::RED));
    bindColor["GREEN"]              = sol::var(std::ref(Color::GREEN));
    bindColor["BLUE"]               = sol::var(std::ref(Color::BLUE));
    bindColor["CYAN"]               = sol::var(std::ref(Color::CYAN));
    bindColor["MAGENTA"]            = sol::var(std::ref(Color::MAGENTA));
    bindColor["YELLOW"]             = sol::var(std::ref(Color::YELLOW));
    bindColor["TRANSPARENT_BLACK"]  = sol::var(std::ref(Color::TRANSPARENT_BLACK));
    bindColor["LUMINOSITY_GAMMA"]   = sol::var(std::ref(Color::LUMINOSITY_GAMMA));
    bindColor["LUMINOSITY_LINEAR"]  = sol::var(std::ref(Color::LUMINOSITY_LINEAR));

    auto bindMatrix3x4 = math3d.new_usertype<Matrix3x4>("Matrix3x4",
        sol::call_constructor,
        sol::constructors<Matrix3x4(),
        Matrix3x4(const Vector3&, const Quaternion&, float scale),
        Matrix3x4(const Vector3&, const Quaternion&, const Vector3 & scale)>());

    bindMatrix3x4["Inverse"]    = &Matrix3x4::Inverse;
    //"ToString", &Matrix3x4::ToString,
    bindMatrix3x4[sol::meta_function::multiplication] = sol::overload(
            sol::resolve<Vector3(const Vector3&) const>(&Matrix3x4::operator*),
            sol::resolve<Vector3(const Vector4&) const>(&Matrix3x4::operator*),
            sol::resolve<Matrix3x4(const Matrix3x4&) const>(&Matrix3x4::operator*));
    bindMatrix3x4["ZERO"]       = sol::var(std::ref(Matrix3x4::ZERO));
    bindMatrix3x4["IDENTITY"]   = sol::var(std::ref(Matrix3x4::IDENTITY));

//     auto bindTransform = math3d.new_usertype<Transform>("Transform");
//     bindTransform["position"] = &Transform::position_;
//     bindTransform["rotation"] = &Transform::rotation_;
//     bindTransform["scale"] = &Transform::scale_;
//     bindTransform["FromMatrix3x4"] = &Transform::FromMatrix3x4;
//     bindTransform["ToMatrix3x4"] = &Transform::ToMatrix3x4;
//     bindTransform["Lerp"] = &Transform::Lerp;
//     bindTransform["Inverse"] = &Transform::Inverse;
//     bindTransform[sol::meta_function::multiplication] = sol::overload(
//         sol::resolve<Transform(const Transform&) const>(&Transform::operator*),
//         sol::resolve<Vector3(const Vector3&) const>(&Transform::operator*),
//         sol::resolve<Quaternion(const Quaternion&) const>(&Transform::operator*));

    auto bindBoundingBox = math3d.new_usertype<BoundingBox>("BoundingBox",
        sol::call_constructor,
        sol::constructors<BoundingBox(), BoundingBox(float min, float max), BoundingBox(const Vector3 & min, const Vector3 & max)>());
    bindBoundingBox["min"]      = &BoundingBox::min_;
    bindBoundingBox["max"]      = &BoundingBox::max_;
    bindBoundingBox["Define"]   = sol::overload([](BoundingBox* self, const Vector3& point) { self->Define(point); }, sol::resolve<void(const Vector3&, const Vector3&)>(&BoundingBox::Define));
    bindBoundingBox["Merge"]    = sol::overload([](BoundingBox* self, const Vector3& point) { self->Merge(point); }, sol::resolve<void(const BoundingBox&)>(&BoundingBox::Merge));
    bindBoundingBox["Clip"]     = &BoundingBox::Clip;
    bindBoundingBox["Size"]     = &BoundingBox::Size;
    bindBoundingBox["Center"]   = &BoundingBox::Center;
    bindBoundingBox["IsInside"] = sol::overload(
        [](BoundingBox* self, const BoundingBox& box) { return self->IsInside(box); },
        [](BoundingBox* self, const Vector3& point) { return self->IsInside(point); });

    auto bindRect = math3d.new_usertype<Rect>("Rect",
        sol::call_constructor,
        sol::constructors<Rect(),
        Rect(float, float, float, float),
        Rect(const Vector2&, const Vector2&)>());
    bindRect["Size"]    = &Rect::Size;
    bindRect["Center"]  = &Rect::Center;
    bindRect["Left"]    = &Rect::Left;
    bindRect["Top"]     = &Rect::Top;
    bindRect["Right"]   = &Rect::Right;
    bindRect["Bottom"]  = &Rect::Bottom;
    bindRect["min"]     = &Rect::min_;
    bindRect["max"]     = &Rect::max_;

    math3d.new_usertype<Plane>("Plane",
        sol::call_constructor,
        sol::constructors<Plane(),
        Plane(const Vector3&, const Vector3&, const Vector3&),
        Plane(const Vector3&, const Vector3&)>());

    auto bindRay = math3d.new_usertype<Ray>("Ray",
        sol::call_constructor, sol::constructors<Ray(), Ray(const Vector3&, const Vector3&)>());
    bindRay["HitDistance"] = sol::overload(
        sol::resolve<float(const Plane&) const>(&Ray::HitDistance),
        sol::resolve<float(const BoundingBox&) const>(&Ray::HitDistance),
        //sol::resolve<float(const Frustum&, bool) const>(&Ray::HitDistance),
        //sol::resolve<float(const Sphere&) const>(&Ray::HitDistance),
        [](Ray* self, const Vector3& v0, const Vector3& v1, const Vector3& v2) { return self->HitDistance(v0, v1, v2); });
    bindRay["origin"] = &Ray::origin_;
    bindRay["direction"] = &Ray::direction_;

	math3d["Random"] = sol::overload(
		sol::resolve<float(void)>(&Random),
        sol::resolve<float(float)>(&Random),
        sol::resolve<float(float, float)>(&Random),
        sol::resolve<int(int)>(&Random),
        sol::resolve<int(int, int)>(&Random));
    math3d["RandomNormal"] = RandomNormal;
    math3d["ClampF"] = Clamp<float>;
    math3d["ClampI"] = Clamp<int>;
    math3d["ModF"] = Mod<float>;
    math3d["ModI"] = Mod<int>;
    math3d["M_LARGE_EPSILON"] = M_LARGE_EPSILON;
    math3d["M_MAX_UNSIGNED"] = M_MAX_UNSIGNED;
    math3d["M_DEGTORAD"] = M_DEGTORAD;
    math3d["M_RADTODEG"] = M_RADTODEG;
    math3d["VectorMin"] = sol::overload(
        sol::resolve<IntVector2(const IntVector2&, const IntVector2&)>(&VectorMin),
        sol::resolve<Vector3(const Vector3&, const Vector3&)>(&VectorMin));
    math3d["VectorMax"] = sol::overload(
        sol::resolve<IntVector2(const IntVector2&, const IntVector2&)>(&VectorMax),
        sol::resolve<Vector3(const Vector3&, const Vector3&)> (&VectorMax));
	return 0;
}
