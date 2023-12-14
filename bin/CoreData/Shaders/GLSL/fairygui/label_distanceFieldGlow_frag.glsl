#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform vec4 u_effectColor;
uniform vec4 u_textColor;
uniform sampler2D u_texture;

void main()
{
    float dist = texture2D(u_texture, v_texCoord).a;
    //TODO: Implementation 'fwidth' for glsl 1.0
    //float width = fwidth(dist);
    //assign width for constant will lead to a little bit fuzzy,it's temporary measure.
    float width = 0.04;
    float alpha = smoothstep(0.5-width, 0.5+width, dist);
    //glow
    float mu = smoothstep(0.5, 1.0, sqrt(dist));
    vec4 color = u_effectColor*(1.0-alpha) + u_textColor*alpha;
    gl_FragColor = v_fragmentColor * vec4(color.rgb, max(alpha,mu)*color.a);
}
// cbuffer PSConstants {
//     float4 u_effectColor;
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
//     float dist = u_texture.Sample(u_texture_sampler, PSIn.v_texCoord).a;
//     float width = 0.04;
//     float alpha = smoothstep(0.5-width, 0.5+width, dist);
//     //glow
//     float mu = smoothstep(0.5, 1.0, sqrt(dist));
//     float4 Color = u_effectColor*(1.0-alpha) + u_textColor*alpha;
//     PSOut.Color = PSIn.v_fragmentColor * float4(Color.rgb, max(alpha,mu)*Color.a);
// }
