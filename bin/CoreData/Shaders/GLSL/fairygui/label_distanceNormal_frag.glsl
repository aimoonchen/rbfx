#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform vec4 u_textColor;
uniform sampler2D u_texture;

void main()
{
    vec4 color = texture2D(u_texture, v_texCoord);
    //the texture use dual channel 16-bit output for distance_map
    //float dist = color.b+color.g/256.0;
    // the texture use single channel 8-bit output for distance_map
    float dist = color.a;
    //TODO: Implementation 'fwidth' for glsl 1.0
    //float width = fwidth(dist);
    //assign width for constant will lead to a little bit fuzzy,it's temporary measure.
    float width = 0.04;
    float alpha = smoothstep(0.5-width, 0.5+width, dist) * u_textColor.a;
    gl_FragColor = v_fragmentColor * vec4(u_textColor.rgb,alpha);
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
//     float dist = u_texture.Sample(u_texture_sampler, PSIn.v_texCoord).a;
//     float width = 0.04;
//     float alpha = smoothstep(0.5-width, 0.5+width, dist) * u_textColor.a;
//     PSOut.Color = PSIn.v_fragmentColor * float4(u_textColor.rgb, alpha);
// }
