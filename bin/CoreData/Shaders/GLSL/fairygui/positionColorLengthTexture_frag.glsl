#ifdef GL_ES
// #extension GL_OES_standard_derivatives : enable

varying mediump vec4 v_color;
varying mediump vec2 v_texcoord;
#else
varying vec4 v_color;
varying vec2 v_texcoord;
#endif

void main()
{
// #if defined GL_OES_standard_derivatives
// gl_FragColor = v_color*smoothstep(0.0, length(fwidth(v_texcoord)), 1.0 - length(v_texcoord));
// #else
    gl_FragColor = v_color*step(0.0, 1.0 - length(v_texcoord));
// #endif
}
// struct PSInput {
//     float4 Pos          : SV_POSITION;
//     float2 v_texcoord   : TEX_COORD;
//     float4 v_color      : COLOR0;
// };
// struct PSOutput {
//     float4 Color : SV_TARGET;
// };
// void main(in PSInput PSIn, out PSOutput PSOut)
// {
//     PSOut.Color = PSIn.v_color * step(0.0, 1.0 - length(PSIn.v_texcoord));
// }
