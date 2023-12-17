// #version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 PosVS;
    vec4 Color;
    vec2 UV;
    vec4 PosP;
};

uniform PS_ConstantBuffer
{
    vec4 u_fLightDirection;
    vec4 u_fLightColor;
    vec4 u_fLightAmbient;
    vec4 u_fFlipbookParameter;
    vec4 u_fUVDistortionParameter;
    vec4 u_fBlendTextureParameter;
    vec4 u_fCameraFrontDirection;
    vec4 u_fFalloffParameter;
    vec4 u_fFalloffBeginColor;
    vec4 u_fFalloffEndColor;
    vec4 u_fEmissiveScaling;
    vec4 u_fEdgeColor;
    vec4 u_fEdgeParameter;
    vec4 u_softParticleParam;
    vec4 u_reconstructionParam1;
    vec4 u_reconstructionParam2;
    vec4 u_mUVInversedBack;
    vec4 u_miscFlags;
};

// uniform PS_ConstantBuffer CBPS0;

layout(binding = 0) uniform sampler2D Sampler_sampler_colorTex;
layout(binding = 1) uniform sampler2D Sampler_sampler_depthTex;

centroid in vec4 _VSPS_Color;
centroid in vec2 _VSPS_UV;
in vec4 _VSPS_PosP;
layout(location = 0) out vec4 _entryPointOutput;

vec3 PositivePow(vec3 base, vec3 power)
{
    return pow(max(abs(base), vec3(1.1920928955078125e-07)), power);
}

vec3 LinearToSRGB(vec3 c)
{
    vec3 param = c;
    vec3 param_1 = vec3(0.4166666567325592041015625);
    return max((PositivePow(param, param_1) * 1.05499994754791259765625) - vec3(0.054999999701976776123046875), vec3(0.0));
}

vec4 LinearToSRGB(vec4 c)
{
    vec3 param = c.xyz;
    return vec4(LinearToSRGB(param), c.w);
}

vec4 ConvertFromSRGBTexture(vec4 c, bool isValid)
{
    if (!isValid)
    {
        return c;
    }
    vec4 param = c;
    return LinearToSRGB(param);
}

float SoftParticle(float backgroundZ, float meshZ, vec4 softparticleParam, vec4 reconstruct1, vec4 reconstruct2)
{
    float distanceFar = softparticleParam.x;
    float distanceNear = softparticleParam.y;
    float distanceNearOffset = softparticleParam.z;
    vec2 rescale = reconstruct1.xy;
    vec4 params = reconstruct2;
    vec2 zs = vec2((backgroundZ * rescale.x) + rescale.y, meshZ);
    vec2 depth = ((zs * params.w) - vec2(params.y)) / (vec2(params.x) - (zs * params.z));
    float dir = sign(depth.x);
    depth *= dir;
    float alphaFar = (depth.x - depth.y) / distanceFar;
    float alphaNear = (depth.y - distanceNearOffset) / distanceNear;
    return min(max(min(alphaFar, alphaNear), 0.0), 1.0);
}

vec3 SRGBToLinear(vec3 c)
{
    return min(c, c * ((c * ((c * 0.305306017398834228515625) + vec3(0.6821711063385009765625))) + vec3(0.01252287812530994415283203125)));
}

vec4 SRGBToLinear(vec4 c)
{
    vec3 param = c.xyz;
    return vec4(SRGBToLinear(param), c.w);
}

vec4 ConvertToScreen(vec4 c, bool isValid)
{
    if (!isValid)
    {
        return c;
    }
    vec4 param = c;
    return SRGBToLinear(param);
}

vec4 _main(PS_Input Input)
{
    bool convertColorSpace = u_miscFlags.x != 0.0;
    vec4 param = texture(Sampler_sampler_colorTex, Input.UV);
    bool param_1 = convertColorSpace;
    vec4 Output = ConvertFromSRGBTexture(param, param_1) * Input.Color;
    vec4 _256 = Output;
    vec3 _258 = _256.xyz * u_fEmissiveScaling.x;
    Output.x = _258.x;
    Output.y = _258.y;
    Output.z = _258.z;
    vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = u_mUVInversedBack.x + (u_mUVInversedBack.y * screenUV.y);
    if (u_softParticleParam.w != 0.0)
    {
        float backgroundZ = texture(Sampler_sampler_depthTex, screenUV).x;
        float param_2 = backgroundZ;
        float param_3 = screenPos.z;
        vec4 param_4 = u_softParticleParam;
        vec4 param_5 = u_reconstructionParam1;
        vec4 param_6 = u_reconstructionParam2;
        Output.w *= SoftParticle(param_2, param_3, param_4, param_5, param_6);
    }
    if (Output.w == 0.0)
    {
        discard;
    }
    vec4 param_7 = Output;
    bool param_8 = convertColorSpace;
    return ConvertToScreen(param_7, param_8);
}

void main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = _VSPS_Color;
    Input.UV = _VSPS_UV;
    Input.PosP = _VSPS_PosP;
    vec4 _367 = _main(Input);
    _entryPointOutput = _367;
}

