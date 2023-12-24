/****************************************************************************
 Copyright (c) 2016 Chukong Technologies Inc.

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

layout(binding = 1) uniform PSConstants {
    vec4 u_startColor;
    vec4 u_endColor;
    vec2 u_center;
    float u_radius;
    float u_expand;
};

in vec4 v_position;
layout(location = 0) out vec4 _output;
#define gl_FragColor _output
void main()
{
    float d = distance(v_position.xy, u_center) / u_radius;
    if (d <= 1.0)
    {
        if (d <= u_expand)
        {
            gl_FragColor = u_startColor;
        }
        else
        {
            gl_FragColor = mix(u_startColor, u_endColor, (d - u_expand) / (1.0 - u_expand));
        }
    }
    else
    {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
    #ifdef URHO3D_LINEAR_OUTPUT
        gl_FragColor.rgb *= gl_FragColor.rgb * (gl_FragColor.rgb * 0.305306011 + 0.682171111) + 0.012522878;
    #endif
}
// cbuffer PSConstants {
//     float4 u_startColor;
//     float4 u_endColor;
//     float2 u_center;
//     float u_radius;
//     float u_expand;
// };
// struct PSInput {
//     float4 Pos          : SV_POSITION;
//     float4 v_position   : TEX_COORD;
// };
// struct PSOutput {
//     float4 Color : SV_TARGET;
// };
// void main(in PSInput PSIn, out PSOutput PSOut)
// {
//     float d = distance(PSIn.v_position.xy, u_center) / u_radius;
//     float4 Color;
//     if (d <= 1.0) {
//         if (d <= u_expand) {
//             Color = u_startColor;
//         } else {
//             Color = lerp(u_startColor, u_endColor, (d - u_expand) / (1.0 - u_expand));
//         }
//     } else {
//         Color = float4(0.0, 0.0, 0.0, 0.0);
//     }
//     PSOut.Color = Color;
// }
