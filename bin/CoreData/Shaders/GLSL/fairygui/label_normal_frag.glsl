#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform vec4 u_textColor;
uniform sampler2D u_texture;

void main()
{
    gl_FragColor =  v_fragmentColor * vec4(u_textColor.rgb,// RGB from uniform
        u_textColor.a * texture2D(u_texture, v_texCoord).a// A from texture & uniform
    );
}
// cbuffer PSConstants {
//     float4 u_textColor;
// };
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
//     PSOut.Color = PSIn.v_fragmentColor * float4(u_textColor.rgb, u_textColor.a * u_texture.Sample(u_texture_sampler, PSIn.v_texCoord).a);
// }
