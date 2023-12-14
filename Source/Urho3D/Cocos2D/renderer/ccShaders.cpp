/****************************************************************************
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2012 		cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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
#include "renderer/ccShaders.h"

NS_CC_BEGIN

// backend shader file
const char* positionColor_vert                   = "fairygui/positionColor_vert";
const char* positionColor_frag                   = "fairygui/positionColor_frag";
const char* positionTexture_vert                 = "fairygui/positionTexture_vert";
const char* positionTexture_frag                 = "fairygui/positionTexture_frag";
const char* positionTextureColor_vert            = "fairygui/positionTextureColor_vert";
const char* positionTextureColor_frag            = "fairygui/positionTextureColor_frag";
const char* positionTextureColorAlphaTest_frag   = "fairygui/positionTextureColorAlphaTest_frag";
const char* label_normal_frag                    = "fairygui/label_normal_frag";
const char* label_distanceNormal_frag            = "fairygui/label_distanceNormal_frag";
const char* labelOutline_frag                    = "fairygui/label_outline_frag";
const char* labelDistanceFieldGlow_frag          = "fairygui/label_distanceFieldGlow_frag";
const char* positionColorLengthTexture_vert      = "fairygui/positionColorLengthTexture_vert";
const char* positionColorLengthTexture_frag      = "fairygui/positionColorLengthTexture_frag";
const char* positionColorTextureAsPointsize_vert = "fairygui/positionColorTextureAsPointsize_vert";
const char* position_vert                        = "fairygui/position_vert";
const char* positionNoMVP_vert                   = "fairygui/positionNoMVP_vert";
const char* layer_radialGradient_frag            = "fairygui/layer_radialGradient_frag";
const char* grayScale_frag                       = "fairygui/ui_Gray_frag";
const char* positionTextureUColor_vert           = "fairygui/positionTextureUColor_vert";
const char* positionTextureUColor_frag           = "fairygui/positionTextureUColor_frag";
const char* positionUColor_vert                  = "fairygui/positionUColor_vert";
const char* positionUColor_frag                  = "fairygui/positionUColor_frag";
const char* etc1_frag                            = "fairygui/etc1_frag";
const char* etc1Gray_frag                        = "fairygui/etc1_Gray_frag";
const char* cameraClear_vert                     = "fairygui/cameraClear_vert";
const char* cameraClear_frag                     = "fairygui/cameraClear_frag";
const char* lineColor3D_frag                     = "fairygui/lineColor_frag";
const char* lineColor3D_vert                     = "fairygui/lineColor_vert";

// const char* positionColor_vert                   = "Shaders/HLSL/fairygui/positionColor.vert";
// const char* positionColor_frag                   = "Shaders/HLSL/fairygui/positionColor.frag";
// const char* positionTexture_vert                 = "Shaders/HLSL/fairygui/positionTexture.vert";
// const char* positionTexture_frag                 = "Shaders/HLSL/fairygui/positionTexture.frag";
// const char* positionTextureColor_vert            = "Shaders/HLSL/fairygui/positionTextureColor.vert";
// const char* positionTextureColor_frag            = "Shaders/HLSL/fairygui/positionTextureColor.frag";
// const char* positionTextureColorAlphaTest_frag   = "Shaders/HLSL/fairygui/positionTextureColorAlphaTest.frag";
// const char* label_normal_frag                    = "Shaders/HLSL/fairygui/label_normal.frag";
// const char* label_distanceNormal_frag            = "Shaders/HLSL/fairygui/label_distanceNormal.frag";
// const char* labelOutline_frag                    = "Shaders/HLSL/fairygui/label_outline.frag";
// const char* labelDistanceFieldGlow_frag          = "Shaders/HLSL/fairygui/label_distanceFieldGlow.frag";
// const char* positionColorLengthTexture_vert      = "Shaders/HLSL/fairygui/positionColorLengthTexture.vert";
// const char* positionColorLengthTexture_frag      = "Shaders/HLSL/fairygui/positionColorLengthTexture.frag";
// const char* positionColorTextureAsPointsize_vert = "Shaders/HLSL/fairygui/positionColorTextureAsPointsize.vert";
// const char* position_vert                        = "Shaders/HLSL/fairygui/position.vert";
// const char* positionNoMVP_vert                   = "Shaders/HLSL/fairygui/positionNoMVP.vert";
// const char* layer_radialGradient_frag            = "Shaders/HLSL/fairygui/layer_radialGradient.frag";
// const char* grayScale_frag                       = "Shaders/HLSL/fairygui/ui_Gray.frag";
// const char* positionTextureUColor_vert           = "Shaders/HLSL/fairygui/positionTextureUColor.vert";
// const char* positionTextureUColor_frag           = "Shaders/HLSL/fairygui/positionTextureUColor.frag";
// const char* positionUColor_vert                  = "Shaders/HLSL/fairygui/positionUColor.vert";
// const char* positionUColor_frag                  = "Shaders/HLSL/fairygui/positionUColor.frag";
// const char* etc1_frag                            = "Shaders/HLSL/fairygui/etc1.frag";
// const char* etc1Gray_frag                        = "Shaders/HLSL/fairygui/etc1_Gray.frag";
// const char* cameraClear_vert                     = "Shaders/HLSL/fairygui/cameraClear.vert";
// const char* cameraClear_frag                     = "Shaders/HLSL/fairygui/cameraClear.frag";
// const char* lineColor3D_frag                     = "Shaders/HLSL/fairygui/lineColor.frag";
// const char* lineColor3D_vert                     = "Shaders/HLSL/fairygui/lineColor.vert";
NS_CC_END
