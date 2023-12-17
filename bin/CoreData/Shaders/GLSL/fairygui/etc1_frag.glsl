/*
 * cocos2d for iPhone: http://www.cocos2d-iphone.org
 *
 * Copyright (c) 2011 Ricardo Quesada
 * Copyright (c) 2012 Zynga Inc.
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

in vec4 v_fragmentColor;
in vec2 v_texCoord;

layout(binding = 0) uniform sampler2D u_texture;
layout(binding = 1) uniform sampler2D u_texture1;
layout(location = 0) out vec4 _output;
#define gl_FragColor _output
void main() {
    vec4 texColor = vec4(texture(u_texture, v_texCoord).rgb, texture(u_texture1, v_texCoord).r);

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
