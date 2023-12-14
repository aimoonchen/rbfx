#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform float u_alpha_value;
uniform sampler2D u_texture;

void main()
{
    vec4 texColor = texture2D(u_texture, v_texCoord);

// mimic: glAlphaFunc(GL_GREATER)
// pass if ( incoming_pixel >= u_alpha_value ) => fail if incoming_pixel < u_alpha_value

    if ( texColor.a <= u_alpha_value )
        discard;

    gl_FragColor = texColor * v_fragmentColor;
}
// cbuffer PSConstants {
//     float u_alpha_value;
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
//     float4 Color = u_texture.Sample(u_texture_sampler, PSIn.v_texCoord);
//     if (Color.a <= u_alpha_value)
//         discard;
//     PSOut.Color = PSIn.v_fragmentColor * Color;
// }
