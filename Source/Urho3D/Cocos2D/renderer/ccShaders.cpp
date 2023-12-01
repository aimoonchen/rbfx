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
const char* positionColor_vert                   = "positionColor.vert";
const char* positionColor_frag                   = "positionColor.frag";
const char* positionTexture_vert                 = "positionTexture.vert";
const char* positionTexture_frag                 = "positionTexture.frag";
const char* positionTextureColor_vert            = "positionTextureColor.vert";
const char* positionTextureColor_frag            = "positionTextureColor.frag";
const char* positionTextureColorAlphaTest_frag   = "positionTextureColorAlphaTest.frag";
const char* label_normal_frag                    = "label_normal.frag";
const char* label_distanceNormal_frag            = "label_distanceNormal.frag";
const char* labelOutline_frag                    = "label_outline.frag";
const char* labelDistanceFieldGlow_frag          = "label_distanceFieldGlow.frag";
const char* positionColorLengthTexture_vert      = "positionColorLengthTexture.vert";
const char* positionColorLengthTexture_frag      = "positionColorLengthTexture.frag";
const char* positionColorTextureAsPointsize_vert = "positionColorTextureAsPointsize.vert";
const char* position_vert                        = "position.vert";
const char* positionNoMVP_vert                   = "positionNoMVP.vert";
const char* layer_radialGradient_frag            = "layer_radialGradient.frag";
const char* grayScale_frag                       = "ui_Gray.frag";
const char* positionTextureUColor_vert           = "positionTextureUColor.vert";
const char* positionTextureUColor_frag           = "positionTextureUColor.frag";
const char* positionUColor_vert                  = "positionUColor.vert";
const char* positionUColor_frag                  = "positionUColor.frag";
const char* etc1_frag                            = "etc1.frag";
const char* etc1Gray_frag                        = "etc1_Gray.frag";
const char* cameraClear_vert                     = "cameraClear.vert";
const char* cameraClear_frag                     = "cameraClear.frag";
const char* lineColor3D_frag                     = "lineColor.frag";
const char* lineColor3D_vert                     = "lineColor.vert";

NS_CC_END
