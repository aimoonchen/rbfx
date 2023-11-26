#include <sol/sol.hpp>
#include "../../Math/Vector2.h"
#include "../../Math/Vector3.h"
#include "../../Math/Vector4.h"
#include "../../Math/Matrix3x4.h"
#include "../../Math/Quaternion.h"
#include "../../Math/Color.h"

using namespace Urho3D;

int sol2_MathLuaAPI_open(sol::state& lua)
{
    auto math3d = lua["math3d"].get_or_create<sol::table>();
    math3d.new_enum("Intersection",
        "OUTSIDE", OUTSIDE,
        "INTERSECTS", INTERSECTS,
        "INSIDE", INSIDE);
    math3d.new_usertype<IntVector2>("IntVector2",
        sol::call_constructor, sol::factories([]() { return IntVector2(); }, [](int x, int y) { return IntVector2(x, y); }),
		"x", &IntVector2::x_,
		"y", &IntVector2::y_,
        "ZERO",                 sol::var(std::ref(IntVector2::ZERO)),
        "LEFT",                 sol::var(std::ref(IntVector2::LEFT)),
        "RIGHT",                sol::var(std::ref(IntVector2::RIGHT)),
        "UP",                   sol::var(std::ref(IntVector2::UP)),
        "DOWN",                 sol::var(std::ref(IntVector2::DOWN)),
        "ONE",                  sol::var(std::ref(IntVector2::ONE)),
        sol::meta_function::addition, &IntVector2::operator+,
        sol::meta_function::subtraction, sol::resolve<IntVector2(const IntVector2&) const>(&IntVector2::operator-),
        sol::meta_function::multiplication, sol::overload(
            sol::resolve<IntVector2(const IntVector2&) const>(&IntVector2::operator*),
            sol::resolve<IntVector2(int) const>(&IntVector2::operator*)),
        sol::meta_function::division, sol::overload(
            sol::resolve<IntVector2(const IntVector2&) const>(&IntVector2::operator/),
            sol::resolve<IntVector2(int) const>(&IntVector2::operator/)));

    math3d.new_usertype<Vector2>("Vector2",
        sol::call_constructor, sol::factories([]() { return Vector2(); }, [](float x, float y) { return Vector2(x, y); }),
		"x", &Vector2::x_,
		"y", &Vector2::y_,
        "ZERO",                 sol::var(std::ref(Vector2::ZERO)),
        "LEFT",                 sol::var(std::ref(Vector2::LEFT)),
        "RIGHT",                sol::var(std::ref(Vector2::RIGHT)),
        "UP",                   sol::var(std::ref(Vector2::UP)),
        "DOWN",                 sol::var(std::ref(Vector2::DOWN)),
        "ONE",                  sol::var(std::ref(Vector2::ONE)),
        sol::meta_function::addition, &Vector2::operator+,
        sol::meta_function::subtraction, sol::resolve<Vector2(const Vector2&) const>(&Vector2::operator-),
        sol::meta_function::multiplication, sol::overload(
            sol::resolve<Vector2(const Vector2&) const>(&Vector2::operator*),
            sol::resolve<Vector2(float) const>(&Vector2::operator*)),
        sol::meta_function::division, sol::overload(
            sol::resolve<Vector2(const Vector2&) const>(&Vector2::operator/),
            sol::resolve<Vector2(float) const>(&Vector2::operator/)));

    math3d.new_usertype<Vector3>("Vector3",
		sol::call_constructor, sol::factories([]() { return Vector3(); }, [](float x, float y) { return Vector3(x, y); }, [](float x, float y, float z) { return Vector3(x, y, z); }),
		"Normalize",			&Vector3::Normalize,
		"Length",				&Vector3::Length,
		"LengthSquared",		&Vector3::LengthSquared,
		"DotProduct",			&Vector3::DotProduct,
		"AbsDotProduct",		&Vector3::AbsDotProduct,
		"ProjectOntoAxis",		&Vector3::ProjectOntoAxis,
		"ProjectOntoPlane",		&Vector3::ProjectOntoPlane,
		"ProjectOntoLine",		&Vector3::ProjectOntoLine,
		"DistanceToPoint",		&Vector3::DistanceToPoint,
		"DistanceToPlane",		&Vector3::DistanceToPlane,
		"Orthogonalize",		&Vector3::Orthogonalize,
		"CrossProduct",			&Vector3::CrossProduct,
		"Abs",					&Vector3::Abs,
		"Lerp",					&Vector3::Lerp,
		"Equals",				&Vector3::Equals,
		"Angle",				&Vector3::Angle,
		"IsNaN",				&Vector3::IsNaN,
		"IsInf",				&Vector3::IsInf,
		"Normalized",			&Vector3::Normalized,
		"NormalizedOrDefault",	&Vector3::NormalizedOrDefault,
		"ReNormalized",			&Vector3::ReNormalized,
		//"ToString",				&Vector3::ToString,
		"ToHash",				&Vector3::ToHash,
		//sol::meta_function::to_string, &Vector3::ToString,
		sol::meta_function::addition, &Vector3::operator+,
		sol::meta_function::subtraction, sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator-),
		sol::meta_function::multiplication, sol::overload(
            sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator*),
            sol::resolve<Vector3(float) const>(&Vector3::operator*)),
        sol::meta_function::division, sol::overload(
            sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator/),
            sol::resolve<Vector3(float) const>(&Vector3::operator/)),
		"x",					&Vector3::x_,
		"y",					&Vector3::y_,
		"z",					&Vector3::z_,
        "ZERO",                 sol::var(std::ref(Vector3::ZERO)),
        "LEFT",                 sol::var(std::ref(Vector3::LEFT)),
        "RIGHT",                sol::var(std::ref(Vector3::RIGHT)),
        "UP",                   sol::var(std::ref(Vector3::UP)),
        "DOWN",                 sol::var(std::ref(Vector3::DOWN)),
        "FORWARD",              sol::var(std::ref(Vector3::FORWARD)),
        "BACK",                 sol::var(std::ref(Vector3::BACK)),
        "ONE",                  sol::var(std::ref(Vector3::ONE)));

    math3d.new_usertype<Vector4>("Vector4",
        sol::call_constructor, sol::factories(
            []() { return Vector4(); },
            [](float x, float y, float z, float w) { return Vector4(x, y, z, w); },
            [](const Vector3& vector, float w) { return Vector4(vector, w); }
            ),
        "x", &Vector4::x_,
        "y", &Vector4::y_,
        "z", &Vector4::z_,
        "w", &Vector4::w_);

    math3d.new_usertype<Quaternion>("Quaternion",
        sol::call_constructor, sol::factories(
            []() { return Quaternion(); },
            [](float w, float x, float y, float z) { return Quaternion(w, x, y, z); },
            [](float angle, const Vector3& axis) { return Quaternion(angle, axis); },
            [](float angle) { return Quaternion(angle); },
            [](float ex, float ey, float ez) { return Quaternion(ex, ey, ez); },
            [](const Vector3& angles) { return Quaternion(angles); },
            [](const Vector3& start, const Vector3& end) { return Quaternion(start, end); },
            [](const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis) { return Quaternion(xAxis, yAxis, zAxis); }
        ),
        "w",            &Quaternion::w_,
        "x",            &Quaternion::x_,
        "y",            &Quaternion::y_,
        "z",            &Quaternion::z_,
        sol::meta_function::multiplication, sol::overload(
            sol::resolve<Quaternion(float) const>(&Quaternion::operator*),
            sol::resolve<Quaternion(const Quaternion&) const>(&Quaternion::operator*),
            sol::resolve<Vector3(const Vector3&) const>(&Quaternion::operator*)),
        "YawAngle",     &Quaternion::YawAngle,
        "PitchAngle",   &Quaternion::PitchAngle,
        "RollAngle",    &Quaternion::RollAngle,
        "EulerAngles",  &Quaternion::EulerAngles,
        "FromRotationTo", &Quaternion::FromRotationTo,
        "FromLookRotation", sol::overload(
            [](Quaternion* self, const Vector3& direction) { return self->FromLookRotation(direction); },
            [](Quaternion* self, const Vector3& direction, const Vector3& up) { return self->FromLookRotation(direction, up); }),
        "Slerp",        &Quaternion::Slerp,
        "Nlerp",        sol::overload(
            [](Quaternion* self, const Quaternion& rhs, float t) { return self->Nlerp(rhs, t); },
            [](Quaternion* self, const Quaternion& rhs, float t, bool shortestPath) { return self->Nlerp(rhs, t, shortestPath); }),
        "IDENTITY",     sol::var(std::ref(Quaternion::IDENTITY)));

	math3d.new_usertype<Color>("Color",
        sol::call_constructor, sol::factories(
            []() { return Color(); },
            [](float r, float g, float b) { return Color(r, g, b); },
            [](float r, float g, float b, float a) { return Color(r, g, b, a); },
            [](uint8_t r, uint8_t g, uint8_t b) { return Color((float)r/255.0f, (float)g/255.0f, (float)b/255.0f, 1.0f); },
			[](uint8_t r, uint8_t g, uint8_t b, uint8_t a) { return Color((float)r/255.0f, (float)g/255.0f, (float)b/255.0f, (float)a/255.0f); }),
		"r", &Color::r_,
		"g", &Color::g_,
		"b", &Color::b_,
		"a", &Color::a_,
        "WHITE",                sol::var(std::ref(Color::WHITE)),
        "GRAY",                 sol::var(std::ref(Color::GRAY)),
        "BLACK",                sol::var(std::ref(Color::BLACK)),
        "RED",                  sol::var(std::ref(Color::RED)),
        "GREEN",                sol::var(std::ref(Color::GREEN)),
        "BLUE",                 sol::var(std::ref(Color::BLUE)),
        "CYAN",                 sol::var(std::ref(Color::CYAN)),
        "MAGENTA",              sol::var(std::ref(Color::MAGENTA)),
        "YELLOW",               sol::var(std::ref(Color::YELLOW)),
        "TRANSPARENT_BLACK",    sol::var(std::ref(Color::TRANSPARENT_BLACK)),
        "LUMINOSITY_GAMMA",     sol::var(std::ref(Color::LUMINOSITY_GAMMA)),
        "LUMINOSITY_LINEAR",    sol::var(std::ref(Color::LUMINOSITY_LINEAR)));

    auto m34 = math3d.new_usertype<Matrix3x4>("Matrix3x4", sol::call_constructor,
        sol::factories([]() { return Matrix3x4(); },
            [](const Vector3& translation, const Quaternion& rotation, float scale) { return Matrix3x4(translation, rotation, scale); },
            [](const Vector3& translation, const Quaternion& rotation, const Vector3& scale) { return Matrix3x4(translation, rotation, scale); }),
        "Inverse", &Matrix3x4::Inverse,
        //"ToString", &Matrix3x4::ToString,
        sol::meta_function::multiplication, sol::overload(
            sol::resolve<Vector3(const Vector3&) const>(&Matrix3x4::operator*),
            sol::resolve<Vector3(const Vector4&) const>(&Matrix3x4::operator*),
            sol::resolve<Matrix3x4(const Matrix3x4&) const>(&Matrix3x4::operator*)),
        "ZERO", sol::var(std::ref(Matrix3x4::ZERO)),
        "IDENTITY", sol::var(std::ref(Matrix3x4::IDENTITY)));

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
