#include <nanobind/nanobind.h>
#include <nanobind/operators.h> 

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
void init_cmodule_math3d(nb::module_& pm)
{
    auto m = pm.def_submodule("math3d");
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
        .def_ro_static("ONE", &IntVector2::ONE)
        .def("__str__", [](const IntVector2& self) { return self.ToString().c_str(); })
        .def(nb::self + nb::self)
        .def(nb::self - nb::self)
        .def(nb::self * nb::self)
        .def(nb::self * int())
        .def(nb::self / nb::self)
        .def(nb::self / int());

    nb::enum_<Intersection>(m, "Intersection")
        .value("OUTSIDE", Intersection::OUTSIDE)
        .value("INTERSECTS", Intersection::INTERSECTS)
        .value("INSIDE", Intersection::INSIDE);

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
        .def("Equals", [](const Vector2& self, const Vector2& rhs) { return self.Equals(rhs); })
        .def("Equals", [](const Vector2& self, const Vector2& rhs, float eps) { return self.Equals(rhs, eps); })
        .def("Angle", &Vector2::Angle)
        .def("IsNaN", &Vector2::IsNaN)
        .def("IsInf", &Vector2::IsInf)
        .def("Normalized", &Vector2::Normalized)
        .def("NormalizedOrDefault", &Vector2::NormalizedOrDefault)
        .def("ReNormalized", &Vector2::ReNormalized)
        .def("ToHash", &Vector2::ToHash)
        .def("__str__", [](const Vector2& self) { return self.ToString().c_str(); })
        .def(nb::self + nb::self)
        .def(nb::self - nb::self)
        .def(nb::self * float())
        .def(nb::self * nb::self)
        .def(nb::self / float())
        .def(nb::self / nb::self)
        .def(nb::self += nb::self)
        .def(nb::self -= nb::self)
        .def(nb::self *= float())
        .def(nb::self *= nb::self)
        .def(nb::self /= float())
        .def(nb::self /= nb::self)
        .def_ro_static("ZERO", &Vector2::ZERO)
        .def_ro_static("LEFT", &Vector2::LEFT)
        .def_ro_static("RIGHT", &Vector2::RIGHT)
        .def_ro_static("UP", &Vector2::UP)
        .def_ro_static("DOWN", &Vector2::DOWN)
        .def_ro_static("ONE", &Vector2::ONE);

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
        .def("Equals", [](const Vector3& self, const Vector3& rhs) { return self.Equals(rhs); })
        .def("Equals", [](const Vector3& self, const Vector3& rhs, float eps) { return self.Equals(rhs, eps); })
        .def("Angle", &Vector3::Angle)
        .def("IsNaN", &Vector3::IsNaN)
        .def("IsInf", &Vector3::IsInf)
        .def("Normalized", &Vector3::Normalized)
        .def("NormalizedOrDefault", &Vector3::NormalizedOrDefault)
        .def("ReNormalized", &Vector3::ReNormalized)
        .def("ToHash", &Vector3::ToHash)
        .def("__str__", [](const Vector3& self) { return self.ToString().c_str(); })
        .def(nb::self + nb::self)
        .def(nb::self - nb::self)
        .def(nb::self * nb::self)
        .def(nb::self * float())
        .def(nb::self / nb::self)
        .def(nb::self / float())
        .def(nb::self += nb::self)
        .def(nb::self -= nb::self)
        .def(nb::self *= nb::self)
        .def(nb::self *= float())
        .def(nb::self /= nb::self)
        .def(nb::self /= float())
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
        .def_rw("w", &Vector4::w_)
        .def("__str__", [](Vector4* self) { return self->ToString().c_str(); });

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
        .def_ro_static("IDENTITY", &Quaternion::IDENTITY)
        .def("YawAngle", &Quaternion::YawAngle)
        .def("PitchAngle", &Quaternion::PitchAngle)
        .def("RollAngle", &Quaternion::RollAngle)
        .def("EulerAngles", &Quaternion::EulerAngles)
        .def("FromRotationTo", &Quaternion::FromRotationTo)
        .def("FromLookRotation", [](Quaternion* self, const Vector3& direction) { return self->FromLookRotation(direction); })
        .def("FromLookRotation", [](Quaternion* self, const Vector3& direction, const Vector3& up) { return self->FromLookRotation(direction, up); })
        .def("Slerp", &Quaternion::Slerp)
        .def("Nlerp", [](Quaternion* self, const Quaternion& rhs, float t) { return self->Nlerp(rhs, t); })
        .def("Nlerp", [](Quaternion* self, const Quaternion& rhs, float t, bool shortestPath) { return self->Nlerp(rhs, t, shortestPath); })
        .def(nb::self * nb::self)
        .def(nb::self * float())
        .def("__mul__", [](const Quaternion& lhs, const Vector3& rhs) { return lhs * rhs; }, nb::is_operator())
        .def("__str__", [](Quaternion* self) { return self->ToString().c_str(); });

    nb::class_<Color>(m, "Color")
        .def(nb::init<>())
        .def(nb::init<float, float, float>())
        .def(nb::init<float, float, float, float>())
        .def_rw("r", &Color::r_)
        .def_rw("g", &Color::g_)
        .def_rw("b", &Color::b_)
        .def_rw("a", &Color::a_)
        .def_ro_static("WHITE", &Color::WHITE)
        .def_ro_static("GRAY", &Color::GRAY)
        .def_ro_static("BLACK", &Color::BLACK)
        .def_ro_static("RED", &Color::RED)
        .def_ro_static("GREEN", &Color::GREEN)
        .def_ro_static("BLUE", &Color::BLUE)
        .def_ro_static("CYAN", &Color::CYAN)
        .def_ro_static("MAGENTA", &Color::MAGENTA)
        .def_ro_static("YELLOW", &Color::YELLOW)
        .def_ro_static("TRANSPARENT_BLACK", &Color::TRANSPARENT_BLACK)
        .def_ro_static("LUMINOSITY_GAMMA", &Color::LUMINOSITY_GAMMA)
        .def_ro_static("LUMINOSITY_LINEAR", &Color::LUMINOSITY_LINEAR)
        .def("__str__", [](Color* self) { return self->ToString().c_str(); });

    nb::class_<Matrix3x4>(m, "Matrix3x4")
        .def(nb::init<>())
        .def(nb::init<const Vector3&, const Quaternion&, float>())
        .def(nb::init<const Vector3&, const Quaternion&, const Vector3&>())
        .def("Inverse", &Matrix3x4::Inverse)
        .def("__str__", [](Matrix3x4* self) { return self->ToString().c_str(); })
        .def("__mul__", [](const Matrix3x4& self, const Vector3& rhs) { return self * rhs; }, nb::is_operator())
        .def("__mul__", [](const Matrix3x4& self, const Vector4& rhs) { return self * rhs; }, nb::is_operator())
        .def(nb::self * nb::self)
        .def_ro_static("ZERO", &Matrix3x4::ZERO)
        .def_ro_static("IDENTITY", &Matrix3x4::IDENTITY);

    nb::class_<Transform>(m, "Transform")
        .def_rw("position", &Transform::position_)
        .def_rw("rotation", &Transform::rotation_)
        .def_rw("scale", &Transform::scale_)
        .def("FromMatrix3x4", &Transform::FromMatrix3x4)
        .def("ToMatrix3x4", &Transform::ToMatrix3x4)
        .def("Lerp", &Transform::Lerp)
        .def("Inverse", &Transform::Inverse)
        .def("__mul__", [](const Transform& self, const Vector3& rhs) { return self * rhs; }, nb::is_operator())
        .def("__mul__", [](const Transform& self, const Quaternion& rhs) { return self * rhs; }, nb::is_operator())
        .def(nb::self* nb::self);

    nb::class_<BoundingBox>(m, "BoundingBox")
        .def(nb::init<>())
        .def(nb::init<float, float>())
        .def(nb::init<const Vector3&, const Vector3&>())
        .def_rw("min", &BoundingBox::min_)
        .def_rw("max", &BoundingBox::max_)
        .def("Define", [](BoundingBox* self, const Vector3& point) { self->Define(point); })
        .def("Define", nb::overload_cast<const Vector3&, const Vector3&>(&BoundingBox::Define))
        .def("Merge", [](BoundingBox* self, const Vector3& point) { self->Merge(point); })
        .def("Merge", nb::overload_cast<const BoundingBox&>(&BoundingBox::Merge))
        .def("Clip", &BoundingBox::Clip)
        .def("Size", &BoundingBox::Size)
        .def("Center", &BoundingBox::Center)
        .def("IsInside", [](BoundingBox* self, const BoundingBox& box) { return self->IsInside(box); })
        .def("IsInside", [](BoundingBox* self, const Vector3& point) { return self->IsInside(point); })
        .def("__str__", [](const BoundingBox& self) { return self.ToString().c_str(); });

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
        .def("Merge", [](Rect* self, const Vector2& point) { self->Merge(point); })
        .def("Merge", [](Rect* self, const Rect& rect) { self->Merge(rect); })
        .def("IsInside", [](Rect* self, const Vector2& point) { return self->IsInside(point); })
        .def("IsInside", [](Rect* self, const Rect& rect) { return self->IsInside(rect); })
        .def_ro_static("FULL", &Rect::FULL)
        .def_ro_static("ZERO", &Rect::ZERO)
        .def_ro_static("POSITIVE", &Rect::POSITIVE)
        .def("__str__", [](const Rect& self) { return self.ToString().c_str(); });

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
        .def("IsInside", [](const IntRect& self, const IntVector2& point) { return self.IsInside(point); })
        .def("IsInside", [](const IntRect& self, const IntRect& rect) { return self.IsInside(rect); })
        .def("__str__", [](const IntRect& self) { return self.ToString().c_str(); })
        .def_ro_static("ZERO", &IntRect::ZERO);

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
        .def_rw("origin", &Ray::origin_)
        .def_rw("direction", &Ray::direction_)
        .def("HitDistance", nb::overload_cast<const Plane&>(&Ray::HitDistance, nb::const_))
        .def("HitDistance", nb::overload_cast<const BoundingBox&>(&Ray::HitDistance, nb::const_))
        .def("HitDistance", [](const Ray& self, const Vector3& v0, const Vector3& v1, const Vector3& v2) { return self.HitDistance(v0, v1, v2); });

    m.attr("M_LARGE_EPSILON") = M_LARGE_EPSILON;
    m.attr("M_MAX_UNSIGNED") = M_MAX_UNSIGNED;
    m.attr("M_DEGTORAD") = M_DEGTORAD;
    m.attr("M_RADTODEG") = M_RADTODEG;
    m.def("Random", nb::overload_cast<>(&Random));
    m.def("Random", nb::overload_cast<float>(&Random));
    m.def("Random", nb::overload_cast<float, float>(&Random));
    m.def("Random", nb::overload_cast<int>(&Random));
    m.def("Random", nb::overload_cast<int, int>(&Random));
    m.def("RandomNormal", &RandomNormal);
    m.def("ClampF", &Clamp<float>);
    m.def("ClampI", &Clamp<int>);
    m.def("ModF", &Mod<float>);
    m.def("ModI", &Mod<int>);
    m.def("VectorMin", nb::overload_cast<const IntVector2&, const IntVector2&>(&VectorMin));
    m.def("VectorMin", nb::overload_cast<const Vector3&, const Vector3&>(&VectorMin));
    m.def("VectorMax", nb::overload_cast<const IntVector2&, const IntVector2&>(&VectorMax));
    m.def("VectorMax", nb::overload_cast<const Vector3&, const Vector3&>(&VectorMax));
}
