#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform vec4 u_effectColor;
uniform vec4 u_textColor;
uniform sampler2D u_texture;

#ifdef GL_ES
uniform lowp int u_effectType; // 0: None (Draw text), 1: Outline, 2: Shadow
#else
uniform int u_effectType;
#endif

void main()
{
    vec4 texColor = texture2D(u_texture, v_texCoord);
    // fontAlpha == 1 means the area of solid text (without edge)
    // fontAlpha == 0 means the area outside text, including outline area
    // fontAlpha == (0, 1) means the edge of text
    float fontAlpha = texColor.a;

    // outlineAlpha == 1 means the area of 'solid text' and 'solid outline'
    // outlineAlpha == 0 means the transparent area outside text and outline
    // outlineAlpha == (0, 1) means the edge of outline
    float outlineAlpha = texColor.r;

    if (u_effectType == 0) // draw text
    {
        gl_FragColor = v_fragmentColor * vec4(u_textColor.rgb, u_textColor.a * fontAlpha);
    }
    else if (u_effectType == 1) // draw outline
    {
        // multipy (1.0 - fontAlpha) to make the inner edge of outline smoother and make the text itself transparent.
        gl_FragColor = v_fragmentColor * vec4(u_effectColor.rgb, u_effectColor.a * outlineAlpha * (1.0 - fontAlpha));
    }
    else // draw shadow
    {
        gl_FragColor = v_fragmentColor * vec4(u_effectColor.rgb, u_effectColor.a * outlineAlpha);
    }
}
// cbuffer PSConstants {
//     float4 u_effectColor;
//     float4 u_textColor;
//     int u_effectType;
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
//     float4 sample = u_texture.Sample(u_texture_sampler, PSIn.v_texCoord);
//     float fontAlpha = sample.a;
//     float outlineAlpha = sample.r;
//     float4 Color = PSIn.v_fragmentColor;
//     if (u_effectType == 0) {
//         Color *= float4(u_textColor.rgb, u_textColor.a * fontAlpha);
//     } else if (u_effectType == 1) {
//         Color *= float4(u_effectColor.rgb, u_effectColor.a * outlineAlpha * (1.0 - fontAlpha));
//     } else {
//         Color *= float4(u_effectColor.rgb, u_effectColor.a * outlineAlpha);
//     }
//     PSOut.Color = Color;
// }
