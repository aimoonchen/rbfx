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

const char* positionTextureColor_vert = R"(
// attribute vec4 a_position;
// attribute vec2 a_texCoord;
// attribute vec4 a_color;

// uniform mat4 u_MVPMatrix;

// #ifdef GL_ES
// varying lowp vec4 v_fragmentColor;
// varying mediump vec2 v_texCoord;
// #else
// varying vec4 v_fragmentColor;
// varying vec2 v_texCoord;
// #endif

// void main()
// {
//     gl_Position = u_MVPMatrix * a_position;
//     v_fragmentColor = a_color;
//     v_texCoord = a_texCoord;
// }
cbuffer VSConstants {
    float4x4 u_MVPMatrix;
};
struct VSInput {
    float3 a_position   : ATTRIB0;
    float4 a_color      : ATTRIB1;
    float2 a_texCoord   : ATTRIB2;
};
struct PSInput {
    float4 Pos              : SV_POSITION;
    float2 v_texCoord       : TEX_COORD;
    float4 v_fragmentColor  : TEX_COOR1;
};
void main(in VSInput VSIn, out PSInput PSIn) 
{
    PSIn.Pos                = mul(u_MVPMatrix, float4(VSIn.a_position, 1.0));
    PSIn.v_texCoord         = VSIn.a_texCoord;
    PSIn.v_fragmentColor    = VSIn.a_color;
}
)";

