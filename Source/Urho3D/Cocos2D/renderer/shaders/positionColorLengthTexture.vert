/* Copyright (c) 2012 Scott Lembcke and Howling Moon Software
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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

const char* positionColorLengthTexture_vert = R"(
// #ifdef GL_ES
// precision lowp float;
// #endif

// #ifdef GL_ES
// attribute mediump vec4 a_position;
// attribute mediump vec2 a_texCoord;
// attribute mediump vec4 a_color;

// varying mediump vec4 v_color;
// varying mediump vec2 v_texcoord;

// #else

// attribute vec4 a_position;
// attribute vec2 a_texCoord;
// attribute vec4 a_color;

// varying vec4 v_color;
// varying vec2 v_texcoord;

// #endif

// uniform float u_alpha;
// uniform mat4 u_MVPMatrix;

// void main()
// {
//     v_color = vec4(a_color.rgb * a_color.a * u_alpha, a_color.a * u_alpha);
//     v_texcoord = a_texCoord;

//     gl_Position = u_MVPMatrix * a_position;
// }

cbuffer VSConstants {
    float4x4 u_MVPMatrix;
    float u_alpha;
};
struct VSInput {
    float3 a_position   : ATTRIB0;
    float4 a_color      : ATTRIB1;
    float2 a_texCoord   : ATTRIB2;
};
struct PSInput {
    float4 Pos          : SV_POSITION;
    float2 v_texcoord   : TEX_COORD;
    float4 v_color      : COLOR0;
};
void main(in VSInput VSIn, out PSInput PSIn) 
{
    PSIn.Pos        = mul(u_MVPMatrix, float4(VSIn.a_position, 1.0));
    PSIn.v_texcoord = VSIn.a_texCoord;
    PSIn.v_color    = float4(VSIn.a_color.rgb * VSIn.a_color.a * u_alpha, VSIn.a_color.a * u_alpha);
}
)";