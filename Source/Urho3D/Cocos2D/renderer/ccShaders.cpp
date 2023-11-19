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
//
//
const char* positionColor_vert = "FairyGUI/vs_positionColor";
const char* positionColor_frag = "FairyGUI/fs_positionColor";
const char* positionTexture_vert = "FairyGUI/vs_positionTexture";
const char* positionTexture_frag = "FairyGUI/fs_positionTexture";
const char* positionTextureColor_vert = "FairyGUI/vs_positionTextureColor";
const char* positionTextureColor_frag = "FairyGUI/fs_positionTextureColor";
const char* positionTextureColorAlphaTest_frag = "FairyGUI/fs_positionTextureColorAlphaTest";
const char* labelNormal_frag = "FairyGUI/fs_labelNormal";
const char* labelDistanceNormal_frag = "FairyGUI/fs_labelDistanceNormal";
const char* labelOutline_frag = "FairyGUI/fs_labelOutline";
const char* labelDistanceFieldGlow_frag = "FairyGUI/fs_labelDistanceFieldGlow";
// const char* lineColor3D_frag = "fs_lineColor3D";
// const char* lineColor3D_vert = "vs_lineColor3D";
const char* positionColorLengthTexture_vert = "FairyGUI/vs_positionColorLengthTexture";
const char* positionColorLengthTexture_frag = "FairyGUI/fs_positionColorLengthTexture";
const char* positionColorTextureAsPointsize_vert = "FairyGUI/vs_positionColorTextureAsPointsize";
const char* position_vert = "FairyGUI/vs_position";
const char* positionNoMVP_vert = "FairyGUI/vs_positionNoMVP";
const char* layerRadialGradient_frag = "FairyGUI/fs_layerRadialGradient";
const char* grayScale_frag = "FairyGUI/fs_grayScale";
const char* positionTextureUColor_vert = "FairyGUI/vs_positionTextureUColor";
const char* positionTextureUColor_frag = "FairyGUI/fs_positionTextureUColor";
const char* positionUColor_vert = "FairyGUI/vs_positionUColor";
const char* positionUColor_frag = "FairyGUI/fs_positionUColor";
const char* etc1_frag = "FairyGUI/fs_etc1";
const char* etc1Gray_frag = "FairyGUI/fs_etc1Gray";
const char* cameraClear_vert = "FairyGUI/vs_cameraClear";
const char* cameraClear_frag = "FairyGUI/fs_cameraClear";

// backend shader file
// #include "renderer/shaders/positionColor.vert"
// #include "renderer/shaders/positionColor.frag"
// #include "renderer/shaders/positionTexture.vert"
// #include "renderer/shaders/positionTexture.frag"
// #include "renderer/shaders/positionTextureColor.vert"
// #include "renderer/shaders/positionTextureColor.frag"
// #include "renderer/shaders/positionTextureColorAlphaTest.frag"
// #include "renderer/shaders/label_normal.frag"
// #include "renderer/shaders/label_distanceNormal.frag"
// #include "renderer/shaders/label_outline.frag"
// #include "renderer/shaders/label_distanceFieldGlow.frag"
// #include "renderer/shaders/positionColorLengthTexture.vert"
// #include "renderer/shaders/positionColorLengthTexture.frag"
// #include "renderer/shaders/positionColorTextureAsPointsize.vert"
// #include "renderer/shaders/position.vert"
// #include "renderer/shaders/positionNoMVP.vert"
// #include "renderer/shaders/layer_radialGradient.frag"
// #include "renderer/shaders/ui_Gray.frag"
// #include "renderer/shaders/positionTextureUColor.vert"
// #include "renderer/shaders/positionTextureUColor.frag"
// #include "renderer/shaders/positionUColor.vert"
// #include "renderer/shaders/positionUColor.frag"
// #include "renderer/shaders/etc1.frag"
// #include "renderer/shaders/etc1_Gray.frag"
// #include "renderer/shaders/cameraClear.vert"
// #include "renderer/shaders/cameraClear.frag"
// 
// 
// #include "renderer/shaders/3D_color.frag"
// #include "renderer/shaders/3D_colorNormal.frag"
// #include "renderer/shaders/3D_colorNormalTexture.frag"
// #include "renderer/shaders/3D_colorTexture.frag"
// #include "renderer/shaders/3D_particle.vert"
// #include "renderer/shaders/3D_particle.frag"
// #include "renderer/shaders/3D_positionNormalTexture.vert"
// #include "renderer/shaders/3D_positionTexture.vert"
// #include "renderer/shaders/3D_skybox.vert"
// #include "renderer/shaders/3D_skybox.frag"
// #include "renderer/shaders/3D_terrain.frag"
// #include "renderer/shaders/3D_terrain.vert"
// 
// #include "renderer/shaders/lineColor.frag"
// #include "renderer/shaders/lineColor.vert"

NS_CC_END
