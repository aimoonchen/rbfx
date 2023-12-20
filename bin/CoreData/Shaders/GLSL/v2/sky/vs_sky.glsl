#include "../_Config.glsl"
/*
* Copyright 2017 Stanislav Pidhorskyi. All rights reserved.
* License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
*/

VERTEX_INPUT(vec3 iPos)
VERTEX_OUTPUT(vec2 vScreenPos)
VERTEX_OUTPUT(vec3 vSkyColor)
VERTEX_OUTPUT(vec3 vViewDir)

//UNIFORM(mat4 cInvViewProj)
UNIFORM_BUFFER_BEGIN(1, Camera)
    UNIFORM_HIGHP(mat4 cViewProj)
UNIFORM_BUFFER_END(1, Camera)

UNIFORM_BUFFER_BEGIN(4, Material)
	UNIFORM(vec4 cSunDirection)
	UNIFORM(vec3 cSunLuminance)
	UNIFORM(vec3 cSkyLuminanceXYZ)
	// x - sun size, y - sun bloom, z - exposition
	UNIFORM(vec4 cParameters)
	UNIFORM(vec4 cPerezCoeff0)
	UNIFORM(vec4 cPerezCoeff1)
	UNIFORM(vec4 cPerezCoeff2)
	UNIFORM(vec4 cPerezCoeff3)
	UNIFORM(vec4 cPerezCoeff4)
UNIFORM_BUFFER_END(4, Material)

vec3 Perez(vec3 A,vec3 B,vec3 C,vec3 D, vec3 E,float costeta, float cosgamma)
{
	float _1_costeta = 1.0 / costeta;
	float cos2gamma = cosgamma * cosgamma;
	float gamma = acos(cosgamma);
	vec3 f = (vec3(1.0) + A * exp(B * _1_costeta))
		   * (vec3(1.0) + C * exp(D * gamma) + E * cos2gamma);
	return f;
}

vec3 convertXYZ2RGB(vec3 _xyz)
{
	vec3 rgb;
	rgb.x = dot(vec3( 3.2404542, -1.5371385, -0.4985314), _xyz);
	rgb.y = dot(vec3(-0.9692660,  1.8760108,  0.0415560), _xyz);
	rgb.z = dot(vec3( 0.0556434, -0.2040259,  1.0572252), _xyz);
	return rgb;
}

void main()
{
	vScreenPos = iPos.xy;
	mat4 invViewProj = inverse(cViewProj);
	vec4 rayStart = vec4(vec3(iPos.xy, -1.0), 1.0) * invViewProj;
	vec4 rayEnd = vec4(vec3(iPos.xy, 1.0), 1.0) * invViewProj;

	rayStart = rayStart / rayStart.w;
	rayEnd = rayEnd / rayEnd.w;

	vViewDir = normalize(rayEnd.xyz - rayStart.xyz);
	vViewDir.y = abs(vViewDir.y);

	gl_Position = vec4(iPos.xy, cSunDirection.w, 1.0);

	vec3 lightDir = normalize(cSunDirection.xyz);
	vec3 skyDir = vec3(0.0, 1.0, 0.0);

	// Perez coefficients.
	vec3 A = cPerezCoeff0.xyz;
	vec3 B = cPerezCoeff1.xyz;
	vec3 C = cPerezCoeff2.xyz;
	vec3 D = cPerezCoeff3.xyz;
	vec3 E = cPerezCoeff4.xyz;

	float costeta = max(dot(vViewDir, skyDir), 0.001);
	float cosgamma = clamp(dot(vViewDir, lightDir), -0.9999, 0.9999);
	float cosgammas = dot(skyDir, lightDir);

	vec3 P = Perez(A,B,C,D,E, costeta, cosgamma);
	vec3 P0 = Perez(A,B,C,D,E, 1.0, cosgammas);

	vec3 skyColorxyY = vec3(
		  cSkyLuminanceXYZ.x / (cSkyLuminanceXYZ.x+cSkyLuminanceXYZ.y + cSkyLuminanceXYZ.z)
		, cSkyLuminanceXYZ.y / (cSkyLuminanceXYZ.x+cSkyLuminanceXYZ.y + cSkyLuminanceXYZ.z)
		, cSkyLuminanceXYZ.y
		);

	vec3 Yp = skyColorxyY * P / P0;

	vec3 skyColorXYZ = vec3(Yp.x * Yp.z / Yp.y,Yp.z, (1.0 - Yp.x- Yp.y)*Yp.z/Yp.y);

	vSkyColor = convertXYZ2RGB(skyColorXYZ * cParameters.z);
}
