/*
 * cocos2d for iPhone: http://www.cocos2d-iphone.org
 *
 * Copyright (c) 2011 Brian Chapados
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

const char* positionTextureColorAlphaTest_frag = R"(
// #ifdef GL_ES
// precision lowp float;
// #endif

// varying vec4 v_fragmentColor;
// varying vec2 v_texCoord;

// uniform float u_alpha_value;
// uniform sampler2D u_texture;

// void main()
// {
//     vec4 texColor = texture2D(u_texture, v_texCoord);

// // mimic: glAlphaFunc(GL_GREATER)
// // pass if ( incoming_pixel >= u_alpha_value ) => fail if incoming_pixel < u_alpha_value

//     if ( texColor.a <= u_alpha_value )
//         discard;

//     gl_FragColor = texColor * v_fragmentColor;
// }
cbuffer Constants {
    float4 g_AlphaValue;
};
Texture2D    g_Texture;
SamplerState g_Texture_sampler;
struct PSInput {
    float4 Pos      : SV_POSITION;
    float2 UV       : TEX_COORD;
    float4 Color    : TEX_COOR1;
};
struct PSOutput {
    float4 Color : SV_TARGET;
};
void main(in  PSInput  PSIn, out PSOutput PSOut)
{
    float4 Color = g_Texture.Sample(g_Texture_sampler, PSIn.UV);
    if (Color.a <= g_AlphaValue)
        discard;
#if CONVERT_PS_OUTPUT_TO_GAMMA
    // Use fast approximation for gamma correction.
    Color.rgb = pow(Color.rgb, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
#endif
    PSOut.Color = PSIn.Color * Color;
}
)";
