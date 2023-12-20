#include "../_Config.glsl"
/*
* Copyright 2017 Stanislav Pidhorskyi. All rights reserved.
* License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
*/

VERTEX_OUTPUT(vec2 vScreenPos)
VERTEX_OUTPUT(vec3 vSkyColor)
VERTEX_OUTPUT(vec3 vViewDir)

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

void main()
{
	float size2 = cParameters.x * cParameters.x;

	vec3 lightDir = normalize(cSunDirection.xyz);
	float dist = 2.0 * (1.0 - dot(normalize(vViewDir), lightDir));
	float sun  = exp(-dist/ cParameters.y / size2) + step(dist, size2);
	float sun2 = min(sun * sun, 1.0);
	vec3 color = vSkyColor + sun2;
	color = toGamma(color);

	gl_FragColor = vec4(color, 1.0);
}
