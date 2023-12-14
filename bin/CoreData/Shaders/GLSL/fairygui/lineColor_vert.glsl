attribute vec3 a_position;
attribute vec4 a_color;

uniform mat4 u_MVPMatrix;

#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
#else
varying vec4 v_fragmentColor;
#endif

void main()
{
    gl_Position = u_MVPMatrix *  vec4(a_position, 1.0);
    v_fragmentColor = a_color;
}
// cbuffer VSConstants {
//     float4x4 u_MVPMatrix;
// };
// struct VSInput {
//     float3 a_position   : ATTRIB0;
//     float4 a_color      : ATTRIB1;
// };
// struct PSInput {
//     float4 Pos              : SV_POSITION;
//     float4 v_fragmentColor  : COLOR0;
// };
// void main(in VSInput VSIn, out PSInput PSIn)
// {
//     PSIn.Pos                = mul(u_MVPMatrix, float4(VSIn.a_position, 1.0));
//     PSIn.v_fragmentColor    = VSIn.a_color;
// }

