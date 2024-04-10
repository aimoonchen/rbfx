#include "_Config.glsl"
UNIFORM_BUFFER_BEGIN(1, Camera)
    UNIFORM_HIGHP(mat4 cViewMat)
    UNIFORM_HIGHP(mat4 cViewProjection)
    UNIFORM_HIGHP(mat4 cProjection)
UNIFORM_BUFFER_END(1, Camera)

UNIFORM_BUFFER_BEGIN(5, Object)
    UNIFORM_HIGHP(mat4 cModelMat)
    UNIFORM_HIGHP(vec4 cResolution)
    UNIFORM_HIGHP(vec4 cColor)
UNIFORM_BUFFER_END(5, Object)

VERTEX_OUTPUT_HIGHP(vec2 vUV)
VERTEX_OUTPUT_HIGHP(vec4 vColor)
VERTEX_OUTPUT_HIGHP(float vCounters)

#ifdef URHO3D_VERTEX_SHADER
VERTEX_INPUT(vec3 iPos)
VERTEX_INPUT(vec3 iNormal)
VERTEX_INPUT(vec4 iBinormal)
VERTEX_INPUT(vec4 iTangent)
VERTEX_INPUT(vec2 iTexCoord)
#define position iPos
#define previous iNormal
#define next iBinormal.xyz
#define side iTangent.x
#define width iTangent.y
#define counters iTangent.z
#define alphaFade iTangent.w
#define uv iTexCoord

#define projectionMatrix cProjection
#define resolution cResolution.xy
#define lineWidth cResolution.z
#define sizeInWorld cResolution.w
#define color cColor.rgb
#define opacity cColor.a

// From the clip space to the screen space
vec2 ClipToScreen(vec4 P)
{
    vec2 p = 0.5 + (P.xyz/P.w).xy * 0.5;
    return p * cResolution.xy;
}
// From the screen space to the clip space
vec4 ScreenToClip(vec2 p, float z, float w)
{
    vec4 P = vec4( w*((p/cResolution.xy)*2.0 - 1.0), z, w);
    return P;
}
// Estimate the linewidth
// WARNING: wrong if pos == screen_pos
float EstimateWidth(vec3 pos, vec2 screen_pos, float w)
{
    vec4 view_pos = vec4(pos, 1.0) * cModelMat * cViewMat;
    vec4 scale_pos = view_pos - vec4(normalize(view_pos.xy) * w, 0.0, 1.0);
    vec2 screen_scale_pos = ClipToScreen(scale_pos * cProjection);
    return distance(screen_pos, screen_scale_pos);
}
void main() {
    vColor = vec4(color, opacity);
    vUV = uv;
    vCounters = counters;
    mat4 mvp = cModelMat * cViewProjection;
    vec2 prevS = ClipToScreen(vec4(previous, 1.0) * mvp);
    vec2 nextS = ClipToScreen(vec4(next, 1.0) * mvp);
    vec4 currC = vec4(position, 1.0) * mvp;
    vec2 currS = ClipToScreen(currC);
    vec2 tangent1 = normalize(prevS - currS);
    vec2 tangent2 = normalize(currS - nextS);
    vec2 tangent = normalize(tangent1 + tangent2);
    vec2 ortho = vec2(-tangent.y, tangent.x) * side;
    float w = lineWidth * width;
    if (sizeInWorld == 0.) {
        w *= 0.5;
    } else {
        w = EstimateWidth(position, currS, w) * 0.5;
    }
    vec2 spos = currS + ortho * w;
    gl_Position = ScreenToClip(spos, currC.z, currC.w);
}
#endif

#ifdef URHO3D_PIXEL_SHADER
SAMPLER(0, sampler2D sBaseMap)
SAMPLER(1, sampler2D sAlphaMap)
UNIFORM_BUFFER_BEGIN(6, Custom)
    UNIFORM(vec4 cLineDesc0)
    UNIFORM(vec4 cLineDesc1)
    UNIFORM(vec4 cRepeat)
    UNIFORM(vec3 cGradient1)
    UNIFORM(vec3 cGradient2)
UNIFORM_BUFFER_END(6, Custom)

#define useMap cLineDesc0.x
#define useAlphaMap cLineDesc0.y
#define visibility cLineDesc0.z
#define alphaTest cLineDesc0.w
#define useDash cLineDesc1.x
#define dashArray cLineDesc1.y
#define dashOffset cLineDesc1.z
#define dashRatio cLineDesc1.w
#define repeat cRepeat.xy
#define alphaFade cRepeat.z
#define useGradient cRepeat.w

void main() {
    vec4 diffuseColor = vColor;
    //
    if (alphaFade > 0.5) diffuseColor.a *= (1.0 - vCounters);
    else if (alphaFade < -0.5) diffuseColor.a *= vCounters;
    else if (vCounters < alphaFade) diffuseColor.a *= vCounters / alphaFade;
    else if (vCounters > (1.0 - alphaFade)) diffuseColor.a *= (1.0 - vCounters) / alphaFade;
    //
    if (useGradient == 1.) diffuseColor = vec4(mix(cGradient1, cGradient2, vCounters), 1.0);
    if (useMap == 1.) diffuseColor *= texture2D(sBaseMap, vUV * repeat);
    if (useAlphaMap == 1.) diffuseColor.a *= texture2D(sAlphaMap, vUV * repeat).a;
    if (diffuseColor.a < alphaTest) discard;
    if (useDash == 1.) {
        diffuseColor.a *= ceil(mod(vCounters + dashOffset, dashArray) - (dashArray * dashRatio));
    }
    diffuseColor.a *= step(vCounters, visibility);
    gl_FragColor = diffuseColor;
}
#endif