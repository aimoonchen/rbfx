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
#include "../common.glsl"

in vec4 v_color;
in vec2 v_texcoord;
out vec4 _output;
#define gl_FragColor _output
void main()
{
// #if defined GL_OES_standard_derivatives
// gl_FragColor = v_color*smoothstep(0.0, length(fwidth(v_texcoord)), 1.0 - length(v_texcoord));
// #else
    gl_FragColor = v_color*step(0.0, 1.0 - length(v_texcoord));
// #endif
#ifdef URHO3D_LINEAR_OUTPUT
    gl_FragColor.rgb *= gl_FragColor.rgb * (gl_FragColor.rgb * 0.305306011 + 0.682171111) + 0.012522878;
#endif
}
// struct PSInput {
//     float4 Pos          : SV_POSITION;
//     float2 v_texcoord   : TEX_COORD;
//     float4 v_color      : COLOR0;
// };
// struct PSOutput {
//     float4 Color : SV_TARGET;
// };
// void main(in PSInput PSIn, out PSOutput PSOut)
// {
//     PSOut.Color = PSIn.v_color * step(0.0, 1.0 - length(PSIn.v_texcoord));
// }
