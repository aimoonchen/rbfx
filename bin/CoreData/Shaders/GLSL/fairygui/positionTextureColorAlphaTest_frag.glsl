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
#include "../common.glsl"

in vec4 v_fragmentColor;
in vec2 v_texCoord;

_LAYOUT(1) uniform PSConstants {
    float u_alpha_value;
};
_LAYOUT(0) uniform sampler2D u_texture;
out vec4 _output;
#define gl_FragColor _output
void main()
{
    vec4 texColor = texture(u_texture, v_texCoord);

// mimic: glAlphaFunc(GL_GREATER)
// pass if ( incoming_pixel >= u_alpha_value ) => fail if incoming_pixel < u_alpha_value

    if ( texColor.a <= u_alpha_value )
        discard;

    gl_FragColor = texColor * v_fragmentColor;
    #ifdef URHO3D_LINEAR_OUTPUT
        gl_FragColor.rgb *= gl_FragColor.rgb * (gl_FragColor.rgb * 0.305306011 + 0.682171111) + 0.012522878;
    #endif
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
