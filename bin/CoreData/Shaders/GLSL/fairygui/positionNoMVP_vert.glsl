attribute vec4 a_position;

#ifdef GL_ES
varying lowp vec4 v_position;
#else
varying vec4 v_position;
#endif

void main()
{
    gl_Position = a_position;
    v_position = a_position;
}
// struct VSInput {
//     float3 a_position   : ATTRIB0;
// };
// struct PSInput {
//     float4 Pos          : SV_POSITION;
//     float4 v_position   : TEX_COORD;
// };
// void main(in VSInput VSIn, out PSInput PSIn)
// {
//     PSIn.Pos        = VSIn.a_position;
//     PSIn.v_position = VSIn.a_position;
// }
