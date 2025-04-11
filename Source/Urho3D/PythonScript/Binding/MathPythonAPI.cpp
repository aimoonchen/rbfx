#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include "Urho3D/Math/Vector2.h"
#include "Urho3D/Math/Vector3.h"
#include "Urho3D/Math/Vector4.h"
#include "Urho3D/Math/Matrix3x4.h"
#include "Urho3D/Math/Quaternion.h"
#include "Urho3D/Math/Color.h"
#include "Urho3D/Math/BoundingBox.h"
#include "Urho3D/Math/Rect.h"
#include "Urho3D/Math/Plane.h"
#include "Urho3D/Math/Ray.h"
#include "Urho3D/Math/Transform.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;

NB_MODULE(math3d, m)
{
    nb::class_<IntVector2>(m, "IntVector2")
        .def(nb::init<>())
        .def(nb::init<int, int>())
        .def_rw("x", &IntVector2::x_)
        .def_rw("y", &IntVector2::y_)
        .def_ro_static("ZERO", &IntVector2::ZERO)
        .def_ro_static("LEFT", &IntVector2::LEFT)
        .def_ro_static("RIGHT", &IntVector2::RIGHT)
        .def_ro_static("UP", &IntVector2::UP)
        .def_ro_static("DOWN", &IntVector2::DOWN)
        .def_ro_static("ONE", &IntVector2::ONE);
//     bindIntVector2[sol::meta_function::to_string] = [](IntVector2* self) { return self->ToString().c_str(); };
//     bindIntVector2[sol::meta_function::addition] = &IntVector2::operator+;
//     bindIntVector2[sol::meta_function::subtraction] =
//         sol::resolve<IntVector2(const IntVector2&) const>(&IntVector2::operator-);
//     bindIntVector2[sol::meta_function::multiplication] =
//         sol::overload(sol::resolve<IntVector2(const IntVector2&) const>(&IntVector2::operator*),
//             sol::resolve<IntVector2(int) const>(&IntVector2::operator*));
//     bindIntVector2[sol::meta_function::division] =
//         sol::overload(sol::resolve<IntVector2(const IntVector2&) const>(&IntVector2::operator/),
//             sol::resolve<IntVector2(int) const>(&IntVector2::operator/));

//     math3d.new_enum("Intersection",
//         "OUTSIDE", OUTSIDE,
//         "INTERSECTS", INTERSECTS,
//         "INSIDE", INSIDE);
    nb::enum_<Intersection>(m, "Intersection")
        .value("OUTSIDE", Intersection::OUTSIDE)
        .value("INTERSECTS", Intersection::INTERSECTS)
        .value("INSIDE", Intersection::INSIDE)
        .export_values();

    nb::class_<Vector2>(m, "Vector2")
        .def(nb::init<>())
        .def(nb::init<float, float>())
        .def_rw("x", &Vector2::x_)
        .def_rw("y", &Vector2::y_)
        .def("Normalize", &Vector2::Normalize)
        .def("Length", &Vector2::Length)
        .def("LengthSquared", &Vector2::LengthSquared)
        .def("DotProduct", &Vector2::DotProduct)
        .def("AbsDotProduct", &Vector2::AbsDotProduct)
        .def("ProjectOntoAxis", &Vector2::ProjectOntoAxis)
        .def("CrossProduct", &Vector2::CrossProduct)
        .def("Abs", &Vector2::Abs)
        .def("Lerp", &Vector2::Lerp)
        //     bindVector2["Equals"]               = sol::overload(
        //         [](Vector3* self, const Vector3& rhs) { return self->Equals(rhs); },
        //         [](Vector3* self, const Vector3& rhs, float eps) { return self->Equals(rhs); });
        .def("Angle", &Vector2::Angle)
        .def("IsNaN", &Vector2::IsNaN)
        .def("IsInf", &Vector2::IsInf)
        .def("Normalized", &Vector2::Normalized)
        .def("NormalizedOrDefault", &Vector2::NormalizedOrDefault)
        .def("ReNormalized", &Vector2::ReNormalized)
        .def("ToHash", &Vector2::ToHash)
        .def_ro_static("ZERO", &Vector2::ZERO)
        .def_ro_static("LEFT", &Vector2::LEFT)
        .def_ro_static("RIGHT", &Vector2::RIGHT)
        .def_ro_static("UP", &Vector2::UP)
        .def_ro_static("DOWN", &Vector2::DOWN)
        .def_ro_static("ONE", &Vector2::ONE);
//     bindVector2[sol::meta_function::to_string]      = [](Vector2* self) { return self->ToString().c_str(); };
//     bindVector2[sol::meta_function::addition]       = &Vector2::operator+;
//     bindVector2[sol::meta_function::subtraction]    = sol::resolve<Vector2(const Vector2&) const>(&Vector2::operator-);
//     bindVector2[sol::meta_function::multiplication] = sol::overload(
//         sol::resolve<Vector2(const Vector2&) const>(&Vector2::operator*),
//         sol::resolve<Vector2(float) const>(&Vector2::operator*));
//     bindVector2[sol::meta_function::division]       = sol::overload(
//         sol::resolve<Vector2(const Vector2&) const>(&Vector2::operator/),
//         sol::resolve<Vector2(float) const>(&Vector2::operator/));

    nb::class_<Vector3>(m, "Vector3")
        .def(nb::init<>())
        .def(nb::init<float, float>())
        .def(nb::init<float, float, float>())
        .def_rw("x", &Vector3::x_)
        .def_rw("y", &Vector3::y_)
        .def_rw("z", &Vector3::z_)
	    .def("Normalize", &Vector3::Normalize)
	    .def("Length", &Vector3::Length)
	    .def("LengthSquared", &Vector3::LengthSquared)
	    .def("DotProduct", &Vector3::DotProduct)
	    .def("AbsDotProduct", &Vector3::AbsDotProduct)
	    .def("ProjectOntoAxis", &Vector3::ProjectOntoAxis)
	    .def("ProjectOntoPlane", &Vector3::ProjectOntoPlane)
	    .def("ProjectOntoLine", &Vector3::ProjectOntoLine)
	    .def("DistanceToPoint", &Vector3::DistanceToPoint)
	    .def("DistanceToPlane", &Vector3::DistanceToPlane)
	    .def("Orthogonalize", &Vector3::Orthogonalize)
	    .def("CrossProduct", &Vector3::CrossProduct)
	    .def("Abs", &Vector3::Abs)
	    .def("Lerp", &Vector3::Lerp)
//         bindVector3["Equals"]               = sol::overload(
//             [](Vector3* self, const Vector3& rhs) { return self->Equals(rhs); },
//             [](Vector3* self, const Vector3& rhs, float eps) { return self->Equals(rhs); });
	    .def("Angle", &Vector3::Angle)
	    .def("IsNaN", &Vector3::IsNaN)
	    .def("IsInf", &Vector3::IsInf)
	    .def("Normalized", &Vector3::Normalized)
	    .def("NormalizedOrDefault", &Vector3::NormalizedOrDefault)
	    .def("ReNormalized", &Vector3::ReNormalized)
        .def("ToHash", &Vector3::ToHash)
// 	bindVector3[sol::meta_function::to_string]      = [](Vector3* self) { return self->ToString().c_str(); };
// 	bindVector3[sol::meta_function::addition]       = &Vector3::operator+;
// 	bindVector3[sol::meta_function::subtraction]    = sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator-);
// 	bindVector3[sol::meta_function::multiplication] = sol::overload(
//         sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator*),
//         sol::resolve<Vector3(float) const>(&Vector3::operator*));
//     bindVector3[sol::meta_function::division]       = sol::overload(
//         sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator/),
//         sol::resolve<Vector3(float) const>(&Vector3::operator/));
        .def_ro_static("ZERO", &Vector3::ZERO)
        .def_ro_static("LEFT", &Vector3::LEFT)
        .def_ro_static("RIGHT", &Vector3::RIGHT)
        .def_ro_static("UP", &Vector3::UP)
        .def_ro_static("DOWN", &Vector3::DOWN)
        .def_ro_static("FORWARD", &Vector3::FORWARD)
        .def_ro_static("BACK", &Vector3::BACK)
        .def_ro_static("ONE", &Vector3::ONE);

    nb::class_<Vector4>(m, "Vector4")
        .def(nb::init<>())
        .def(nb::init<float, float, float, float>())
        .def(nb::init<const Vector3&, float>())
        .def_rw("x", &Vector4::x_)
        .def_rw("y", &Vector4::y_)
        .def_rw("z", &Vector4::z_)
        .def_rw("w", &Vector4::w_);
    //bindVector4[sol::meta_function::to_string] = [](Vector4* self) { return self->ToString().c_str(); };

    nb::class_<Quaternion>(m, "Quaternion")
        .def(nb::init<>())
        .def(nb::init<float, float, float, float>())
        .def(nb::init<float, const Vector3&>())
        .def(nb::init<float>())
        .def(nb::init<float, float, float>())
        .def(nb::init<const Vector3&>())
        .def(nb::init<const Vector3&, const Vector3&>())
        .def(nb::init<const Vector3&, const Vector3&, const Vector3&>())
        .def_rw("w", &Quaternion::w_)
        .def_rw("x", &Quaternion::x_)
        .def_rw("y", &Quaternion::y_)
        .def_rw("z", &Quaternion::z_)
        .def("YawAngle", &Quaternion::YawAngle)
        .def("PitchAngle", &Quaternion::PitchAngle)
        .def("RollAngle", &Quaternion::RollAngle)
        .def("EulerAngles", &Quaternion::EulerAngles)
        .def("FromRotationTo", &Quaternion::FromRotationTo);
    //     bindQuaternion["FromLookRotation"]  = sol::overload(
//         [](Quaternion* self, const Vector3& direction) { return self->FromLookRotation(direction); },
//         [](Quaternion* self, const Vector3& direction, const Vector3& up) { return self->FromLookRotation(direction, up); });
//     bindQuaternion["Slerp"]             = &Quaternion::Slerp;
//     bindQuaternion["Nlerp"]             = sol::overload(
//         [](Quaternion* self, const Quaternion& rhs, float t) { return self->Nlerp(rhs, t); },
//         [](Quaternion* self, const Quaternion& rhs, float t, bool shortestPath) { return self->Nlerp(rhs, t, shortestPath); });
//     bindQuaternion["IDENTITY"]          = sol::var(std::ref(Quaternion::IDENTITY));
//     bindQuaternion[sol::meta_function::multiplication] = sol::overload(
//         sol::resolve<Quaternion(float) const>(&Quaternion::operator*),
//         sol::resolve<Quaternion(const Quaternion&) const>(&Quaternion::operator*),
//         sol::resolve<Vector3(const Vector3&) const>(&Quaternion::operator*));
//     bindQuaternion[sol::meta_function::to_string] = [](Quaternion* self) { return self->ToString().c_str(); };

    nb::class_<Color>(m, "Color")
        .def(nb::init<>())
        .def(nb::init<float, float, float>())
        .def(nb::init<float, float, float, float>())
        .def(nb::init<uint8_t, uint8_t, uint8_t>())
        .def(nb::init<uint8_t, uint8_t, uint8_t, uint8_t>())
	    .def_rw("r", &Color::r_)
        .def_rw("g", &Color::g_)
        .def_rw("b", &Color::b_)
        .def_rw("a", &Color::a_)
        .def("WHITE", &Color::WHITE)
        .def("GRAY", &Color::GRAY)
        .def("BLACK", &Color::BLACK)
        .def("RED", &Color::RED)
        .def("GREEN", &Color::GREEN)
        .def("BLUE", &Color::BLUE)
        .def("CYAN", &Color::CYAN)
        .def("MAGENTA", &Color::MAGENTA)
        .def("YELLOW", &Color::YELLOW)
        .def("TRANSPARENT_BLACK", &Color::TRANSPARENT_BLACK)
        .def("LUMINOSITY_GAMMA", &Color::LUMINOSITY_GAMMA)
        .def("LUMINOSITY_LINEAR", &Color::LUMINOSITY_LINEAR);
    //bindColor[sol::meta_function::to_string] = [](Color* self) { return self->ToString().c_str(); };

    nb::class_<Matrix3x4>(m, "Matrix3x4")
        .def(nb::init<>())
        .def(nb::init<const Vector3&, const Quaternion&, float>())
        .def(nb::init<const Vector3&, const Quaternion&, const Vector3&>())
        .def("Inverse", &Matrix3x4::Inverse)
//     bindMatrix3x4[sol::meta_function::to_string] = [](Matrix3x4* self) { return self->ToString().c_str(); };
//     bindMatrix3x4[sol::meta_function::multiplication] = sol::overload(
//             sol::resolve<Vector3(const Vector3&) const>(&Matrix3x4::operator*),
//             sol::resolve<Vector3(const Vector4&) const>(&Matrix3x4::operator*),
//             sol::resolve<Matrix3x4(const Matrix3x4&) const>(&Matrix3x4::operator*));
        .def_ro_static("ZERO", &Matrix3x4::ZERO)
        .def_ro_static("IDENTITY", &Matrix3x4::IDENTITY);

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

    nb::class_<BoundingBox>(m, "BoundingBox")
        .def(nb::init<>())
        .def(nb::init<float, float>())
        .def(nb::init<const Vector3&, const Vector3&>())
        .def_rw("min", &BoundingBox::min_)
        .def_rw("max", &BoundingBox::max_)
    //bindBoundingBox["Define"]   = sol::overload([](BoundingBox* self, const Vector3& point) { self->Define(point); }, sol::resolve<void(const Vector3&, const Vector3&)>(&BoundingBox::Define));
    //bindBoundingBox["Merge"]    = sol::overload([](BoundingBox* self, const Vector3& point) { self->Merge(point); }, sol::resolve<void(const BoundingBox&)>(&BoundingBox::Merge));
        .def("Clip", &BoundingBox::Clip)
        .def("Size", &BoundingBox::Size)
        .def("Center", &BoundingBox::Center);
    //     bindBoundingBox["IsInside"] = sol::overload(
//         [](BoundingBox* self, const BoundingBox& box) { return self->IsInside(box); },
//         [](BoundingBox* self, const Vector3& point) { return self->IsInside(point); });
//     bindBoundingBox[sol::meta_function::to_string] = [](BoundingBox* self) { return self->ToString().c_str(); };

    nb::class_<Rect>(m, "Rect")
        .def(nb::init<>())
        .def(nb::init<float, float, float, float>())
        .def(nb::init<const Vector2&, const Vector2&>())
        .def_rw("min", &Rect::min_)
        .def_rw("max", &Rect::max_)
        .def("Size", &Rect::Size)
        .def("Center", &Rect::Center)
        .def("Left", &Rect::Left)
        .def("Top", &Rect::Top)
        .def("Right", &Rect::Right)
        .def("Bottom", &Rect::Bottom)
        .def("Equals", &Rect::Equals)
        .def("Clip", &Rect::Clip)
//     bindRect["Merge"]   = sol::overload(
//         [](Rect* self, const Vector2& point) { self->Merge(point); },
//         [](Rect* self, const Rect& rect) { self->Merge(rect); });
//     bindRect["IsInside"] = sol::overload(
//         [](Rect* self, const Vector2& point) { return self->IsInside(point); },
//         [](Rect* self, const Rect& rect) { return self->IsInside(rect); });
        .def_ro_static("FULL", &Rect::FULL)
        .def_ro_static("ZERO", &Rect::ZERO)
        .def_ro_static("POSITIVE", &Rect::POSITIVE);
    //bindRect[sol::meta_function::to_string] = [](Rect* self) { return self->ToString().c_str(); };

    nb::class_<IntRect>(m, "IntRect")
        .def(nb::init<>())
        .def(nb::init<int, int, int, int>())
        .def(nb::init<const IntVector2&, const IntVector2&>())
        .def("Size", &IntRect::Size)
        .def("Left", &IntRect::Left)
        .def("Top", &IntRect::Top)
        .def("Right", &IntRect::Right)
        .def("Bottom", &IntRect::Bottom)
        .def("Contains", &IntRect::Contains)
        .def("Min", &IntRect::Min)
        .def("Max", &IntRect::Max)
        .def("Clip", &IntRect::Clip)
        .def("Merge", &IntRect::Merge)
//     bindIntRect["IsInside"] = sol::overload(
//         [](IntRect* self, const IntVector2& point) { return self->IsInside(point); },
//         [](IntRect* self, const IntRect& rect) { return self->IsInside(rect); });
        .def_ro_static("ZERO", &IntRect::ZERO);
//    bindIntRect[sol::meta_function::to_string] = [](IntRect* self) { return self->ToString().c_str(); };

    nb::class_<Plane>(m, "Plane")
        .def(nb::init<>())
        .def(nb::init<const Vector3&, const Vector3&, const Vector3&>())
        .def(nb::init<const Vector3&, const Vector3&>())
        .def("Distance", &Plane::Distance)
        .def("Project", &Plane::Project)
        .def("Reflect", &Plane::Reflect)
        .def("ToVector4", &Plane::ToVector4);

    nb::class_<Ray>(m, "Ray")
        .def(nb::init<>())
        .def(nb::init<const Vector3&, const Vector3&>())
//     bindRay["HitDistance"] = sol::overload(
//         sol::resolve<float(const Plane&) const>(&Ray::HitDistance),
//         sol::resolve<float(const BoundingBox&) const>(&Ray::HitDistance),
//         //sol::resolve<float(const Frustum&, bool) const>(&Ray::HitDistance),
//         //sol::resolve<float(const Sphere&) const>(&Ray::HitDistance),
//         [](Ray* self, const Vector3& v0, const Vector3& v1, const Vector3& v2) { return self->HitDistance(v0, v1, v2); });
        .def("origin", &Ray::origin_)
        .def("direction", &Ray::direction_);

	math3d["Random"] = sol::overload(
		sol::resolve<float(void)>(&Random),
        sol::resolve<float(float)>(&Random),
        sol::resolve<float(float, float)>(&Random),
        sol::resolve<int(int)>(&Random),
        sol::resolve<int(int, int)>(&Random));
    m.def("RandomNormal", &RandomNormal);
    m.def("ClampF", &Clamp<float>);
    m.def("ClampI", &Clamp<int>);
    m.def("ModF", &Mod<float>);
    m.def("ModI", &Mod<int>);
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
