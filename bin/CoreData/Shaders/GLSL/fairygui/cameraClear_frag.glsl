#ifdef GL_ES
varying mediump vec2 v_texCoord;
varying mediump vec4 v_color;
#else
varying vec2 v_texCoord;
varying vec4 v_color;
#endif

void main()
{
    gl_FragColor = v_color;
}
// struct PSInput {
//     float4 Pos          : SV_POSITION;
//     float2 v_texCoord   : TEX_COORD;
//     float4 v_color      : COLOR0;
// };
// struct PSOutput {
//     float4 Color : SV_TARGET;
// };
// void main(in PSInput PSIn, out PSOutput PSOut)
// {
//     PSOut.Color = PSIn.v_color;
// }