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

// #ifdef GL_ES
// precision lowp float;
// #endif

// uniform vec4 u_color;
// uinform sampler2D u_texture;

// varying vec2 v_texCoord;

// void main()
// {
//     gl_FragColor =  texture2D(u_texture, v_texCoord) * u_color;
// }
cbuffer PSConstants {
    float4 u_color;
};
Texture2D    u_texture;
SamplerState u_texture_sampler;
struct PSInput {
    float4 Pos          : SV_POSITION;
    float2 v_texCoord   : TEX_COORD;
};
struct PSOutput {
    float4 Color : SV_TARGET;
};
void main(in PSInput PSIn, out PSOutput PSOut)
{
    PSOut.Color = u_color * u_texture.Sample(u_texture_sampler, PSIn.v_texCoord);
}