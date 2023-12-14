#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;

void main()
{
    gl_FragColor = v_fragmentColor;
}
// struct PSInput {
//     float4 Pos              : SV_POSITION;
//     float4 v_fragmentColor  : COLOR0;
// };
// struct PSOutput {
//     float4 Color : SV_TARGET;
// };
// void main(in PSInput PSIn, out PSOutput PSOut)
// {
//     PSOut.Color = PSIn.v_fragmentColor;
// }
