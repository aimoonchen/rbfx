#ifdef GL_ES
    precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform sampler2D u_texture;
uniform sampler2D u_texture1;

void main()
{
    vec4 texColor = texture2D(u_texture, v_texCoord);
    texColor.a = texture2D(u_texture1, v_texCoord).r;
    texColor.rgb *= texColor.a; // premultiply alpha channel

    texColor = v_fragmentColor * texColor;

    gl_FragColor.rgb = vec3(0.2126*texColor.r + 0.7152*texColor.g + 0.0722*texColor.b);
    gl_FragColor.a = texColor.a;
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
//     Color *= PSIn.v_fragmentColor;
//     float gray = 0.2126*Color.r + 0.7152*Color.g + 0.0722*Color.b;
//     PSOut.Color = float4(gray, gray, gray, Color.a);
// }
