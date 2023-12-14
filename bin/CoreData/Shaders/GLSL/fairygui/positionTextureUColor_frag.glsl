#ifdef GL_ES
precision lowp float;
#endif

uniform vec4 u_color;
uniform sampler2D u_texture;

varying vec2 v_texCoord;

void main()
{
    gl_FragColor =  texture2D(u_texture, v_texCoord) * u_color;
}
// cbuffer PSConstants {
//     float4 u_color;
// };
// Texture2D    u_texture;
// SamplerState u_texture_sampler;
// struct PSInput {
//     float4 Pos          : SV_POSITION;
//     float2 v_texCoord   : TEX_COORD;
// };
// struct PSOutput {
//     float4 Color : SV_TARGET;
// };
// void main(in PSInput PSIn, out PSOutput PSOut)
// {
//     PSOut.Color = u_color * u_texture.Sample(u_texture_sampler, PSIn.v_texCoord);
// }
