uniform float depth;
attribute vec4 a_position;
attribute vec4 a_color;
attribute vec2 a_texCoord;

#ifdef GL_ES
varying mediump vec2 v_texCoord;
varying mediump vec4 v_color;
#else
varying vec2 v_texCoord;
varying vec4 v_color;
#endif

void main()
{
    gl_Position = a_position;
    gl_Position.z = depth;
    gl_Position.w = 1.0;
    v_texCoord = a_texCoord;
    v_color = a_color;
}
// cbuffer VSConstants {
//     float depth;
// };
// struct VSInput {
//     float3 a_position   : ATTRIB0;
//     float4 a_color      : ATTRIB1;
//     float2 a_texCoord   : ATTRIB2;
// };
// struct PSInput {
//     float4 Pos          : SV_POSITION;
//     float2 v_texCoord   : TEX_COORD;
//     float4 v_color      : COLOR0;
// };
// void main(in VSInput VSIn, out PSInput PSIn) 
// {
//     PSIn.Pos        = float4(VSIn.a_position.xy, depth, 1.0);
//     PSIn.v_texCoord = VSIn.a_texCoord;
//     PSIn.v_color    = VSIn.a_color;
// }