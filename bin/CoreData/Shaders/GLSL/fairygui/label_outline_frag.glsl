/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
 
/*
 * LICENSE ???
 */
 #ifdef GL_ES
    precision highp float;
    #define _LAYOUT(index)
#else
    #define _LAYOUT(index) layout(binding=index)
#endif
in vec4 v_fragmentColor;
in vec2 v_texCoord;

_LAYOUT(1) uniform PSConstants {
    vec4 u_effectColor;
    vec4 u_textColor;
    int u_effectType;
};
_LAYOUT(0) uniform sampler2D u_texture;
out vec4 _output;
#define gl_FragColor _output
void main()
{
    vec4 texColor = texture(u_texture, v_texCoord);
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
    #ifdef URHO3D_LINEAR_OUTPUT
        gl_FragColor.rgb *= gl_FragColor.rgb * (gl_FragColor.rgb * 0.305306011 + 0.682171111) + 0.012522878;
    #endif
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
