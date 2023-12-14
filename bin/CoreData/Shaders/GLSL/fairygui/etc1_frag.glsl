#ifdef GL_ES
    precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform sampler2D u_texture;
uniform sampler2D u_texture1;

void main() {
    vec4 texColor = vec4(texture2D(u_texture, v_texCoord).rgb, texture2D(u_texture1, v_texCoord).r);

    texColor.rgb *= texColor.a; // Premultiply with Alpha channel

    gl_FragColor = v_fragmentColor * texColor;
}
// Texture2D    u_texture;
// SamplerState u_texture_sampler;
// Texture2D    u_texture1;
// SamplerState u_texture1_sampler;
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
//     float4 Color = float4(u_texture.Sample(u_texture_sampler, PSIn.v_texCoord).rgb, u_texture1.Sample(u_texture1_sampler, PSIn.v_texCoord).r);
//     Color.rgb *= Color.a;
//     PSOut.Color = PSIn.v_fragmentColor * Color;
// }
