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

// https://www.shadertoy.com/view/4ssXRX
// http://www.loopit.dk/banding_in_games.pdf
// http://www.dspguide.com/ch2/6.htm

//uniformly distributed, normalized rand, [0, 1)
float nrand(in vec2 n)
{
	return fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

float n4rand_ss(in vec2 n)
{
	float nrnd0 = nrand( n + 0.07*fract( cParameters.w ) );
	float nrnd1 = nrand( n + 0.11*fract( cParameters.w + 0.573953 ) );
	return 0.23*sqrt(-log(nrnd0+0.00001))*cos(2.0*3.141592*nrnd1)+0.5;
}

vec3 toGamma(vec3 _rgb)
{
	return pow(abs(_rgb), vec3(1.0/2.2));
}

void main()
{
	float size2 = cParameters.x * cParameters.x;

	vec3 lightDir = normalize(cSunDirection.xyz);
	float distance = 2.0 * (1.0 - dot(normalize(vViewDir), lightDir));
	float sun = exp(-distance/ cParameters.y / size2) + step(distance, size2);
	float sun2 = min(sun * sun, 1.0);
	vec3 color = vSkyColor + sun2;
	color = toGamma(color);
	float r = n4rand_ss(vScreenPos);
	color += vec3(r, r, r) / 40.0;

	gl_FragColor = vec4(color, 1.0);
}
