#pragma once

#include <stdint.h>
#include <Effekseer.h>
// #include <String.hpp>
// #include <RID.hpp>
// #include <Vector2.hpp>
// #include <Vector3.hpp>
// #include <Transform.hpp>
// #include <Color.hpp>
// #include <Script.hpp>
#include <EASTL/string.h>
#include "../../Math/Vector2.h"
#include "../../Math/Vector3.h"
#include "../../Math/Matrix3x4.h"
#include "../../Math/Matrix4.h"
#include "../../Math/Color.h"

namespace EffekseerUrho3D
{

// inline int64_t RIDToInt64(godot::RID rid)
// {
// 	int64_t val;
// 	memcpy(&val, &rid, sizeof(rid));
// 	return val;
// }
// 
// inline godot::RID Int64ToRID(int64_t val)
// {
// 	godot::RID rid;
// 	memcpy(&rid, &val, sizeof(rid));
// 	return rid;
// }

inline Effekseer::Vector2D ToEfkVector2(Urho3D::Vector2 v)
{
	return { v.x_, v.y_ };
}

inline Urho3D::Vector2 ToGdVector2(Effekseer::Vector2D v)
{
	return { v.X, v.Y };
}

inline Effekseer::Vector3D ToEfkVector3(Urho3D::Vector3 v)
{
	return { v.x_, v.y_, v.z_ };
}

inline Urho3D::Vector3 ToGdVector3(Effekseer::Vector3D v)
{
	return { v.X, v.Y, v.Z };
}

// struct SRT2D {
// 	Urho3D::Vector2 scale;
// 	real_t rotation;
// 	Urho3D::Vector2 translation;
// };
// inline SRT2D ToSRT(const godot::Transform2D& transform)
// {
// 	SRT2D srt;
// 
// 	srt.rotation = atan2(transform.elements[0].y, transform.elements[0].x);
// 
// 	real_t cr = cos(-srt.rotation);
// 	real_t sr = sin(-srt.rotation);
// 	srt.scale.x = cr * transform.elements[0].x - sr * transform.elements[0].y;
// 	srt.scale.y = sr * transform.elements[1].x + cr * transform.elements[1].y;
// 
// 	srt.translation.x = transform.elements[2].x;
// 	srt.translation.y = transform.elements[2].y;
// 
// 	return srt;
// }

inline Effekseer::Matrix44 ToEfkMatrix44(const Urho3D::Matrix4& transform)
{
	Effekseer::Matrix44 matrix;
	auto mat = transform.Transpose();
	memcpy(matrix.Values, &mat.m00_, 16 * sizeof(float));
// 	matrix.Values[0][0] = transform.basis[0][0];
// 	matrix.Values[0][1] = transform.basis[0][1];
// 	matrix.Values[0][2] = transform.basis[0][2];
// 	matrix.Values[0][3] = 0.0f;
// 	matrix.Values[1][0] = transform.basis[1][0];
// 	matrix.Values[1][1] = transform.basis[1][1];
// 	matrix.Values[1][2] = transform.basis[1][2];
// 	matrix.Values[1][3] = 0.0f;
// 	matrix.Values[2][0] = transform.basis[2][0];
// 	matrix.Values[2][1] = transform.basis[2][1];
// 	matrix.Values[2][2] = transform.basis[2][2];
// 	matrix.Values[2][3] = 0.0f;
// 	matrix.Values[3][0] = transform.origin.x;
// 	matrix.Values[3][1] = transform.origin.y;
// 	matrix.Values[3][2] = transform.origin.z;
// 	matrix.Values[3][3] = 1.0f;
	return matrix;
}

// inline Effekseer::Matrix44 ToEfkMatrix44(const godot::Transform2D& transform)
// {
// 	Effekseer::Matrix44 matrix;
// 	matrix.Values[0][0] = transform.elements[0].x;
// 	matrix.Values[0][1] = transform.elements[0].y;
// 	matrix.Values[0][2] = 0.0f;
// 	matrix.Values[0][3] = 0.0f;
// 	matrix.Values[1][0] = transform.elements[1].x;
// 	matrix.Values[1][1] = transform.elements[1].y;
// 	matrix.Values[1][2] = 0.0f;
// 	matrix.Values[1][3] = 0.0f;
// 	matrix.Values[2][0] = 0.0f;
// 	matrix.Values[2][1] = 0.0f;
// 	matrix.Values[2][2] = 1.0f;
// 	matrix.Values[2][3] = 0.0f;
// 	matrix.Values[3][0] = transform.elements[2].x;
// 	matrix.Values[3][1] = transform.elements[2].y;
// 	matrix.Values[3][2] = 0.0f;
// 	matrix.Values[3][3] = 1.0f;
// 	return matrix;
// }
inline Effekseer::Matrix43 ToEfkMatrix43(const Urho3D::Matrix3x4& transform)
{
	Effekseer::Matrix43 matrix;
	matrix.Value[0][0] = transform.m00_;
	matrix.Value[0][1] = transform.m10_;
	matrix.Value[0][2] = transform.m20_;
	matrix.Value[1][0] = transform.m01_;
	matrix.Value[1][1] = transform.m11_;
	matrix.Value[1][2] = transform.m21_;
	matrix.Value[2][0] = transform.m02_;
	matrix.Value[2][1] = transform.m12_;
	matrix.Value[2][2] = transform.m22_;
	matrix.Value[3][0] = transform.m03_;
	matrix.Value[3][1] = transform.m13_;
	matrix.Value[3][2] = transform.m23_;
	return matrix;
}

// inline Effekseer::Matrix43 ToEfkMatrix43(const Urho3D::Matrix4& transform)
// {
// 	Effekseer::Matrix43 matrix;
// 	matrix.Value[0][0] = transform.basis[0][0];
// 	matrix.Value[0][1] = transform.basis[0][1];
// 	matrix.Value[0][2] = transform.basis[0][2];
// 	matrix.Value[1][0] = transform.basis[1][0];
// 	matrix.Value[1][1] = transform.basis[1][1];
// 	matrix.Value[1][2] = transform.basis[1][2];
// 	matrix.Value[2][0] = transform.basis[2][0];
// 	matrix.Value[2][1] = transform.basis[2][1];
// 	matrix.Value[2][2] = transform.basis[2][2];
// 	matrix.Value[3][0] = transform.origin.x;
// 	matrix.Value[3][1] = transform.origin.y;
// 	matrix.Value[3][2] = transform.origin.z;
// 	return matrix;
// }

// inline Effekseer::Matrix43 ToEfkMatrix43(const godot::Transform2D& transform, 
// 	const Urho3D::Vector3& orientation, bool flipH, bool flipV)
// {
// 	using namespace Effekseer::SIMD;
// 
// 	auto srt = ToSRT(transform);
// 
// 	// Invert XY by flip or negative scale
// 	float scaleX = (flipH ^ (srt.scale.x < 0.0f)) ? -1.0f : 1.0f;
// 	float scaleY = (flipV ^ (srt.scale.y < 0.0f)) ? -1.0f : 1.0f;
// 	
// 	// Invalidate scale (Apply scale at rendering)
// 	float translationX = srt.translation.x / abs(srt.scale.x);
// 	float translationY = srt.translation.y / abs(srt.scale.y);
// 
// 	Mat43f transformMatrix = Mat43f::SRT({scaleX, scaleY, 1.0f},
// 		Mat43f::RotationZ(srt.rotation), 
// 		{translationX, translationY, 0.0f});
// 
// 	Mat43f orientationMatrix = Mat43f::RotationZXY(orientation.z, orientation.x, orientation.y);
// 	
// 	// Multiply and Convert
// 	return ToStruct(orientationMatrix * transformMatrix);
// }

inline Urho3D::Matrix4 ToGdMatrix(Effekseer::Matrix44 matrix)
{
	Urho3D::Matrix4 transform(&matrix.Values[0][0]);
// 	transform.basis[0][0] = matrix.Values[0][0];
// 	transform.basis[1][0] = matrix.Values[1][0];
// 	transform.basis[2][0] = matrix.Values[2][0];
// 	transform.basis[0][1] = matrix.Values[0][1];
// 	transform.basis[1][1] = matrix.Values[1][1];
// 	transform.basis[2][1] = matrix.Values[2][1];
// 	transform.basis[0][2] = matrix.Values[0][2];
// 	transform.basis[1][2] = matrix.Values[1][2];
// 	transform.basis[2][2] = matrix.Values[2][2];
// 	transform.origin.x = matrix.Values[3][0];
// 	transform.origin.y = matrix.Values[3][1];
// 	transform.origin.z = matrix.Values[3][2];
	return transform;
}

inline Effekseer::Color ToEfkColor(Urho3D::Color c)
{
	return {
		(uint8_t)Effekseer::Clamp((int)(c.r_ * 255.0f), 255, 0),
		(uint8_t)Effekseer::Clamp((int)(c.g_ * 255.0f), 255, 0),
		(uint8_t)Effekseer::Clamp((int)(c.b_ * 255.0f), 255, 0),
		(uint8_t)Effekseer::Clamp((int)(c.a_ * 255.0f), 255, 0),
	};
}

inline Urho3D::Color ToGdColor(Effekseer::Color c)
{
	return { c.R / 255.0f, c.G / 255.0f, c.B / 255.0f, c.A / 255.0f };
}

size_t ToEfkString(char16_t* to, const ea::string& from, size_t size);

ea::string ToGdString(const char16_t* from);

//godot::Variant ScriptNew(godot::Ref<godot::Script> script);

}
