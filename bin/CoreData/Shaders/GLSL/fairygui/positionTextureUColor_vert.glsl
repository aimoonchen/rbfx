attribute vec4 a_position;
attribute vec2 a_texCoord;

uniform mat4 u_MVPMatrix;

#ifdef GL_ES
varying mediump vec2 v_texCoord;
#else
varying vec2 v_texCoord;
#endif

void main()
{
    gl_Position = u_MVPMatrix * a_position;
    v_texCoord = a_texCoord;
}
// cbuffer VSConstants {
//     float4x4 u_MVPMatrix;
// };
// struct VSInput {
//     float3 a_position : ATTRIB0;
//     float2 a_texCoord  : ATTRIB1;
// };
// struct PSInput {
//     float4 Pos          : SV_POSITION;
//     float2 v_texCoord   : TEX_COORD;
// };
// void main(in VSInput VSIn, out PSInput PSIn)
// {
//     PSIn.Pos        = mul(u_MVPMatrix, float4(VSIn.a_position, 1.0));
//     PSIn.v_texCoord = VSIn.a_texCoord;
// }
