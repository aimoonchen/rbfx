attribute vec4 a_position;

#ifdef GL_ES
varying lowp vec4 v_position;
#else
varying vec4 v_position;
#endif

uniform mat4 u_MVPMatrix;

void main()
{
    gl_Position = u_MVPMatrix * a_position;
    v_position = a_position;
}
// cbuffer VSConstants {
//     float4x4 u_MVPMatrix;
// };
// struct VSInput {
//     float3 a_position   : ATTRIB0;
// };
// struct PSInput {
//     float4 Pos          : SV_POSITION;
//     float4 v_position   : TEX_COORD;
// };
// void main(in VSInput VSIn, out PSInput PSIn)
// {
//     PSIn.Pos        = mul(u_MVPMatrix, float4(VSIn.a_position, 1.0));
//     PSIn.v_position = float4(VSIn.a_position, 1.0);
// }
