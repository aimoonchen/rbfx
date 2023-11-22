/*
 * Copyright (c) 2013-2017 Chukong Technologies Inc.
 *
 * http://www.cocos2d-x.org
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

const char* positionColorTextureAsPointsize_vert = R"(
// attribute vec4 a_position;
// attribute vec4 a_color;

// attribute vec2 a_texCoord;

// #ifdef GL_ES
// varying lowp vec4 v_fragmentColor;
// #else
// varying vec4 v_fragmentColor;
// #endif
// uniform float u_alpha;
// uniform mat4 u_MVPMatrix;

// void main()
// {
//     gl_Position = u_MVPMatrix * a_position;
//     gl_PointSize = a_texCoord.x;
//     v_fragmentColor = vec4(a_color.rgb * a_color.a * u_alpha, a_color.a * u_alpha);
// }
cbuffer VSConstants {
    float4x4 u_MVPMatrix;
    float u_alpha;
};
struct VSInput {
    float3 a_position   : ATTRIB0;
    float2 a_texCoord   : ATTRIB1;
    float4 a_color      : ATTRIB2;
};
struct PSInput {
    float4 Pos              : SV_POSITION;
    float4 v_fragmentColor  : TEX_COORD;
    float pointSize         : PSIZE;
};
void main(in VSInput VSIn, out PSInput PSIn)
{
    PSIn.Pos                = mul(float4(VSIn.a_position,1.0), u_MVPMatrix);
    PSIn.pointSize          = VSIn.a_texCoord.x;
    PSIn.v_fragmentColor    = float4(VSIn.a_color.rgb * u_alpha * VSIn.a_color.a, u_alpha * VSIn.a_color.a);
}
)";
