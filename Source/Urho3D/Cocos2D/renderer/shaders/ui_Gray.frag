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
 
const char* grayScale_frag = R"(
// #ifdef GL_ES
// precision mediump float;
// #endif

// varying vec4 v_fragmentColor;
// varying vec2 v_texCoord;

// uniform sampler2D u_texture;

// void main(void)
// {
//     vec4 c = texture2D(u_texture, v_texCoord);
//      c = v_fragmentColor * c;
//     gl_FragColor.xyz = vec3(0.2126*c.r + 0.7152*c.g + 0.0722*c.b);
//     gl_FragColor.w = c.w;
// }
Texture2D    u_texture;
SamplerState u_texture_sampler;
struct PSInput {
    float4 Pos              : SV_POSITION;
    float2 v_texCoord       : TEX_COORD;
    float4 v_fragmentColor  : COLOR0;
};
struct PSOutput {
    float4 Color : SV_TARGET;
};
void main(in PSInput PSIn, out PSOutput PSOut)
{
    float4 Color = PSIn.v_fragmentColor * u_texture.Sample(u_texture_sampler, PSIn.v_texCoord);
    float gray = 0.2126*Color.r + 0.7152*Color.g + 0.0722*Color.b;
    PSOut.Color = float4(gray, gray, gray, Color.a);
}
)";