#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform sampler2D u_texture;

void main(void)
{
    vec4 c = texture2D(u_texture, v_texCoord);
     c = v_fragmentColor * c;
    gl_FragColor.xyz = vec3(0.2126*c.r + 0.7152*c.g + 0.0722*c.b);
    gl_FragColor.w = c.w;
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
//     float4 Color = PSIn.v_fragmentColor * u_texture.Sample(u_texture_sampler, PSIn.v_texCoord);
//     float gray = 0.2126*Color.r + 0.7152*Color.g + 0.0722*Color.b;
//     PSOut.Color = float4(gray, gray, gray, Color.a);
// }
