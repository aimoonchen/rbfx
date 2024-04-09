#include "_Config.glsl"
UNIFORM_BUFFER_BEGIN(1, Camera)
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
#define sizeAttenuation cResolution.w
#define color cColor.xyz
#define opacity cColor.a

vec2 fix(vec4 i, float aspect) {
    vec2 res = i.xy / i.w;
    res.x *= aspect;
    return res;
}

void main() {
    float aspect = resolution.x / resolution.y;
    vColor = vec4(color, opacity);
    vUV = uv;
    vCounters = counters;

    // mat4 m = projectionMatrix * modelViewMatrix;
    mat4 m = cModelMat * cViewProjection;
    vec4 finalPosition = vec4(position, 1.0) * m * aspect;
    vec4 prevPos = vec4(previous, 1.0) * m;
    vec4 nextPos = vec4(next, 1.0) * m;

    vec2 currentP = fix(finalPosition, aspect);
    vec2 prevP = fix(prevPos, aspect);
    vec2 nextP = fix(nextPos, aspect);

    float w = lineWidth * width;

    vec2 dir1 = normalize(currentP - prevP);
    vec2 dir2 = normalize(nextP - currentP);
    vec2 dir = normalize(dir1 + dir2);

    vec2 perp = vec2(-dir1.y, dir1.x);
    vec2 miter = vec2(-dir.y, dir.x);
    //w = clamp(w / dot(miter, perp), 0., 4. * lineWidth * width);

    //vec2 normal = (cross(vec3(dir, 0.), vec3(0., 0., 1.))).xy;
    vec4 normal = vec4(-dir.y, dir.x, 0., 1.);
    normal.xy *= .5 * w;
    //normal *= projectionMatrix;
    if (sizeAttenuation == 0.) {
        normal.xy *= finalPosition.w;
        normal.xy /= (vec4(resolution, 0., 1.) * projectionMatrix).xy * aspect;
    }

    finalPosition.xy += normal.xy * side;
    gl_Position = finalPosition;
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