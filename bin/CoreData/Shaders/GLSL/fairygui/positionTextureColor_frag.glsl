#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform sampler2D u_texture;

void main()
{
    gl_FragColor = v_fragmentColor * texture2D(u_texture, v_texCoord);
}
// Texture2D    u_texture;
// SamplerState u_texture_sampler;
// struct PSInput {
//     float4 Pos              : SV_POSITION;
//     float2 v_texCoord       : TEX_COORD;
//     float4 v_fragmentColor  : COLOR0;
// };
// struct PSOutput {
//     float4 Color : SV_TARGET;
// };
// void main(in PSInput PSIn, out PSOutput PSOut)
// {
//     PSOut.Color = PSIn.v_fragmentColor * u_texture.Sample(u_texture_sampler, PSIn.v_texCoord);
// }
